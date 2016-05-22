#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "classes.h"
#include "malloc.h"
#include "string.h"
#include "glados.h"
#include "cavejohnson.h"
#include "main.h"

#define countof(a) (sizeof(a) / sizeof(*a))
DataArrayCore::DataArrayCore()
{
	_DataArraySize = 0;
}

DataArrayCore::DataArrayCore(int ArraySize)
{
	_DataArraySize = ArraySize;
	_DataArray = (unsigned long *)malloc(_DataArraySize * sizeof(unsigned long));
}

DataArrayCore::DataArrayCore(int ArraySize, char **List) : DataArrayCore(ArraySize)
{
	memcpy(_DataArray, List, _DataArraySize * sizeof(unsigned long));
}

DataArrayCore::~DataArrayCore()
{
	if(_DataArraySize)
		free(_DataArray);
}

void DataArrayCore::InitArray(int FromCaveJohnson)
{
	unsigned long NewSize;
	unsigned long *Temp;

	//if no size then get a new size
	if(!FromCaveJohnson)
	{
		WriteString("What new size do you want? ", 0);
		NewSize = ReadInt();
	}
	else
		NewSize = (random() % 1000) + 1;

	//if too big then fail
	if(NewSize > 1000)
	{
		WriteString("New size too large", 1);
		return;
	}
	else if(NewSize == 0)
	{
		if(_DataArraySize)
			free(_DataArray);
		_DataArraySize = 0;
		_DataArray = 0;
		return;
	}

	//create the new buffer
	Temp = (unsigned long *)malloc(NewSize * sizeof(unsigned long));
	//printf("addr: %016lx to %016lx\n", (unsigned long)Temp, (unsigned long)Temp + (NewSize * sizeof(unsigned long)));
	if(_DataArraySize)
	{
		//we have data, copy it over
		if(_DataArraySize > NewSize)
			_DataArraySize = NewSize;

		memcpy(Temp, _DataArray, _DataArraySize * sizeof(unsigned long));
		memset(&Temp[_DataArraySize], 0, (NewSize - _DataArraySize) * sizeof(unsigned long));
		free(_DataArray);
		_DataArray = Temp;
	}
	else
	{
		_DataArray = Temp;
		memset(_DataArray, 0, NewSize * sizeof(unsigned long));
	}
	_DataArraySize = NewSize;

	if(!FromCaveJohnson)
		WriteString("New size created\n", 1);
}

int DataArrayCore::Action(int FromCaveJohnson)
{
	unsigned long i;
	unsigned long Val;

	if(!FromCaveJohnson)
	{
		if(!_DataArraySize)
			InitArray(0);
		else
		{
			//find out what they want to do
			WriteString("0. Previous\n-----------\n1. Create New Array\n2. Read Array Entry\n3. Write Array Entry\nSelection: ", 0);
			i = ReadInt();

			if(i <= 0)
				return 0;

			//handle the request
			switch(i)
			{
				case 1:
					InitArray(0);
					break;

				case 2:
					WriteString("Which Array Entry: ", 0);
					i = ReadInt();
					i = GetEntry(i);
					printf("Value: %lu\n", i);
					break;

				case 3:
					WriteString("Which Array Entry: ", 0);
					i = ReadInt();
					WriteString("New Value: ", 0);
					Val = ReadInt();
					SetEntry(i, Val);
					break;

				default:
					WriteString("Invalid selection", 1);
			}
		}
	}
	else
	{
		//random Cave Johnson selection
		i = random() % 2;
		if(!i || !_DataArraySize)
			InitArray(1);
		else
		{
			//write a random entry
			i = random() % _DataArraySize;
			_DataArray[i] = random();
		}
	}
	return 0;
}

int DataArrayCore::TotalMemSize()
{
	int Ret = sizeof(DataArrayCore);

	if(_DataArraySize)
		Ret += sizeof(unsigned long) * _DataArraySize;

	return Ret;
}

