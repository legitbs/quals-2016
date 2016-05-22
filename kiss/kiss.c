#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define WriteString(a) write(1, a, strlen(a))

inline int ReadLine(char *Buffer, int Size)
{
	int Count = 0;
	int TotalCount = 0;
	while(1)
	{
		Count = read(0, Buffer, 1);
		if(Count > 0)
		{
			TotalCount += Count;
			if(TotalCount >= Size)
				return TotalCount;
			else if(*Buffer == '\n')
			{
				*Buffer = 0;
				return TotalCount - 1;
			}
			Buffer += Count;
		}
		else
			_exit(0);
	};

	return TotalCount;
}

inline int ReadAll(char *Buffer, int Size)
{
	//read the number of bytes specified
	int Count = 0;
	int TotalCount = 0;
	while(Size)
	{
		Count = read(0, Buffer, Size);
		if(Count > 0)
		{
			Size -= Count;
			Buffer += Count;
			TotalCount += Count;
		}
		else
			_exit(0);
	};

	return TotalCount;
}

inline unsigned long ReadInt()
{
	char Buffer[30];
	ReadLine(Buffer, sizeof(Buffer) - 1);
	Buffer[sizeof(Buffer) - 1] = 0;
	if(strlen(Buffer) == 0)
		return -1;
	else
		return strtoul(Buffer, 0, 16);
}

inline void WriteHex(unsigned long Val)
{
	unsigned char CurChar;
	int i;

	for(i = 0; i < 16; i++)
	{
		CurChar = (Val >> 60) & 0x0f;
		Val <<= 4;
		if(CurChar <= 9)
			CurChar |= 0x30;
		else
			CurChar += 0x41-0x0a;
		write(1, &CurChar, 1);
	};
}

int main(int argc, char **argv)
{
	int fd;
	unsigned long seed;
	char *Buf;
	unsigned long BaseMem;

	WriteString("KISS - Keep It Simple Stupid\n");

	//reseed random generator
	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &seed, sizeof(seed));
	close(fd);

	//allocate a random amount of memory
	seed %= 2048;
	BaseMem = (unsigned long)(malloc(seed)) & ~4095;
	WriteString("Buffer is around ");
	WriteHex(BaseMem);
	WriteString("\n");

	WriteString("Binary is around ");
	WriteHex((unsigned long)main & ~4095);
	WriteString("\n");

	WriteString("How big of a buffer do you want? ");
	seed = ReadInt();
	if(seed > 0xa00)
	{
		WriteString("Invalid size\n");
		exit(0);
	}

	Buf = (char *)malloc(seed);
	WriteString("Waiting for data.\n");
	ReadAll(Buf, seed);

	WriteString("What location shall we attempt? ");
	seed = ReadInt();

	if(seed < (unsigned long)BaseMem || seed > (unsigned long)BaseMem + (0xa00*3))
	{
		WriteString("Invalid location.\n");
		exit(0);
	}

	WriteString("Good luck!\n");
	__asm(	"mov %0, %%rax\n"
		"xor %%rbx, %%rbx\n"
		"xor %%rcx, %%rcx\n"
		"xor %%rdx, %%rdx\n"
		"xor %%rdi, %%rdi\n"
		"xor %%rsi, %%rsi\n"
		"xor %%r8, %%r8\n"
		"xor %%r9, %%r9\n"
		"xor %%r10, %%r10\n"
		"xor %%r11, %%r11\n"
		"xor %%r12, %%r12\n"
		"xor %%r13, %%r13\n"
		"xor %%r14, %%r14\n"
		"xor %%r15, %%r15\n"
		"xor %%rbp, %%rbp\n"
		"xor %%rsp, %%rsp\n"
		"pxor %%mm0, %%mm0\n"
		"pxor %%mm1, %%mm1\n"
		"pxor %%mm2, %%mm2\n"
		"pxor %%mm3, %%mm3\n"
		"pxor %%mm4, %%mm4\n"
		"pxor %%mm5, %%mm5\n"
		"pxor %%mm6, %%mm6\n"
		"pxor %%mm7, %%mm7\n"
		"xorps %%xmm0, %%xmm0\n"
		"xorps %%xmm1, %%xmm1\n"
		"xorps %%xmm2, %%xmm2\n"
		"xorps %%xmm3, %%xmm3\n"
		"xorps %%xmm4, %%xmm4\n"
		"xorps %%xmm5, %%xmm5\n"
		"xorps %%xmm6, %%xmm6\n"
		"xorps %%xmm7, %%xmm7\n"
		"xorps %%xmm8, %%xmm8\n"
		"xorps %%xmm9, %%xmm9\n"
		"xorps %%xmm10, %%xmm10\n"
		"xorps %%xmm11, %%xmm11\n"
		"xorps %%xmm12, %%xmm12\n"
		"xorps %%xmm13, %%xmm13\n"
		"xorps %%xmm14, %%xmm14\n"
		"xorps %%xmm15, %%xmm15\n"

		"mov (%%rax), %%rbx\n"
		"mov (%%rbx), %%rcx\n"
		"mov (%%rcx), %%rdx\n"
		"jmp (%%rdx)\n"
		:
		: "g" (seed)
	);

	return 0;
}
