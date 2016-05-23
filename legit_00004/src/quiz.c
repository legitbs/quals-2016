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

int quizWords( langInfoType *database ) {

int count;
langInfoType *tmpPtr;
int choice;
int wrongAnswer;
char buffer[10];
static unsigned short *randChoice = 0;
unsigned char rightAnswerNum;
langInfoType *selectedWord;

char choiceLetters[4] = { 'a', 'b', 'c', 'd' };
int i, x;

	tmpPtr = database;
	count = 0;
	buffer[0] = 0;

	while( tmpPtr != 0 ) {

		++count;
		tmpPtr = tmpPtr->next;

	}

	if (count == 0) 
		return -1;


	printf("Answer with 'q' to exit the quiz\n\n");

	if ( randChoice == 0 )
		randChoice = (unsigned short *)0x4347c000;


	while (buffer[0] != 'q') {


		choice = *randChoice % count;
		randChoice++;

		if ((unsigned int)randChoice > (0x4347c000 + 4094))
			randChoice = (unsigned short *)0x4347c000;

		rightAnswerNum = *randChoice % 4;
		++randChoice;

		if ((unsigned int)randChoice > (0x4347c000 + 4094))
			randChoice = (unsigned short *)0x4347c000;

		selectedWord = database;

		for ( i=0; i < choice; ++i ) 
			selectedWord = selectedWord->next;


		printf("\n@s\n", selectedWord->spanishVerb);

		for ( x=0; x < 4; ++x ) {

			printf("  @c - ", choiceLetters[x]);

			if ( x == rightAnswerNum ) {

				printf("@s\n", selectedWord->englishMeaning);
			}
			else {

				wrongAnswer = *randChoice % count;
				++randChoice;

				if ((unsigned int)randChoice > (0x4347c000 + 4094))
					randChoice = (unsigned short *)0x4347c000;

				tmpPtr = database;

				for ( i=0; i < wrongAnswer; ++i ) 
					tmpPtr = tmpPtr->next;

				if ( tmpPtr == selectedWord ) {

					if ( wrongAnswer < count-1 )
						tmpPtr = tmpPtr->next;
					else
						tmpPtr = database;
				}

				printf("@s\n", tmpPtr->englishMeaning);

			}
		}

		getline(buffer, sizeof(buffer));

		if (buffer[0] == choiceLetters[rightAnswerNum] )
			printf("\nCorrect!\n");
		else if (buffer[0] == 'q' )
			break;
		else
			printf("\nWrong!\n");

	}

	return 0;
}


