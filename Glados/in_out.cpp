#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

void WriteString(char *s, int WithNewline)
{
	write(1, s, strlen(s));
	if(WithNewline)
		write(1, "\n", 1);
}

int ReadLine(char *Buffer, int Size)
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

int ReadAll(char *Buffer, int Size)
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

unsigned long ReadInt()
{
	char Buffer[30];
	ReadLine(Buffer, sizeof(Buffer) - 1);
	Buffer[sizeof(Buffer) - 1] = 0;
	if(strlen(Buffer) == 0)
		return -1;
	else
		return strtoul(Buffer, 0, 10);
}
