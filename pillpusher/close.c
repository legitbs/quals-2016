#include "unistd.h"

asm(".globl close\n\t"
    ".type close, @function\n\t"
    "close:\n\t"
    ".cfi_startproc\n\t"
    "movl $3, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
