//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define BACKSPACE 0x100  // erase the last output character
#define C(x)  ((x)-'@')  // Control-x
#define HISTSZ 16

char history[HISTSZ][100];
int hist_count = 0;
int hist_pos = 0;
//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printf and to echo input
// characters.
//
void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b'); uartputc_sync(' '); uartputc_sync('\b');
  } else {
    uartputc_sync(c);
  }
}

struct {
  struct spinlock lock;
  int esc_state;
  // input circular buffer
#define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
int
consolewrite(int user_src, uint64 src, int n)
{
  char buf[32]; // move batches from user space to uart.
  int i = 0;

  while(i < n){
    int nn = sizeof(buf);
    if(nn > n - i)
      nn = n - i;
    if(either_copyin(buf, user_src, src+i, nn) == -1)
      break;
    uartwrite(buf, nn);
    i += nn;
  }

  return i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(killed(myproc())){
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
void
consoleintr(int c)
{
  acquire(&cons.lock);

  switch(c){
  case C('P'):  // Print process list.
    procdump();
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF_SIZE] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f': // Delete key
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF_SIZE){
      c = (c == '\r') ? '\n' : c;

      // Handle escape sequences for arrow keys
      if (c == 0x1b) {
        cons.esc_state = 1;
        break;
      }
      if (cons.esc_state == 1) {
        if (c == '[') {
          cons.esc_state = 2;
        } else {
          cons.esc_state = 0;
        }
        break;
      }
      if (cons.esc_state == 2) {
        cons.esc_state = 0;
        if (c == 'A' || c == 'B') {  // Up or Down arrow
          // Calculate new history position
          int new_pos = hist_pos;
          if (c == 'A' && hist_pos > 0) {
            new_pos = hist_pos - 1;
          } else if (c == 'B' && hist_pos < hist_count) {
            new_pos = hist_pos + 1;
          }
          if (new_pos == hist_pos)
            break;
          hist_pos = new_pos;

          // Erase current line
          while (cons.e != cons.w && cons.buf[(cons.e-1) % INPUT_BUF_SIZE] != '\n') {
            cons.e--;
            consputc(BACKSPACE);
          }

          // Insert history entry (or empty if at end)
          if (hist_pos < hist_count) {
            char *cmd = history[hist_pos];
            while (*cmd) {
              cons.buf[cons.e++ % INPUT_BUF_SIZE] = *cmd;
              consputc(*cmd);
              cmd++;
            }
          }
        }
        break;
      }

      // Save command to history when newline is entered
      if (c == '\n') {
        // Copy current line to history
        int len = 0;
        uint i = cons.w;
        while (i < cons.e && len < 99) {
          char ch = cons.buf[i % INPUT_BUF_SIZE];
          if (ch != '\n')
            history[hist_count % HISTSZ][len++] = ch;
          i++;
        }
        if (len > 0) {
          history[hist_count % HISTSZ][len] = '\0';
          hist_count++;
          if (hist_count > HISTSZ)
            hist_count = HISTSZ;
        }
        hist_pos = hist_count;  // Reset to end of history
      }

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      if(c == '\n' || c == C('D') || cons.e-cons.r == INPUT_BUF_SIZE){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }
  
  release(&cons.lock);
}

void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
