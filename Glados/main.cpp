#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include "malloc.h"
#include "classes.h"
#include "main.h"
#include "init.h"

void *__dso_handle;
CoreStruct *CoreHead;
GladosCore *MainCore;

char *CoreNameList[] = {
	"",
	"Glados",
	"Cave Johnson",
	"Data Array",
	"Memory Info",
	"Random Number",
	"Buffer Overflow",
	"Raw Data",
	0
};

typedef void (*sighandler_t)(int);
#define AlarmTime 30

void alarmHandle(int sig)
{
	_exit(0);
}

CoreStruct *CreateCoreEntry(int CoreType)
{
	BaseCore::CoreEnum TypeOfCore = (BaseCore::CoreEnum)CoreType;

	CoreStruct *NewEntry;
	NewEntry = (CoreStruct *)malloc(sizeof(CoreStruct));
	memset(NewEntry, 0, sizeof(CoreStruct));

	if(TypeOfCore == BaseCore::CoreEnum::CaveJohnson)
		NewEntry->Core = new CaveJohnsonCore();
	else if(TypeOfCore == BaseCore::CoreEnum::Glados)
		NewEntry->Core = new GladosCore();
	else if(TypeOfCore == BaseCore::CoreEnum::DataArray)
		NewEntry->Core = new DataArrayCore();
	else if(TypeOfCore == BaseCore::CoreEnum::RandomNumber)
		NewEntry->Core = new RandomNumberCore();
	else if(TypeOfCore == BaseCore::CoreEnum::BufferOverflow)
		NewEntry->Core = new BufferOverflowCore();
	else if(TypeOfCore == BaseCore::CoreEnum::MemoryInfo)
		NewEntry->Core = new MemoryInfoCore();
	else if(TypeOfCore == BaseCore::CoreEnum::RawData)
		NewEntry->Core = new RawDataCore();
	else
		NewEntry->Core = new BaseCore();	//return base if invalid entry, avoid bad pointer as a result

	//printf("core addr: %016lx\n", (unsigned long)NewEntry->Core);
	return NewEntry;
}

void AddCoreEntry(CoreStruct *Entry)
{
	CoreStruct *Temp;

	//add entry to the core list
	Entry->Next = 0;
	if(!CoreHead)
	{
		CoreHead = Entry;
		CoreHead->Prev = 0;
	}
	else
	{
		Temp = CoreHead;
		while(Temp->Next)
			Temp = Temp->Next;

		Temp->Next = Entry;
		Entry->Prev = Temp;
	}
}

void DeleteCoreEntry(CoreStruct *Entry)
{
	//if this core matches the main core then indicate that we need to find a new one
	if(MainCore == Entry->Core)
		MainCore = 0;

	//delete a core entry
	if(CoreHead == Entry)
	{
		CoreHead = Entry->Next;
		if(CoreHead)
			CoreHead->Prev = 0;
	}
	else
	{
		//remove it from the tree
		if(Entry->Prev)
			Entry->Prev->Next = Entry->Next;
		if(Entry->Next)
			Entry->Next->Prev = Entry->Prev;
	}

	delete(Entry->Core);
	free(Entry);

	//If no main core then scan through the cores looking for another glados
	//or cave johnson core. if none found then allocate glados
	if(!MainCore)
	{
		for(Entry = CoreHead; Entry; Entry = Entry->Next)
		{
			if(	(Entry->Core->CoreType() == BaseCore::CoreEnum::Glados) ||
				(Entry->Core->CoreType() == BaseCore::CoreEnum::CaveJohnson))
			{
				MainCore = (GladosCore *)Entry->Core;
				break;
			}
		}
	}

	//if none found then setup a new one
	if(!MainCore)
	{
		WriteString("No main core found!", 1);
		Entry = CreateCoreEntry(BaseCore::CoreEnum::Glados);
		MainCore = (GladosCore *)Entry->Core;
		AddCoreEntry(Entry);
	}
}

