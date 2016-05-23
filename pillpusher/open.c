#include "unistd.h"

asm(".globl open\n\t"
    ".type open, @function\n\t"
    "open:\n\t"
    ".cfi_startproc\n\t"
    "movl $2, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
