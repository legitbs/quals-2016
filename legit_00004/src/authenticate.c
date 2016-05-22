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

#define NAMELEN 10
#define PASSLEN 10
#define AUTH_DATA_ADDR 0x4347c000
#define AUTH_DATA_LEN 4096

extern char *initialData[][2];

int authenticateUser(char *username, char *password) {
int i, x;
int offset;
unsigned char *authData;



char tmpusername[10];
char tmppassword[10];

	authData = (unsigned char *)AUTH_DATA_ADDR;

	for ( i=0; i < AUTH_DATA_LEN / (NAMELEN + PASSLEN ); ++i ) {

		offset = i * (NAMELEN + PASSLEN);

		for ( x=0; x < NAMELEN; ++x ) {

			tmpusername[x] = (*(authData + offset + x) % 26) + 'a';
			tmppassword[x] = (*(authData + offset + NAMELEN + x ) % 90 ) +32;

		} // for (x=0;;)

		if (compare_strings(username, tmpusername) == 0 ) {


			if (compare_strings(password, tmppassword) == 0 ) 
				return 1;
			else
				return 0;

		}
	} // for (i=0;;)

	for ( i=0; i < 2; ++i ) {

		if (compare_strings(username, initialData[32+i][0]) == 0 ) {


				if (compare_strings(password, initialData[32+i][1]) == 0 ) 
					return 1;
				else
					return 0;

		}

	} // for

	return 0;

}
