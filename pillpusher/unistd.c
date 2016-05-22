int write( int fd, const void*buf, size_t count)
{
	asm( "xor %rax, %rax\n\n"
	     "inc %rax\n\n"
	     "mov %rdi $1\n\n"
	

}
