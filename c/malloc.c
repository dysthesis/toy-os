#include "malloc.h"
#include "common.h"
#include "kernel.h"

// Import the memory segment address from the linker script `kernel.ld`
extern char __free_ram[], __free_ram_end[];

paddr_t alloc_pages(uint32_t n) {
  static paddr_t next = (paddr_t)__free_ram;
  paddr_t curr = next;
  next += n * PAGE_SIZE;

  if (next > (paddr_t)__free_ram_end)
    PANIC("out of memory");

  memset((void *)curr, 0, n * PAGE_SIZE);
  return curr;
}
