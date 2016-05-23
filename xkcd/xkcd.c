//With apologies to Randall Monroe

#include <stdio.h>
#include <stdlib.h>
struct _globals {
	char userEchoRequest[512];
	char flagBuf[256];
} globals;

int main(int argc, char ** argv)
{
	char *line;
	FILE *theFlag;
	char *tmp;
	int howmany;
	int linelen;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	bzero(globals.flagBuf, 256);
	theFlag = fopen("/flag", "r");
	
	if(theFlag == NULL) {
		puts("Could not open the flag.");
		return -1;
	}

	fread(globals.flagBuf, 1, 256, theFlag);
	while(1) {
		line = fgetln(stdin, &linelen);

		tmp = strtok(line, "?");
		if(strcmp(tmp, "SERVER, ARE YOU STILL THERE") != 0) {
			puts("MALFORMED REQUEST");
			exit(-1);
		}

		tmp = strtok(NULL, "\"");
		if(strcmp(tmp, " IF SO, REPLY ") != 0) {
			puts("MALFORMED REQUEST");
			exit(-1);
		}

		tmp = strtok(NULL, "\"");
		memcpy(globals.userEchoRequest, tmp, strlen(tmp));

		tmp = strtok(NULL, "(");
		tmp = strtok(NULL, ")");
		sscanf(tmp, "%d LETTERS", &howmany);

		globals.userEchoRequest[howmany] = 0;
		if(howmany > strlen(globals.userEchoRequest)) {
			puts("NICE TRY");
			exit(-1);
		}

		printf("%s\n", globals.userEchoRequest);
	}
}