void DataArrayCore::SetEntry(unsigned long Index, unsigned long Data)
{
	//assign an entry in the array
	if(!_DataArraySize || (Index >= _DataArraySize))
		return;

	_DataArray[Index] = Data;
}

unsigned long DataArrayCore::GetEntry(long Index)
{
	//** BUG **, no check for negative, memory leak
	if(!_DataArraySize || (Index >= (long)_DataArraySize))
		return 0;

	//return the entry
	return _DataArray[Index];
}

GladosCore::GladosCore()
{
	_GoodMsgs = new DataArrayCore(countof(GladosPassQuotes), GladosPassQuotes);
	_BadMsgs = new DataArrayCore(countof(GladosFailQuotes), GladosFailQuotes);
}

GladosCore::~GladosCore()
{
	delete(_GoodMsgs);
	delete(_BadMsgs);
}

int GladosCore::Action(int FromCaveJohnson)
{
	int Rand;
	DataArrayCore *Msg;
	CoreStruct *Temp;

	Rand = random();

	//go see if a Cave Johnson core exists, if so then a higher chance of Glados flipping out
	if(!FromCaveJohnson)
	{
		for(Temp = CoreHead; Temp;)
		{
			if(Temp->Core->CoreType() == BaseCore::CoreEnum::CaveJohnson)
			{
				FromCaveJohnson = 1;
				break;
			}
			Temp = Temp->Next;
		}
	}

	//1 in 100 chance of Glados just disconnecting the person
	//but 1 in 25 if it is Cave Johnsons
	if(((Rand % 100) == 0) || (FromCaveJohnson && ((Rand % 25) == 0)))
	{
		WriteString((char *)_BadMsgs->GetEntry(random() % _BadMsgs->GetCount()), 1);
		_exit(0);
	}
	else
	{
		//1 in 5 chance of doing a bad message
		if(Rand % 5)
			Msg = _GoodMsgs;
		else
			Msg = _BadMsgs;

		WriteString((char *)Msg->GetEntry(Rand % Msg->GetCount()), 1);
	}
	return 0;
}

int GladosCore::TotalMemSize()
{
	return sizeof(GladosCore) + _GoodMsgs->TotalMemSize() + _BadMsgs->TotalMemSize();
}

CaveJohnsonCore::CaveJohnsonCore()
{
	_GoodMsgs = new DataArrayCore(countof(CavePassQuotes), CavePassQuotes);
	_BadMsgs = new DataArrayCore(countof(CaveFailQuotes), CaveFailQuotes);
}
int CaveJohnsonCore::Action(int FromCaveJohnson)
{
	int Count;
	CoreStruct *Temp;
	int Rand;
	DataArrayCore *Msg;

	//1 in 10 chance of cave johnson doing something strange
	Rand = random();
	if((Rand % 10) == 9)
	{
		//determine if we should add a module or test a module
		Rand = random();
		if(Rand % 3 == 0)
		{
			//show first good message 'for science'
			if(!FromCaveJohnson)
				WriteString(CavePassQuotes[0], 1);

			//add a random core
			AddCoreEntry(CreateCoreEntry((BaseCore::CoreEnum)(random() % BaseCore::CoreEnum::CORE_COUNT)));
		}
		else if(Rand % 3 == 1)
		{
			//show first a message 'for science'
			if(!FromCaveJohnson)
			{
				if(random() % 2)
					WriteString(CaveFailQuotes[0], 1);
				else
					WriteString(CavePassQuotes[0], 1);
			}

			//get a count of modules
			for(Temp = CoreHead, Count = 0; Temp; Count++)
				Temp = Temp->Next;

			//pick a random module
			Count = random() % Count;
			Temp = CoreHead;
			for(;Count && Temp; Count--)
				Temp = Temp->Next;

			DeleteCoreEntry(Temp);
		}
		else
		{
			//show first bad message 'for science'
			if(!FromCaveJohnson)
				WriteString(CaveFailQuotes[0], 1);

			//get a count of modules
			Count = 0;
			for(Temp = CoreHead, Count = 0; Temp; Count++)
				Temp = Temp->Next;

			//pick a random module
			Count = random() % Count;
			for(Temp = CoreHead; Count && Temp; Count--)
				Temp = Temp->Next;

			//if we match ourselves then advance one spot before or after
			//in theory you could have 2 cave johnson cores and get one to call the other
			//who random deletes the first one but we return after this so it is a mute point
			if(Temp->Core == this)
			{
				if(Temp->Next)
					Temp = Temp->Next;
				else
					Temp = Temp->Prev;
			}

			//if we have somethin then call it's action indicating it is Cave Johnson
			//this avoids any menu's and auto-fills info when required
			if(Temp)
				Temp->Core->Action(1);
		}
	}
	else if(!FromCaveJohnson)
	{
		//1 in 10 chance of doing a bad message
		if(Rand % 10)
			Msg = _GoodMsgs;
		else
			Msg = _BadMsgs;

		WriteString((char *)Msg->GetEntry(random() % Msg->GetCount()), 1);
	}
	return 0;
}

