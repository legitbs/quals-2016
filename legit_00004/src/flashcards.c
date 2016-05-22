
/*

Author: Deadwood

Copyright (c) 2016 LegitBS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "service.h"
#include "stdlib.h"

int flashCards( langInfoType *database ) {

int count;
langInfoType *tmpPtr;
int choice;
int last_choice;
char buffer[10];
static unsigned short *randchoice = 0;
int i;

	tmpPtr = database;
	count = 0;
	buffer[0] = 0;

	while( tmpPtr != 0 ) {

		++count;
		tmpPtr = tmpPtr->next;

	}

	if (count == 0) 
		return -1;

	if ( randchoice == 0 )
		randchoice = (unsigned short *)0x4347c000;


	printf("\nHit <Enter> to continue, or q to exit\n");

	while (buffer[0] != 'q') {

		// here is a weakness.  If the number of entries in the database is 64K, then the random choice
		// exactly matches the value from the magic page.  Two consecutive choices gives the 4 bytes necessary
		// for a type 2 POV.  The right way to patch this would be to fix the method used to pick a random entry
		// but that would break the poller...
		choice = *randchoice % count;
		randchoice++;	

		if ((unsigned int)randchoice > (0x4347c000 + 4094))
			randchoice = (unsigned short *)0x4347c000;

		tmpPtr = database;

		for ( i=0; i < choice; ++i ) 
			tmpPtr = tmpPtr->next;


		printf("\n@s\n", tmpPtr->spanishVerb);

		getline(buffer, sizeof(buffer));

		printf("@s\n\n", tmpPtr->englishMeaning);

	}

	return 0;
}


