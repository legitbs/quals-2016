#include <libcgc.h>
#include "libc.h"

int strlen( const char *str)
{
    int count = 0;

    if ( str == NULL ) {
        return 0;
    }

    while ( str[count] ) {
        count++;
    }

    return count;
}

int sendstring( int fd, const char *str)
{
    return senddata( fd, str, strlen(str));
}

int senddata(int fd, const char *outdata, const size_t size) {
    size_t tx_bytes = 0;
    size_t count = 0;

    if ( outdata == NULL) {
        return -1;
    } 

    while (count < size) {
        if ( transmit(fd, outdata + count, size - count, &tx_bytes) != 0 ) {
            _terminate(0);
        }

        if (tx_bytes <= 0) {
            _terminate(0);
        }

        count += tx_bytes;
    }

    return 0;
}

int recvdata(int fd, char *indata, const size_t size ) {
    size_t rx_bytes = 0;
    size_t total = 0;

    if (indata == NULL ) {
        return -1;
    } 

    while (total < size) {
        if ( receive(fd, indata + total, size-total, &rx_bytes) != 0 ) {
            _terminate(0);
        }

        total += rx_bytes;
    }

    return 0;
}