RandomNumberCore::RandomNumberCore()
{
	_RandomNumberList = 0;
	RefillList();
}

RandomNumberCore::~RandomNumberCore()
{
	//refill list fills this in so this is safe unless someone somehow fills all memory
	delete(_RandomNumberList);
}

int RandomNumberCore::Action(int FromCaveJohnson)
{
	//if cave johnson requested then don't display anything
	if(!FromCaveJohnson)
		printf("Random number: %lu\n", _RandomNumberList->GetEntry(_CurEntry));
	_CurEntry++;
	if(_CurEntry >= _RandomNumberList->GetCount())
		RefillList();

	return 0;
}

int RandomNumberCore::TotalMemSize()
{
	int Ret = sizeof(RandomNumberCore);

	if(_RandomNumberList)
		Ret += _RandomNumberList->TotalMemSize();

	return Ret;
}

void RandomNumberCore::RefillList()
{
	int i;
	char **Buffer;

	//populate our buffer with a new list of entries
	Buffer = (char **)malloc(1000 * sizeof(unsigned long));
	i = open("/dev/urandom", O_RDONLY);
	read(i, Buffer, 1000 * sizeof(unsigned long));
	close(i);

	//fill the list
	if(_RandomNumberList)
		delete(_RandomNumberList);
	_RandomNumberList = new DataArrayCore(1000, Buffer);
	_CurEntry = 0;
}

BufferOverflowCore::BufferOverflowCore()
{
}

BufferOverflowCore::~BufferOverflowCore()
{
}

int BufferOverflowCore::Action(int FromCaveJohnson)
{
	unsigned int i;

	//we really don't want people solving this route
	//if i wanted to be more mean i could just init the cookie "once" and have some location that
	//keeps changing it that wouldn't be noticed right away but that is too much effort
	struct BufOverflowStruct
	{
		char OrigCookie[32];
		char Buffer[1024];
		char CurCookie[32];
	} BufOverflow;

	//get a random cookie
	i = open("/dev/urandom", O_RDONLY);
	read(i, BufOverflow.OrigCookie, sizeof(BufOverflow.OrigCookie));
	close(i);

	//copy said cookie to the current cookie
	memcpy(BufOverflow.CurCookie, BufOverflow.OrigCookie, sizeof(BufOverflow.CurCookie));

	//make sure buffer is empty
	memset(BufOverflow.Buffer, 0, sizeof(BufOverflow.Buffer));

	//find out how much data they want to send us
	if(!FromCaveJohnson)
	{
		WriteString("How many bytes will be sent? ", 0);
		i = ReadInt();
	}
	else
		i = (random() % 4096) + 1;	//pick random size

	//if too many bytes then fail
	if(i > 4096)
	{
		WriteString("Too many bytes requested", 1);
		return 0;
	}

	if(i == 0)
		return 0;

	//go read all of the data sent
	if(!FromCaveJohnson)
		ReadAll(BufOverflow.Buffer, i);
	else
	{
		//fill the buffer up with a pattern
		for(;i > 0; i--)
			BufOverflow.Buffer[i-1] = (i - 1) % 256;
	}

	//now, compare our cookies
	if(memcmp(BufOverflow.OrigCookie, BufOverflow.CurCookie, sizeof(BufOverflow.CurCookie)) != 0)
	{
		WriteString("Successful buffer overflow", 1);
		_exit(0);
	}

	WriteString("Buffer overflow failed", 1);
	return 0;
}

