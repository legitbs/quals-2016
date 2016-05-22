#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>


void scramble(unsigned char in[64]) {
        unsigned char out[64];
        uint8_t i;

        bzero(out, 64);

        for (i = 0; i < 64; i++) {
                out[i] = (in[i] & 0x80) ? 0x08 : 0; // 0 -> 4
                out[i] |= ((in[i] & 0x40) >> 1); // 1 -> 2
                out[i] |= ((in[i] & 0x20) << 1); // 2 -> 1
                out[i] |= ((in[i] & 0x10) >> 4); // 3 -> 7
                out[i] |= ((in[i] & 0x08) >> 1); // 4 -> 5
                out[i] |= ((in[i] & 0x04) >> 1); // 5 -> 6
                out[i] |= ((in[i] & 0x02) << 3); // 6 -> 3
                out[i] |= ((in[i] & 0x01) << 7); // 7 -> 0
        }
        memcpy(in, out, 64);

}


void unscramble(unsigned char in[64]) {
        unsigned char out[64];
        uint8_t i;

        bzero(out, 64);

        for (i = 0; i < 64; i++) {
                out[i] |= (in[i] & 0x80) ? 0x1 : 0; // 0 -> 7
                out[i] |= ((in[i] & 0x40) >> 1); // 1 -> 2
                out[i] |= ((in[i] & 0x20) << 1); // 2 -> 1
                out[i] |= ((in[i] & 0x10) >> 3); // 3 -> 6
                out[i] |= ((in[i] & 0x08) << 4); // 4 -> 0
                out[i] |= ((in[i] & 0x04) << 1); // 5 -> 4
                out[i] |= ((in[i] & 0x02) << 1); // 6 -> 5
                out[i] |= ((in[i] & 0x01) << 4); // 7 -> 3
        }
        memcpy(in, out, 64);

}

int main(void) {
	unsigned char in[64];
	int i;

	bzero(in, 64);
	printf("Target: ");
	fgets(in, 64, stdin);
	for (i = 0; i < 32; i++) {
		printf("\ttarget[%d] = '\\x%02x';\n", i, in[i]);
	}
	scramble(in);
	printf("Input string to step: ");
	for (i = 0; i < 32; i++) {
		printf("\\x%02x", in[i]);
	}
	printf("\n");

	return(0);
}
