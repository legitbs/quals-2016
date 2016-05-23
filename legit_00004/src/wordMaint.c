
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

#include <libcgc.h>
#include "service.h"
#include "stdlib.h"

int deleteWords( langInfoType **database );
int editWords( langInfoType *database );
int listWords( langInfoType *database );

void printWordMenu( void ) {
	

	printf("\n1) Add New Words\n");
	printf("2) Delete Words\n");
	printf("3) Edit Words\n");
    printf("4) List Words\n");
    printf("5) Exit Word Maintenance\n");

}


int doWordMaint( langInfoType **database ) {

int choice;
char buffer[10];

    while (choice != 5 ) {

        printWordMenu();

        if (getline(buffer, sizeof(buffer)) == 0) {

            return -1;

        }    

        choice = atoi(buffer);

        if ( choice == 1 ) {

            addNewWord( database );
        }
        else if ( choice == 2 ) {

            deleteWords( database );

        }
        else if ( choice == 3 ) {

            editWords( *database );

        }
        else if ( choice == 4 ) {

            listWords( *database );
        }
    }

    return 0;
}


int addNewWord( langInfoType **database ) {
	

char buffer[100];
langInfoType *tmpEntry;
langInfoType *tmpPtr;
int quit = 0;
int count = 0;

    tmpPtr = *database;

    // get to the end of the list so we can count the number of entries.
    // and will be at the point to add new entries to the end
    if (tmpPtr != 0 ) {

        while ( tmpPtr->next != 0 ) {
            tmpPtr = tmpPtr->next;
            ++count;
        }
    }

    while (!quit) {

#ifdef PATCHED_1

        // nothing magic about 10K except it is considerably below 64K (2^16)
        // which it needs to be to thwart the info disclosure vulnerability
        if ( count >= 10000 ) {

            printf("Maximum word count reached\n");
            return -1;
        }
#endif

    	tmpEntry = malloc(sizeof(langInfoType));

    	if ( tmpEntry == 0 ) {

    		printf("out of memory\n");
    		return -1;
    	}

    	tmpEntry->next = 0;


    	printf("Enter infinitive form\n");

        if (getline(buffer, sizeof(buffer)) == 0) {

        	deleteTmpWord(&tmpEntry);
        	return -1;

        }

        tmpEntry->spanishVerb = malloc(strlen(buffer) +1 );

        if (tmpEntry->spanishVerb == 0 ) {

        	deleteTmpWord(&tmpEntry);
        	return -1;

        }

        strcpy(tmpEntry->spanishVerb, buffer);


    	printf("Enter English meaning\n");

        if (getline(buffer, sizeof(buffer)) == 0) {

        	deleteTmpWord(&tmpEntry);
        	return -1;

        }

        tmpEntry->englishMeaning = malloc(strlen(buffer) +1 );

        if (tmpEntry->englishMeaning == 0 ) {

        	deleteTmpWord(&tmpEntry);
        	return -1;

        }

        strcpy(tmpEntry->englishMeaning, buffer);


        // now add to the end of our linked list of words
        if (*database == 0 ) {

            *database = tmpEntry;

            tmpPtr = *database;

        }
        else {

            tmpPtr->next = tmpEntry;  

            tmpPtr = tmpEntry;

        }

        count++;
        printf("\n");

    } // while



    return 0;

}


int deleteTmpWord( langInfoType **word) {
	

	if ( word == 0 || *word == 0 ) 
		return -1;

	if ( (*word)->spanishVerb != 0 ) {

		free((*word)->spanishVerb);

	}

	if ( (*word)->englishMeaning != 0 ) {

		free( (*word)->englishMeaning );

	}

	free( *word );

	return 0;

}

int deleteWords( langInfoType **database ) {

int choice;
char buffer[100];
char answer[10];

langInfoType *tmpPtr;
langInfoType *prevPtr;

    printf("Enter word to delete\n");

    if (getline(buffer, sizeof(buffer)) == 0) {

        return -1;
    }

    tmpPtr = *database;
    prevPtr = *database;

    while ( tmpPtr != 0 ) {


        if ( match_str( buffer, tmpPtr->spanishVerb ) ) {
            printf("@s: @s\n", tmpPtr->spanishVerb, tmpPtr->englishMeaning );

            printf("do you wish to delete this word? ");

            if ( getline(answer, sizeof(answer)) > 0 ) {

                if ( answer[0] == 'y' || answer[0] == 'Y' ) {

                    if ( tmpPtr == *database ) {

                        *database = tmpPtr->next;
                        deleteTmpWord(&tmpPtr);

                        tmpPtr = *database;
                        prevPtr = *database;
                        continue;

                    }
                    else {

                        prevPtr->next = tmpPtr->next;
                        deleteTmpWord(&tmpPtr);

                        tmpPtr = prevPtr->next;
                        continue;

                    } //else

                } // if buffer == y

            } // if getline

        } // if match_str


        if ( prevPtr != tmpPtr && prevPtr != 0) 
            prevPtr = prevPtr->next;

        if ( tmpPtr != 0 )
            tmpPtr = tmpPtr->next;

    } // while


    return 0;
}

int editWords( langInfoType *database ) {

    printf("Oops, this isn't finished.  Guess you'd better delete and then re-add the word\n");
    return 0;
    
}

int listWords( langInfoType *database ) {

int count;

    count = 0;
    while ( database != 0 ) {


        printf( "@d-@s: @s\n", count, database->spanishVerb, database->englishMeaning );

        database = database->next;
        count++;

    }

    return 0;
}

