#include <libcgc.h>
#include "libc.h"

unsigned int get_length( )
{
    unsigned int length;

    if ( recvdata( 0, (char*)&length, sizeof(unsigned int)) != 0 ) {
        sendstring( 1, "recvdata fail\n");
        length = 0;
    }

    return length;
}

void freqanal( unsigned int length, unsigned char *data )
{
    unsigned char fa[256];
    int i = 0;

    if ( data == NULL ) {
        return;
    }

    for ( i = 0; i < 256; i++ ) {
        fa[i] = 0;
    }

    for ( i = 0; i < length; i++) {
        fa[ data[i] ] ++;
    }

    sendstring( 1, "Freq Anal Incoming: ");
    senddata( 1, (char*)fa, 256);

    return;
}

void getdata( unsigned int length )
{
    unsigned char data[512];

    recvdata( 0, (char*)data, length );

    freqanal( length, data );

    return;
}

int main(void) {
    unsigned int length;

    unsigned int eax;
    unsigned int eip;

    sendstring( 1, "Enter the length: ");
    length = get_length();

    if ( length == 0 ) {
        sendstring(1, "Greater than 0 length required.\n");
        _terminate(0);
#ifdef PATCHED_1
    } else if ( length > 512 ) {
#else
    } else if ( length > 1024 ) {
#endif
        sendstring( 1, "Too big\n");
        _terminate(0);
    }

    getdata( length );

	return 0;
}
