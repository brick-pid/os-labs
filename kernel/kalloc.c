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

struct run
{
  struct run *next;
};

struct
{
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void kinit()
{
  push_off();
  int cpu_id = cpuid();
  initlock(&kmem[cpu_id].lock, "kmem");
  freerange(end, (void *)PHYSTOP);
  pop_off();
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  //关中断
  push_off();
  int cpu_id = cpuid();
  pop_off();

  acquire(&kmem[cpu_id].lock);
  r->next = kmem[cpu_id].freelist;
  kmem[cpu_id].freelist = r;
  release(&kmem[cpu_id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int cpu_id = cpuid();
  pop_off();

  acquire(&kmem[cpu_id].lock);
  r = kmem[cpu_id].freelist;
  int find = 0;
  if (r)
  {
    kmem[cpu_id].freelist = r->next;
  }
  release(&kmem[cpu_id].lock);

  if (!r)
  {
    //本cpu的freelist消耗完了，从其他cpu的freelist拿
    for (int i = 0; i < NCPU; i++)
    {
      acquire(&kmem[i].lock);
      r = kmem[i].freelist;
      if (r)
      {
        kmem[i].freelist = r->next;
        find = 1;
      }
      release(&kmem[i].lock);
      if (find)
        break;
    }
  }

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}
