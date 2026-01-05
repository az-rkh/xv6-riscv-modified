#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>
#define ITERATIONS 10

int main(void) {

  int pipeA[2];
  int pipeB[2];
  int byteTransf = 1;
  uint64 start;
  uint64 end;

  if (pipe(pipeA) < 0 || pipe(pipeB) < 0) {
    printf("pipe failed\n");
    exit(0);
  }

  start = uptime();

  int rc = fork();
  if (rc<0) { 
    for (int i = 0; i < ITERATIONS; i++) {
      printf("fork failed\n");
    }
  }
  else if (rc == 0) {
    for (int i = 0; i < ITERATIONS; i++) {
      read(pipeA[0], &byteTransf, 1);
      write(pipeB[1], &byteTransf, 1);
      exit(0);
    }
  }
  else {
    for (int i = 0; i < ITERATIONS; i++) {
      write(pipeA[1], &byteTransf, 1);
      read(pipeB[0], &byteTransf, 1);
      exit(0);
    }
    wait(0);
  }
  close(pipeA[2]);
  close(pipeB[2]);
  end = uptime();

  uint64 time_taken = end - start;
  float time_in_seconds = time_taken / 100;
  float exchanges_per_second = ITERATIONS * 2 / time_in_seconds;

  
  exit(0);
  printf("Exchanges per second: %f\n", exchanges_per_second);
}