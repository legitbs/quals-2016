#include "unistd.h"

asm(".globl write\n\t"
    ".type write, @function\n\t"
    "write:\n\t"
    ".cfi_startproc\n\t"
    "movl $1, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