void PrintMenu()
{
	WriteString(	"\n0. Exit\n"
			"----------------------\n"
			"1. Add Core\n"
			"2. Get Core Info\n"
			"3. List Cores\n"
			"4. Remove Core\n"
			"5. Interact with Core\n\n"
			"Selection: "
			, 0);
}

void ListCores(unsigned int AddBack)
{
	CoreStruct *Temp;
	int ID;

	WriteString("Core List:", 1);

	if(AddBack)
		WriteString("0: Previous\n-----------", 1);

	Temp = CoreHead;
	for(ID = 1; Temp; ID++)
	{
		printf("%d: %s Core\n", ID, Temp->Core->MenuString());
		Temp = Temp->Next;
	}

	WriteString("", 1);
}

CoreStruct *SelectCore(char *Msg)
{
	CoreStruct *Temp;
	int CoreID;

	//find out what core number to select
	WriteString(Msg, 1);
	ListCores(1);
	WriteString("Core Number: ", 0);
	CoreID = ReadInt();
	if(CoreID == 0)	//previous menu
		return 0;

	//off by 1
	CoreID--;
	for(Temp = CoreHead; Temp && CoreID; CoreID--)
		Temp = Temp->Next;

	//if no core error about it
	if(!Temp)
	{
		WriteString("Unable to find core", 1);
		return 0;
	}

	return Temp;
}

void AddCore()
{
	unsigned int i;

	WriteString("What type of core do you want to add?", 1);
	for(i = 1; CoreNameList[i]; i++)
		printf("%d: %s Core\n", i, CoreNameList[i]);

	WriteString("Selection: ", 0);
	i = ReadInt();

	if(i == 0 || i >= BaseCore::CoreEnum::CORE_COUNT)
	{
		WriteString("Invalid selection", 1);
		return;
	}

	//create a core based on the requested ID
	AddCoreEntry(CreateCoreEntry((BaseCore::CoreEnum)i));
}

void RemoveCore()
{
	CoreStruct *Temp;

	Temp = SelectCore("Which core number do you want to remove?");

	//if Temp is invalid then just go back
	if(!Temp)
		return;

	//delete the core
	DeleteCoreEntry(Temp);
}

void InteractWithCore()
{
	CoreStruct *Temp;

	//find a core they want to interact with and call the action for it
	Temp = SelectCore("Which core number do you want to interact with?");

	//if Temp is invalid then just go back
	if(!Temp)
		return;

	Temp->Core->Action(0);
}

void GetCoreInfo()
{
	unsigned int ID;
	CoreStruct *Temp;
	CoreStruct *Temp2;

	//find a core they want to interact with and call the action for it
	Temp = SelectCore("Which core number do you want to information on?");

	//if Temp is invalid then just go back
	if(!Temp)
		return;

	//find the ID of the core
	for(Temp2 = Temp, ID = 0; Temp2; Temp2 = Temp2->Prev, ID++)
	{}
	
	printf("Core Name: %s\n", Temp->Core->MenuString());
	printf("Core ID: %d\n", ID);
	printf("Core Description:\n%s\n", Temp->Core->Description());
	printf("Amount of memory used: %d\n\n", Temp->Core->TotalMemSize());
}

void HandleMenu(int Entry)
{
	switch(Entry)
	{
		case 0:
			_exit(0);

		case 1:
			AddCore();
			break;

		case 2:
			GetCoreInfo();
			break;

		case 3:
			ListCores(0);
			break;

		case 4:
			RemoveCore();
			break;

		case 5:
			InteractWithCore();
			break;

		default:
			break;
	};
}

#define TEXTSTART 0x400000
#define TEXTEND	  0x435000
#define DATASTART 0x634000
#define DATAEND   0x638000

#define PROGSIZE (DATAEND-TEXTSTART)
#define EXECSIZE (TEXTEND-TEXTSTART)
#define EMPTYSIZE (DATASTART-TEXTEND)
#define DATASIZE (DATAEND-DATASTART)

