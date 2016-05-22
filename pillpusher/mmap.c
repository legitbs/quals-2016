#include "mmap.h"

asm(".globl mmap\n\t"
    ".type mmap, @function\n\t"
    "mmap:\n\t"
    ".cfi_startproc\n\t"
    "push %r10\n\t"
    "mov %rcx, %r10\n\t"
    "movl $9, %eax\n\t"
    "syscall\n\t"
    "pop %r10\n\n"
    "ret\n\t"
    ".cfi_endproc");


asm(".globl munmap\n\t"
    ".type munmap, @function\n\t"
    "munmap:\n\t"
    ".cfi_startproc\n\t"
    "movl $11, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
