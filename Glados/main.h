#ifndef GLADOS_MAIN
#define GLADOS_MAIN

//#define _GLADOS_DEBUG

typedef struct CoreStruct
{
	class BaseCore *Core;
	CoreStruct *Prev;
	CoreStruct *Next;
} CoreStruct;

extern CoreStruct *CoreHead;

extern void WriteString(char *s, int WithNewline);
int ReadLine(char *Buffer, int Size);
int ReadAll(char *Buffer, int Size);
unsigned long ReadInt();
void AddCoreEntry(CoreStruct *Entry);
CoreStruct *CreateCoreEntry(int CoreType);
void DeleteCoreEntry(CoreStruct *Entry);

#endif
