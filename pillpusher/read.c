#include "unistd.h"

asm(".globl read\n\t"
    ".type read, @function\n\t"
    "read:\n\t"
    ".cfi_startproc\n\t"
    "movl $0, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
