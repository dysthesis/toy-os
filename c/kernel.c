typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// Import these variables from the linker script (kernel.ld)
extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  while (n--)
    *p++ = c;
  return buf;
}

void kernel_main(void) {
  // WARN: This might be dangerous, as we're casting __bss_end and __bss to a
  // smaller type
  size_t bss_size = (size_t)__bss_end - (size_t)__bss;
  memset(__bss, 0, bss_size);

  for (;;)
    ;
}

// NOTE: the kernel boots from here
__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  // This is just inline assembly
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n" // set the stack pointer
      "j kernel_main\n"       // run the `kernel_main()` fucntion above
      :
      // Pass in `__stack_top` as `stac_top` in the inline asm
      : [stack_top] "r"(__stack_top));
}