int main(int argc, char **argv)
{
	CoreStruct *TempCoreStruct;
	int fd;
	unsigned int seed;
	void *NewAlloc;
	void *NewAllocRet;

	//move the binary
	fd = open("/dev/urandom", O_RDONLY);
	do
	{
		read(fd, &NewAlloc, sizeof(NewAlloc));
		NewAlloc = (void *)((unsigned long)NewAlloc & 0x00007ffffffff000);

		//go allocate an area big enough
#ifdef _GLADOS_DEBUG
		NewAlloc = (void *)0x4000000;
#endif
		NewAllocRet = mmap(NewAlloc, PROGSIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}
	while(NewAlloc != NewAllocRet);
	close(fd);

	//we have our allocation, move ourselves
	memcpy(NewAlloc, (unsigned int *)TEXTSTART, EXECSIZE);
	memcpy((void *)((unsigned long)NewAlloc + EXECSIZE + EMPTYSIZE), (unsigned int *)DATASTART, DATASIZE);

	//now mark the new as read/exec only for code
	mprotect(NewAlloc, EXECSIZE, PROT_READ | PROT_EXEC
#ifdef _GLADOS_DEBUG
					| PROT_WRITE
#endif
);
	munmap((void *)((unsigned long)NewAlloc + EXECSIZE), EMPTYSIZE);

	//now jump to our area after doing a few fixups
	__asm(
		"jmp jump_finish\n"
		"jump_prep:\n"
		"pop %%rax\n"
		"sub %1, %%rax\n"
		"add %0, %%rax\n"
		"jmp %%rax\n"
		"jump_finish:\n"
		"call jump_prep\n"

		//now in new code
		"mov %%rsp, %%rdx\n"		//fix stack
		"shr $12, %%rdx\n"
		"inc %%rdx\n"
		"shl $12, %%rdx\n"

		//find all old entries on the stack then fix them
		"mov %%rsp, %%rcx\n"
		"fix_on_stack:\n"
		"mov (%%rcx), %%rax\n"
		"sub %1, %%rax\n"
		"cmp %3, %%rax\n"
		"ja next_on_stack\n"
		"add %0, %%rax\n"
		"mov %%rax, (%%rcx)\n"
		"next_on_stack:\n"
		"add $8, %%rcx\n"
		"cmp %%rdx, %%rcx\n"
		"jb fix_on_stack\n"

		//fix all data pointers
		"mov %5, %%rcx\n"
		"add %0, %%rcx\n"
		"mov %3, %%rdx\n"
		"add %0, %%rdx\n"
		"fix_data_segment:\n"
		"mov (%%rcx), %%rax\n"
		"sub %1, %%rax\n"
		"cmp %3, %%rax\n"
		"ja next_in_data_segment\n"
		"add %0, %%rax\n"
		"mov %%rax, (%%rcx)\n"
		"next_in_data_segment:\n"
		"add $8, %%rcx\n"
		"cmp %%rdx, %%rcx\n"
		"jb fix_data_segment\n"
		//"int $3\n"
	:
	: "g" (NewAlloc), "i" (TEXTSTART), "i" (EXECSIZE), "i" (PROGSIZE), "i" (DATASTART), "i" (DATASTART-TEXTSTART)
	: "rax","rdx","rcx"
	);

	//now remove our old copy
	munmap((unsigned int *)TEXTSTART, PROGSIZE);

	signal(SIGALRM, (sighandler_t)alarmHandle);

	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &seed, sizeof(seed));
	close(fd);

	srandom(seed);
	CoreHead = 0;
	TempCoreStruct = CreateCoreEntry(BaseCore::CoreEnum::Glados);
	MainCore = (GladosCore *)TempCoreStruct->Core;
	AddCoreEntry(TempCoreStruct);

	while(1)
	{
	        alarm(AlarmTime);
		MainCore->Action(0);
		PrintMenu();
		HandleMenu(ReadInt());
	}
}
