#include "unistd.h"

asm(".globl exit\n\t"
    ".type exit, @function\n\t"
    "exit:\n\t"
    ".cfi_startproc\n\t"
    "movl $60, %eax\n\t"
    "syscall\n\t"
    "ret\n\t"
    ".cfi_endproc");
