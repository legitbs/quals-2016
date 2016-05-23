#include <libcgc.h>
#include "libc.h"
#include "printf.h"

unsigned int get_length( )
{
    unsigned int length;

    if ( recvdata( 0, (char*)&length, sizeof(unsigned int)) != 0 ) {
        sendstring( 0, "recvdata fail\n");
        length = 0;
    }

    return length;
}


int recvuntil( void *dest, unsigned int size, char delim)
{
    unsigned int len = 0;
    char c = 0;

    while ( len < size ) {
        recvdata( 0, &c, 1);

        if ( c == delim ) {
            return len;
        }

        ((char*)dest)[len] = c;

        len++;
    }

    return len;
}

void memcpy( void *dest, void *src, unsigned int l)
{
    int i = 0;

    for ( i = 0; i < l; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }

    return;
}

void memset( void *dest, char val, unsigned int l)
{
    int i = 0;

    for ( i = 0; i < l; i++) {
        ((char*)dest)[i] = val;
    }

    return;
}

void readname( char *name )
{
#ifdef PATCHED_1
    #define MAX     64
#else
    #define MAX     32
#endif

    unsigned char data[MAX];
    register int i = 0;
    register int l =0;

    memset( data, 0, MAX);

    printf("Enter Name: ");

    l = recvuntil( data, 48, '\n' );

    if ( l<= 0 ) {
        return;
    }

    memcpy( name, data, l);

    return;
}

int main(void) {
    char data[10];
    char name[100];

    int i = 0;

    while ( 1 ) {
        printf("1) Gimme Name\n");
        printf("2) Print Name\n");
        printf("3) Exit\n");
        printf(": ");

        for ( i = 0; i < 10; i++) {
            data[i] = 0;
        }

        recvuntil( data, 2, '\n');

        switch ( data[0]-0x30) {
            case 1:
                readname( name );
                break;
            case 2:
                printf("%s\n", name);
                break;
            case 3:
                printf("Exit\n");
                return 0;
                break;
            default:
                printf("Invalid\n");
                break;
        };
    }

	return 0;
}
