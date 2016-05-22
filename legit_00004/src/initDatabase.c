
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

char *initialData[][2] = {

    { "visitar", "to visit" },
    { "ver", "to see" },
    { "hablar", "to speak" },
    { "tener", "to have" },
    { "entender", "to understand" },
    { "ser", "to be" },
    { "estar", "to be" },
    { "ir" , "to go" },
    { "haber", "to have" },
    { "hacer", "to make" },
    { "querer", "to want" },
    { "deber", "to have to" },
    { "poder", "to be able to" },
    { "decir", "to say" },
    { "mirar", "to watch" },
    { "levantarse", "to get up" },
    { "probar", "to taste" },
    { "saber", "to know" }, 
    { "salir", "to leave" },
    { "subir", "to go up or climb" },
    { "venir", "to come" },
    { "caerse", "to fall down" },
    { "conocer", "to know people and places" },
    { "ducharse", "to shower" },
    { "descansar", "to rest" },
    { "llover", "to rain" },
    { "caminar", "to walk" },
    { "cenar", "to have supper" },
    { "comer", "to eat" },
    { "desear", "to desire or want" },
    { "dormir", "to sleep" },
    { "entrar", "to enter" },
    { "huesped", "madera" },
    { "invitado", "muerte" }

};

int initDB( langInfoType **database ) {
	
langInfoType *tmpPtr;

int i;
int randInitValue;


    // do this so its not a fixed number of entries in the database to start
    randInitValue = ((*(unsigned short *)0x4347c000) % 5);

	for (i=0; i < 32-randInitValue; ++i ) {


		if (*database == 0 ) {

			*database = malloc( sizeof(langInfoType) );

			if (*database == 0) 
				return -1;

			tmpPtr = *database;

		}
		else {

			tmpPtr->next = malloc( sizeof(langInfoType) );

			if (tmpPtr->next == 0 ) 
				return -1;

			tmpPtr = tmpPtr->next;
		}

		tmpPtr->spanishVerb = malloc( strlen( initialData[i][0] ) +1 );
		tmpPtr->englishMeaning = malloc( strlen( initialData[i][1] ) +1 );

		strcpy(tmpPtr->spanishVerb, initialData[i][0] );
		strcpy(tmpPtr->englishMeaning, initialData[i][1] );

		tmpPtr->next = 0;

	}

	return 0;

}
