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
#include "stdlib.h"
#include "service.h"


int main(void) {
    
int authenticated = 0;
char buffer[1024];
char password[1024];
int choice;
langInfoType *database;
int quit = 0;


    database = 0;

   initDB(&database);

    // loop forever receiving command strings from STDIN.

    printf("Bienvenidos to the Spanish Tutor!\n\n");

    while (1) {

        if ( authenticated ) { 

            // print the game menu

            printf("\n1) Do verb flashcards\n");
            printf("2) Word quizzes\n");
            printf("3) Perform word maintenance\n");
            printf("4) Quit\n");
            printf(": ");
        }
        else {
            printf("login: ");
        }

        // returns the size of the input line.  if its null, just go back and get another
        if (getline(buffer, sizeof(buffer)) == 0)
            continue;


        if ( !authenticated ) {

            printf("password: ");

            getline(password, sizeof(password));


            authenticated = authenticateUser(buffer, password);

            if ( !authenticated ) 
                printf("\ninvalid login\n");
            else
                printf("\naccess granted\n");

            continue;

        }

        choice = atoi(buffer);

        switch (choice) {


            case 1:

                // play flashcards
                flashCards( database );
                break;

            case 2:

                // take a quiz
                quizWords( database );
                break;

            case 3:

                // update the database of words
                doWordMaint( &database );
                break;

            case 4:

                printf("Adios!\n");
                _terminate(0);
                break;

        }
      
    }  // while(1)

 
}  // main  

