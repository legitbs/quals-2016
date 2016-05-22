#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);
#define AlarmTime 30

#define WriteStr(msg) write(1, msg, strlen(msg));

char Filename[14 + 6 + 2 + 1];	//prefix, X len, suffix, null byte
char OFilename[sizeof(Filename)];
char LogFilename[sizeof(Filename)];

int ReadUntil(char *Buffer, int BufferSize, char EndChar)
{
	int BufPos = 0;
	int ReadSize;

	//read until the buffer is full or we get EndChar
	for(BufPos = 0; BufPos < BufferSize; BufPos += ReadSize)
	{
		ReadSize = read(0, &Buffer[BufPos], 1);
		if(ReadSize <= 0)
			_exit(0);

		if(Buffer[BufPos] == EndChar)
		{
			Buffer[BufPos] = 0;
			return BufPos;
		}

		//if not a normal ascii character and not tab or newline then die
		if(	((Buffer[BufPos] > 0x7e) || (Buffer[BufPos] < 0x20)) &&
			(Buffer[BufPos] != '\n') &&
			(Buffer[BufPos] != '\t') &&
			(Buffer[BufPos] != '\r'))
		{
			WriteStr("Invalid character sent\n");
			_exit(0);
		}
	}

	return BufPos;
}

void WriteAll(char *Data, int DataLen)
{
	int WriteLen;
	int WritePos;

	for(WritePos = 0; WritePos < DataLen; WritePos += WriteLen)
	{
		WriteLen = write(1, &Data[WritePos], DataLen - WritePos);
		if(WriteLen <= 0)
			_exit(0);
	}
}

static void alarmHandle(int sig)
{
	struct stat FileStat;
	return;

	//delete our files if they exist
	if(stat(Filename, &FileStat) == 0)
		unlink(Filename);

	if(stat(OFilename, &FileStat) == 0)
		unlink(OFilename);

	if(stat(LogFilename, &FileStat) == 0)
		unlink(LogFilename);

	WriteStr("\nTimeout, closing connection\n");
	_exit(0);
}
int main(int argc, char **argv)
{
	char Buffer[1025];
	int ReadData;
	int fd;
	struct stat FileStat;

	memset(Filename, 0, sizeof(Filename));
	memset(OFilename, 0, sizeof(OFilename));
	memset(LogFilename, 0, sizeof(LogFilename));

	//make sure we stop if they try to just hang the connection open
        signal(SIGALRM, (sighandler_t)alarmHandle);
        alarm(AlarmTime);

	memset(Buffer, 0, sizeof(Buffer));

	WriteStr("Flag file is opened on file descriptor 3\n");
	WriteStr("Please provide a C file to compile. Terminate the end of the C file with an ETX character\n");
	WriteStr("Example:\nint main()\n{\n\twrite(1, \"hi\", 2);\n}\n");
	WriteStr("1024 byte maximum text limit\n");

	ReadData = ReadUntil(Buffer, sizeof(Buffer) - 1, 0x03);

	memcpy(Filename, "/tmp/crippled-XXXXXX.c", sizeof(Filename) - 1);
	Filename[sizeof(Filename) - 1] = 0;
	fd = mkstemps(Filename, 2);
	if(fd < 0)
	{
		WriteStr("Error creating temp file\n");
		_exit(0);
	}

	//write the file out
	write(fd, Buffer, ReadData);
	close(fd);

	memcpy(OFilename, Filename, sizeof(Filename));
	OFilename[sizeof(Filename) - 2] = 'o';

	memcpy(LogFilename, Filename, sizeof(Filename));
	LogFilename[sizeof(Filename) - 2] = 'l';

	//reset the alarm just incase
	alarm(AlarmTime);

	//compile the file
	WriteStr("--Compiling--\n");
	if(fork() == 0)
	{
		//open up the log file and dup to it
		fd = creat(LogFilename, 0700);

		dup2(fd, 1);
		dup2(fd, 2);
		close(0);

		char *NCCArgs[] = {"ncc", "-o", OFilename, Filename, 0};
		execve("/home/crippled/compiler/ncc", NCCArgs, 0);
	}
	else
	{
		//wait for the thread to finish
		wait(0);

		//see if the .o file exists
		unlink(Filename);
		if(stat(OFilename, &FileStat))
		{
			//see if we have a log to return
			if(stat(LogFilename, &FileStat))
			{
				WriteStr("Unknown compile error\n");
			}
			else
			{
				fd = open(LogFilename, O_RDONLY);
				ReadData = read(fd, Buffer, 1024);
				WriteAll(Buffer, ReadData);
				if(ReadData == 1024)
				{
					ReadData = read(fd, Buffer, 1);
					if(ReadData)
						WriteStr("\nOut buffer too big\n");
				}

				close(fd);
				unlink(LogFilename);
				_exit(0);
			}
			fflush(stdout);
			_exit(0);
		}
	}

	//.o file exists, link everything
	unlink(LogFilename);

	//link the file
	Filename[sizeof(Filename) - 3] = 0;

	//reset the alarm just incase
	alarm(AlarmTime);

	WriteStr("--Linking--\n");
	if(fork() == 0)
	{
		//open up the log file and dup to it
		fd = creat(LogFilename, 0700);

		dup2(fd, 1);
		dup2(fd, 2);
		close(0);

		char *NCCArgs[] = {"nld", "-p", "-s", "-o", Filename, "/home/crippled/compiler/start.o", OFilename, 0};
		execve("/home/crippled/compiler/nld", NCCArgs, 0);
	}
	else
	{
		//wait for the thread to finish
		wait(0);

		//see if the final file exists
		if(stat(Filename, &FileStat))
		{
			//see if we have a log to return
			if(stat(LogFilename, &FileStat))
			{
				WriteStr("Unknown linker error\n");
			}
			else
			{
				fd = open(LogFilename, O_RDONLY);
				ReadData = read(fd, Buffer, 1024);
				WriteAll(Buffer, ReadData);
				if(ReadData == 1024)
				{
					ReadData = read(fd, Buffer, 1);
					if(ReadData)
						WriteStr("\nOut buffer too big\n");
				}

				close(fd);
				unlink(LogFilename);
				_exit(0);
			}
			fflush(stdout);
			_exit(0);
		}
	}

	char *NullArgs[] = {0};
	unlink(LogFilename);
	unlink(OFilename);

	//make sure we timeout after the app does
	alarm(AlarmTime);

	WriteStr("--Executing, 10 second alarm--\n");
	if(fork() == 0)
	{
		//open up the log file and dup to it
		close(0);
		fd = creat(LogFilename, 0700);

		dup2(fd, 1);
		dup2(fd, 2);

		char *NullArgs[] = {0};
		execve(Filename, NullArgs, 0);
	}
	else
	{
		//wait for the thread to finish
		wait(0);

		//see if we have a log to return
		if(stat(LogFilename, &FileStat))
		{
			WriteStr("No output\n");
		}
		else
		{
			fd = open(LogFilename, O_RDONLY);
			ReadData = read(fd, Buffer, 1024);
			WriteAll(Buffer, ReadData);
			if(ReadData == 1024)
			{
				ReadData = read(fd, Buffer, 1);
				if(ReadData)
					WriteStr("\nOut buffer too big\n");
			}

			close(fd);
			unlink(LogFilename);
		}
	}

	//cleanup
	fflush(stdout);
	unlink(Filename);
	return 0;
}
