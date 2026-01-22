// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}


// show free memory available right now
uint64 freemem(void) {
  int free_pages = 0;
  struct run *r;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while (r != 0) {
    free_pages ++;
    r = r->next;
  }
  release(&kmem.lock);
  return (uint64)free_pages * PGSIZE / 1024 / 1024;  // in MiB
}


uint64 occup(int argc, char *argv[]) {
      if (argc < 2) {
        fprintf(2, "Usage: occup {number of pages}...\n");
        exit(1);
    }

    int num_pages = atoi(argv[1]);
    if(num_pages <= 0) {
        fprintf(2, "Error, enter positive number of pages!");
        exit(1);
    }

    int bytes = num_pages * 4096;

    int rc = fork();

    if (rc < 0) {
        printf("fork failed");
    }
    else if (rc == 0) {
        char *p = sbrk(bytes);
        if (*p == (char) - 1) {
            printf("sbrk failed. exiting\n");
            exit(1);
        }
        printf("Occupied memory: %d\n", bytes);
    }
    else {
        exit(0);
    }
    while (1) {
        sleep(1000, &kmem.lock);
    }
    exit(0);
}