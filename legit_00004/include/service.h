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

#ifndef SERVICE_H
#define SERVICE_H



typedef struct langInfo {
	
	char *spanishVerb;
	char *englishMeaning;
	char *yoForm;
	char *tuForm;
	char *ustedForm;
	char *weForm;
	char *theyForm;

	struct langInfo *next;

} langInfoType;

int initDB( langInfoType ** );
int compare_strings(char *, char *);
int authenticateUser(char *, char *);
void printWordMenu( void );
int doWordMaint( langInfoType ** );
int addNewWord( langInfoType ** );
int deleteTmpWord( langInfoType ** );
int match_str(char *search, char *target);
int flashCards( langInfoType *database );
int quizWords( langInfoType *database );

#endif
