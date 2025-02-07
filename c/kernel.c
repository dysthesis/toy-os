// Include the SBI to interact with the hardware
#include "kernel.h"
#include "common.h"
#include "malloc.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

// Import these variables from the linker script (kernel.ld)
extern char __bss[], __bss_end[], __stack_top[];

// Call stuff from the SBI
struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid /* SBI function ID */,
                       long eid /* SBI extension ID */) {
  // Pass the arguments to the appropriate registers in asm
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid;
  register long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6),
                         "r"(a7)
                       : "memory");
  return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char c) { sbi_call(c, 0, 0, 0, 0, 0, 0, 1 /* console putchar */); }

// We need to 4-byte align this since `stvec` holds
//
// - the address to the exception handler, and
// - the flags representing th emode in its lower 2 bits.
__attribute__((naked)) __attribute__((aligned(4))) void kernel_entry(void) {
  __asm__ __volatile__(
      "csrw	sscratch,	sp\n" /* sscratch is a temp register to save the
                               stack pointer at time of exception occurrence */
      "addi	sp, 			sp, -4 * 31\n" /* sp -= 4 * 31 */

      // Save the program state during the time of exception
      "sw 	ra,				4 * 0(sp)\n"
      "sw 	gp,  			4 * 1(sp)\n"
      "sw 	tp,  			4 * 2(sp)\n"
      "sw 	t0,  			4 * 3(sp)\n"
      "sw 	t1,  			4 * 4(sp)\n"
      "sw 	t2,  			4 * 5(sp)\n"
      "sw 	t3,  			4 * 6(sp)\n"
      "sw 	t4,  			4 * 7(sp)\n"
      "sw 	t5,  			4 * 8(sp)\n"
      "sw 	t6,  			4 * 9(sp)\n"
      "sw 	a0,  			4 * 10(sp)\n"
      "sw 	a1,  			4 * 11(sp)\n"
      "sw 	a2,  			4 * 12(sp)\n"
      "sw 	a3,  			4 * 13(sp)\n"
      "sw 	a4,  			4 * 14(sp)\n"
      "sw 	a5,  			4 * 15(sp)\n"
      "sw 	a6,  			4 * 16(sp)\n"
      "sw 	a7,  			4 * 17(sp)\n"
      "sw 	s0,  			4 * 18(sp)\n"
      "sw 	s1,  			4 * 19(sp)\n"
      "sw 	s2,  			4 * 20(sp)\n"
      "sw 	s3,  			4 * 21(sp)\n"
      "sw 	s4,  			4 * 22(sp)\n"
      "sw 	s5,  			4 * 23(sp)\n"
      "sw 	s6,  			4 * 24(sp)\n"
      "sw 	s7,  			4 * 25(sp)\n"
      "sw 	s8,  			4 * 26(sp)\n"
      "sw 	s9,  			4 * 27(sp)\n"
      "sw 	s10, 			4 * 28(sp)\n"
      "sw		s11, 			4 * 29(sp)\n"

      "csrr a0,				sscratch\n"
      "sw		a0, 			4 * 30(sp)\n"

      // Call exception handler
      "mv		a0,				sp\n"
      "call	handle_trap\n"

      // Restore the program state, presumably for error handling
      "lw		ra,				4 * 0(sp)\n"
      "lw		gp,				4 * 1(sp)\n"
      "lw		tp,				4 * 2(sp)\n"
      "lw		t0,				4 * 3(sp)\n"
      "lw		t1,				4 * 4(sp)\n"
      "lw		t2,				4 * 5(sp)\n"
      "lw		t3,				4 * 6(sp)\n"
      "lw		t4,				4 * 7(sp)\n"
      "lw		t5,				4 * 8(sp)\n"
      "lw		t6,				4 * 9(sp)\n"
      "lw		a0,				4 * 10(sp)\n"
      "lw		a1,				4 * 11(sp)\n"
      "lw		a2,				4 * 12(sp)\n"
      "lw		a3,				4 * 13(sp)\n"
      "lw		a4,				4 * 14(sp)\n"
      "lw		a5,				4 * 15(sp)\n"
      "lw		a6,				4 * 16(sp)\n"
      "lw		a7,				4 * 17(sp)\n"
      "lw		s0,				4 * 18(sp)\n"
      "lw		s1,				4 * 19(sp)\n"
      "lw		s2,				4 * 20(sp)\n"
      "lw		s3,				4 * 21(sp)\n"
      "lw		s4,				4 * 22(sp)\n"
      "lw		s5,				4 * 23(sp)\n"
      "lw		s6,				4 * 24(sp)\n"
      "lw		s7,				4 * 25(sp)\n"
      "lw		s8,				4 * 26(sp)\n"
      "lw		s9,				4 * 27(sp)\n"
      "lw		s10,			4 * 28(sp)\n"
      "lw		s11,			4 * 29(sp)\n"
      "lw		sp,				4 * 30(sp)\n"
      "sret\n");
}

void handle_trap(struct trap_frame *f) {
  // Type of exception
  uint32_t scause = READ_CSR(scause);

  // Additional info, e.g. memory address of the root cause.
  uint32_t stval = READ_CSR(stval);

  // Program counter of the exception
  uint32_t user_pc = READ_CSR(sepc);

  PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval,
        user_pc);
}

// NOTE: The act ual main function
void kernel_main(void) {
  // WARN: This  might be dangerous, as we're casting __bss_end and __bss to a
  // smaller typ e
  size_t bss_size = (size_t)__bss_end - (size_t)__bss;
  memset(__bss, 0, bss_size);

  paddr_t paddr0 = alloc_pages(2);
  paddr_t paddr1 = alloc_pages(1);
  printf("alloc_pages test: paddr0=%x\n", paddr0);
  printf("alloc_pages test: paddr1=%x\n", paddr1);

  // Tell the CPU where the exception handler is
  WRITE_CSR(stvec, (uint32_t)kernel_entry);

  // Test out the exception handler with this invalid instruction
  __asm__ __volatile__("unimp");

  PANIC("booted!");
}

// NOTE: the ker nel boots from here
__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  // This is just inline assembly
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n" // set the stack pointer
      "j kernel_main\n"       // run the `kernel_main()` fucntion above
      :
      // Pass in  `__stack_top` as `stac_top` in the inline asm
      : [stack_top] "r"(__stack_top));
}
