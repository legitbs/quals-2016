#include <libcgc.h>
#include "libc.h"

int itoa( char *out, int max_length, int value)
{
    int sign = 1;
    int index = 0;
    int t = 0;
    int s = 0;
    int sl = 0;

    if ( value < 0 ) {
        sign = -1;
    }

    value *= sign;

    t = value;

    while ( t ) {
        t /= 10;
        index++;
    }

    if ( value == 0 ) {
        index = 1;
    }

    if ( sign == -1 ) {
        index++;
    }

    if ( max_length < index) {
        return index;
    }

    t = value;

    sl = index;
    while ( index ) {
        s = t % 10;

        out[index-1] = s + 0x30;

        index--;
        t /= 10;
    }

    if ( sign == -1 ) {
        out[0] = '-';
    }

    return sl;
}

unsigned int get_length( )
{
    unsigned int length;

    if ( recvdata( 0, (char*)&length, sizeof(unsigned int)) != 0 ) {
        sendstring( 0, "recvdata fail\n");
        length = 0;
    }

    return length;
}

int summit( unsigned char *data, int length )
{
    int sum = 0;
    int index = 0;

    for ( index = 0; index < length; index++) {
        sum += data[index];
    }

    return sum;
}

void getdata( unsigned int length )
{
    struct {
        unsigned char data[800];
        char *result;
    } locals;

    int yo = 0;
    int i = 0;
    char outy[25];
    unsigned char *t;

    if (allocate( 4096, 0, (void**)&t) != 0) {
        _terminate(0);
    }

    locals.result = "SUCCESS: ";

#ifdef PATCHED_1
    if (length > 800) {
        length = 800;
        sendstring(0, "That is too big to stuff in me.\n");
        return;
    }
#else
    if ( length > 804 )
    {
        length = 804;
    }
#endif

    for ( yo = 0; yo < 25; yo++) {
        outy[yo] = 0;
    }

    recvdata( 0, (char*)t, length );
    yo = summit( t, length );

    for ( i = 0; i < length; i++ ) {
        locals.data[i] = t[i];
    }

    if ( yo != 0x31337) {
        locals.result = "FAIL: ";
    }

    itoa( outy, 25, yo);

    sendstring(0, locals.result);
    sendstring(0, outy);
    sendstring(0, "\n");

    return;
}

int main(void) {
    unsigned int length;

    sendstring( 0, "Enter the length: ");
    length = get_length();

    if ( length <= 0 ) {
        sendstring(0, "Greater than 0 length required.\n");
        _terminate(0);
    }

    getdata( length );

	return 0;
}