RawDataCore::RawDataCore()
{
	_BufferSize = 0;
}

RawDataCore::~RawDataCore()
{
	//if a buffer exists, free it. **BUG** - I only set buffer size on init and should check it instead
	//this can allow for freeing a controlled pointer
	//it is expected that a fake memory block is setup to allow a write what where scenario
	if(_Buffer)
		free(_Buffer);
}

int RawDataCore::TotalMemSize()
{
	return sizeof(RawDataCore) + _BufferSize;
}

void RawDataCore::InitBuffer(int FromCaveJohnson)
{
	long i;

	if(!FromCaveJohnson)
	{
		WriteString("How big of a buffer shall be allocated? ", 0);
		i = ReadInt();
	}
	else
		i = (random() % 10240)+ 1;

	if(i <= 0)
		return;

	//make sure it isn't too large
	if(i > 10240)
	{
		WriteString("Requested buffer size too large", 1);
		return;
	}

	//allocate it
	if(_BufferSize)
		free(_Buffer);
	_Buffer = (char *)malloc(i);
	memset(_Buffer, 0, i);
	_BufferSize = i;
	return;
}

int RawDataCore::Action(int FromCaveJohnson)
{
	int i;

	if(!FromCaveJohnson)
	{
		//if no buffer then ask about creating one
		if(!_BufferSize)
			InitBuffer(0);
		else	
		{
			//find out what they want to do
			WriteString("0. Previous\n1. Create New Buffer\n2. Write Buffer\n3. Read Buffer\nSelection: ", 1);
			i = ReadInt();

			if(i <= 0)
				return 0;

			//handle the request
			switch(i)
			{
				case 1:
					InitBuffer(0);
					break;

				case 2:
					printf("Receiving %d bytes\n", _BufferSize);
					ReadAll(_Buffer, _BufferSize);
					break;

				case 3:
					printf("Sending %d bytes\n", _BufferSize);
					write(1, _Buffer, _BufferSize);
					break;

				default:
					WriteString("Invalid selection", 1);
			}
		}
	}
	else
	{
		//random Cave Johnson selection
		i = random() % 2;
		if(!i || !_BufferSize)
			InitBuffer(1);
		else
		{
			//fill the buffer with a pattern
			for(i = 0; i < _BufferSize; i++)
				_Buffer[i] = i % 256;
		}
	}

	return 0;
}

MemoryInfoCore::MemoryInfoCore()
{
}

MemoryInfoCore::~MemoryInfoCore()
{
}

int MemoryInfoCore::Action(int FromCaveJohnson)
{
	int ModuleCount = 0;
	int MemoryFootprint = 0;
	int TotalMemory;
	CoreStruct *CurCore;

	//if Cave Johnson then ignore
	if(FromCaveJohnson)
		return 0;

	WriteString("", 1);

	//count how many modules are loaded along with a rough memory footprint of them
	CurCore = CoreHead;
	while(CurCore)
	{
		ModuleCount++;

		TotalMemory = CurCore->Core->TotalMemSize();
		MemoryFootprint += TotalMemory;

		//print the data
		printf("Core %d - %s Core - %d bytes of memory used\n", ModuleCount, CurCore->Core->MenuString(), TotalMemory);

		CurCore = CurCore->Next;
	};

	printf("%d total cores\n%d total bytes of memory used\n\n", ModuleCount, MemoryFootprint);
	return 0;
}

