#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#define __USE_GNU
#include <sys/ucontext.h>
#include <sys/mman.h>

#define REG_RIP (16)

unsigned char FLAG[64];

#define FIRST_INSTRUCTION_BYTE (0x400936)
#define LAST_INSTRUCTION_BYTE (0x400fe3)

void sig_trap_handler(int sig, siginfo_t* siginfo, void *ptr) {
	char buf[] = "SIGTRAP at\n";
	int i;
	uint8_t c;
	uint8_t n;
	ucontext_t *context = (ucontext_t*)ptr;
	static uint8_t *pc;
	static uint8_t *prev_pc;

	prev_pc = pc;
	pc = (uint8_t *)(context->uc_mcontext.gregs[REG_RIP]);

	// don't attempt to decrypt/encrypt code outside
	// of the challenge's functions
	if ((uint64_t)pc < FIRST_INSTRUCTION_BYTE || (uint64_t)pc > LAST_INSTRUCTION_BYTE) {
		return;
	}

	// re-encrypt the previous instruction
	if ((uint64_t)prev_pc >= FIRST_INSTRUCTION_BYTE && (uint64_t)prev_pc <= LAST_INSTRUCTION_BYTE) {
		prev_pc[0] ^= ((uint64_t)prev_pc & 0xFF);
	}

	// decrypt the current instruction
	pc[0] ^= ((uint64_t)pc & 0xFF);

}

void unscramble(unsigned char in[32]) {
	unsigned char out[32];
	uint8_t i;

	bzero(out, 32);

	for (i = 0; i < 32; i++) {		
		out[i] |= ((in[i] & 0x80) >> 7); // 0 -> 7
		out[i] |= ((in[i] & 0x40) >> 1); // 1 -> 2
		out[i] |= ((in[i] & 0x20) << 1); // 2 -> 1
		out[i] |= ((in[i] & 0x10) >> 3); // 3 -> 6
		out[i] |= ((in[i] & 0x08) << 4); // 4 -> 0
		out[i] |= ((in[i] & 0x04) << 1); // 5 -> 4
		out[i] |= ((in[i] & 0x02) << 1); // 6 -> 5
		out[i] |= ((in[i] & 0x01) << 4); // 7 -> 3
	}
	memcpy(in, out, 32);

}

void CheckString(unsigned char in[32]) {
	unsigned char target[32];
	unsigned char nope[5];

	nope[0] = '\x6e';
	nope[1] = '\x6f';
	nope[2] = '\x70';
	nope[3] = '\x65';
	nope[4] = '\x0a';
	nope[5] = '\x00';

	// "Please, may I have the flag now"
	target[0] = '\x50';
	target[1] = '\x6c';
	target[2] = '\x65';
	target[3] = '\x61';
	target[4] = '\x73';
	target[5] = '\x65';
	target[6] = '\x2c';
	target[7] = '\x20';
	target[8] = '\x6d';
	target[9] = '\x61';
	target[10] = '\x79';
	target[11] = '\x20';
	target[12] = '\x49';
	target[13] = '\x20';
	target[14] = '\x68';
	target[15] = '\x61';
	target[16] = '\x76';
	target[17] = '\x65';
	target[18] = '\x20';
	target[19] = '\x74';
	target[20] = '\x68';
	target[21] = '\x65';
	target[22] = '\x20';
	target[23] = '\x66';
	target[24] = '\x6c';
	target[25] = '\x61';
	target[26] = '\x67';
	target[27] = '\x20';
	target[28] = '\x6e';
	target[29] = '\x6f';
	target[30] = '\x77';
	target[31] = '\x00';

	if (!memcmp(in, target, 32)) {
		printf("%s\n", FLAG);
	} else {
		printf(nope);
	}
}

void unprotect(void) {

	mprotect((void *)0x00400000, 4096, PROT_READ|PROT_WRITE|PROT_EXEC);
//	umprotect((void *)0x00401000, 4096, PROT_READ|PROT_WRITE|PROT_EXEC);

}

void decrypt(void *addr, uint32_t len, uint8_t key[4]) {
	unsigned char *c;
	uint8_t key_byte = 0;

	for (c = (unsigned char *)addr; c < (unsigned char *)(addr+len); c++) {
		c[0] = c[0] ^ key[key_byte];
		key_byte = (key_byte+1)%4;
	}

}

void setup(unsigned char key[5]) {
	struct sigaction sa;

	// decrypt the trap handler
	decrypt(sig_trap_handler, 0xec, key);

	// set up trap handler
	sa.sa_sigaction = sig_trap_handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);
	sigaction(SIGTRAP, &sa, NULL);

	// set up single step
	__asm__ (
		"pushf\n\t"
		"pop %rax\n\t"
		"or $0x00000100, %rax\n\t"
		"push %rax\n\t"
		"popf\n\t"
	);

	return;
}

void go(void) {
	FILE *in;
	unsigned char str[32];
	char flag[5];
	char key2[7];

	// read in the flag
	flag[0] = '\x66';
	flag[1] = '\x6c';
	flag[2] = '\x61';
	flag[3] = '\x67';
	flag[4] = '\x00';
	if ((in = fopen(flag, "r")) == NULL) {
		exit(1);
	}
	fgets(FLAG, 64, in);
	fclose(in);

	// read the scrambled string
	key2[0] = '\x4b';
	key2[1] = '\x65';
	key2[2] = '\x79';
	key2[3] = '\x32';
	key2[4] = '\x3a';
	key2[5] = '\x20';
	key2[6] = '\x00';
	printf(key2);
	fflush(stdout);
	fgets(str, 32, stdin);
	str[31] = '\0';

	// unscramble the string
	unscramble(str);

	// See if it's a winner
	CheckString(str);
	
}
	

int main(void) {
	uint32_t i;
	uint32_t j;
	unsigned char key[6];

	printf("Key1: ");
	fflush(stdout);
	fgets(key, 6, stdin);
	key[4] = '\0';
	
	decrypt(setup, 0x99, key);
	setup(key);

	go();

	return(0);

}

void constructor(void) __attribute__((constructor));
void constructor(void) {
	unprotect();
}
