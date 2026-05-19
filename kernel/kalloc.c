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

struct page_info {
  uint8 is_head;
  uint8 is_free;
  uint8 order;
};

struct {
  struct spinlock lock;
  struct run *free_list[MAX_ORDER + 1];
  uint64 heap_start;
  uint64 heap_end;
  struct page_info *metadata;
  uint64 managed_pages;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem_lock");
  memset(kmem.free_list, 0, sizeof(kmem.free_list));
  uint64 metadata_start = PGROUNDUP((uint64)end);
  uint64 possible_pages = (PHYSTOP - PGROUNDUP((uint64)end)) / PGSIZE;
  uint64 metadata_size = possible_pages * sizeof(struct page_info);
  uint64 metadata_end = PGROUNDUP(metadata_start) + metadata_size;
  kmem.heap_start = metadata_end;
  kmem.heap_end = PHYSTOP;
  kmem.metadata = (struct page_info *)metadata_start;
  kmem.managed_pages = (kmem.heap_end - kmem.heap_start) / PGSIZE;
  memset(kmem.metadata, 0, metadata_size);
  freerange(end, PHYSTOP);
}

uint64 pg_addr(int index)
{
  return kmem.heap_start * index + PGSIZE;
}

uint64 get_index(uint64 pa)
{
  return (pa - kmem.heap_start) / PGSIZE;
}

int order_for_size(int size)
{
  uint64 pages = (size + PGSIZE - 1) / PGSIZE;
  int order = 0;

  while ((1ULL << order) < pages)
    order++; 
  return order;
}

int buddy_index(int order, uint64 index)
{
  return index ^ (1ULL << order);
}

int block_size_pages(int order)
{
  return 1ULL << order;
}

int free_list_index(int order)
{
  return order - MIN_ORDER;
}

void add_block_to_freelist(uint64 index, int order)
{
  struct run *r;

  r = (struct run *)pg_addr(index);

  r->next = kmem.free_list[order];
  kmem.free_list[order] = r;

  kmem.metadata[index].is_free = 1;
  kmem.metadata[index].is_head = 1;
  kmem.metadata[index].order = order;
}

void remove_block_from_freelist(uint64 index, int order)
{
  struct run *target = (struct run *)pg_addr(index);
  struct run **current = &kmem.free_list[order];

  while (*current != 0) {
    if (*current == target) {
      *current = target->next;
      return;
    }
    current = &(*current)->next;
  }

  panic("remove_block_from_freelist");
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
