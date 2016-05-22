#include <stdio.h>
#include <string.h>
#include <stdint.h>

void enc(unsigned int starting_offset, unsigned int len, uint8_t key[4]) {
	unsigned int i;
	uint8_t key_byte = 0;
	FILE *step;
	unsigned char c;

	step = fopen("step", "rb+");
	for (i = starting_offset; i < starting_offset+len; i++) {
		fseek(step, i, SEEK_SET);
		fread(&c, 1, 1, step);
		fseek(step, i, SEEK_SET);
		c = c ^ key[key_byte];
		key_byte = (key_byte+1)%4;
		fwrite(&c, 1, 1, step);
	}
	fclose(step);
		
}

void enc_setup(unsigned char key[4]) {
	FILE *in;
	char line[1000];
	unsigned int starting_addr, len;
	char dc1, dc2;

	in = fopen("step.objdump", "r");

	// encrypt init entirely
	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (!strncmp(line+61, "setup", 5)) {
			// 00000000004008ed g     F .text  0000000000000060              setup
			sscanf(line, "%16x %c     %c .text  %16x              setup", &starting_addr, &dc1, &dc2, &len);
			//printf("Encrypting setup at %08x for length %d\n", starting_addr, len );
			enc(starting_addr & 0xffff, len, key);
			break;
		}
	}
	fclose(in);

}

void enc_setup_end() {
	FILE *in;
	char line[1000];
	unsigned int starting_addr, len;
	char dc1, dc2;
	int found = 0;
	int found2 = 0;
	unsigned int addr;
	unsigned int opcode;
	FILE *step;
	unsigned char c;

	step = fopen("step", "rb+");

	in = fopen("step.objdump", "r");

	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (!found) {
			if (!strncmp(line, "setup", 5)) {
				//printf("Found start of setup\n");
				found = 1;
			}
			continue;
		}


		if (!strcmp(line, "")) {
			break;
		}

		// skip any instructions wrapping to a 2nd line
		if (strlen(line) < 32) {
			continue;
		}

		if (!found2) {
			if (!strncmp(line+32, "popf", 4)) {
				found2 = 1;
				fgets(line, 999, in);
			}
			continue;
		}

		// encrypt the first byte of this instruction
		sscanf(line, "  %06x:       %02x", &addr, &opcode);
		//printf("encrypting at %06x %02x to %02x\n", addr, opcode, (opcode ^ (addr & 0xff)));
		fseek(step, addr&0xffff, SEEK_SET);
		fread(&c, 1, 1, step);
		fseek(step, addr&0xffff, SEEK_SET);
		c = c ^ (addr & 0xff);
		fwrite(&c, 1, 1, step);
		
	}

	fclose(in);
	fclose(step);

}

void enc_handler(unsigned char key[4]) {
	FILE *in;
	char line[1000];
	unsigned int starting_addr, len;
	char dc1, dc2;

	in = fopen("step.objdump", "r");

	// encrypt init entirely
	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (!strncmp(line+61, "sig_trap_handler", 16)) {
			// 00000000004007f3 g     F .text  00000000000000e9              sig_trap_handler
			sscanf(line, "%16x %c     %c .text  %16x              sig_trap_handler", &starting_addr, &dc1, &dc2, &len);
			//printf("Encrypting handler\n");
			enc(starting_addr & 0xffff, len, key);
			break;
		}
	}
	fclose(in);

}

void enc_function(unsigned char *func) {
	FILE *in;
	char line[1000];
	int found = 0;
	unsigned int addr;
	unsigned int opcode;
	FILE *step;
	unsigned char c;

	step = fopen("step", "rb+");

	in = fopen("step.objdump", "r");

	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (found) {
			if (strlen(line) == 0) {
				break;
			}
			// skip any instructions wrapping to a 2nd line
			if (strlen(line) < 32) {
				continue;
			}

			sscanf(line, "  %06x:       %02x", &addr, &opcode);
			//printf("encrypting at %06x %02x to %02x\n", addr, opcode, (opcode ^ (addr & 0xff)));
			fseek(step, addr&0xffff, SEEK_SET);
			fread(&c, 1, 1, step);
			fseek(step, addr&0xffff, SEEK_SET);
			c = c ^ (addr & 0xff);
			fwrite(&c, 1, 1, step);
		}
		if (!strncmp(line, func, strlen(func))) {
			// found the start of the function
			//printf("found start of %s\n", func);
			//printf("line: %s\n", line);
			found = 1;
			continue;
		}
	}
	fclose(in);
	fclose(step);
}	

void enc_main(void) {
	FILE *in;
	char line[1000];
	unsigned int starting_addr, len;
	char dc1, dc2;
	int found = 0;
	int found2 = 0;
	unsigned int addr;
	unsigned int opcode;
	FILE *step;
	unsigned char c;

	step = fopen("step", "rb+");

	in = fopen("step.objdump", "r");

	//printf("Encrypting main\n");
	// find the start of main
	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (!strncmp(line+17, "<main>", 6)) {
			break;
		}
	}

	while (!feof(in)) {
		fgets(line, 999, in);
		line[strlen(line)-1] = '\0';
		if (!found) {
			if (!strncmp(line+47, "setup", 5)) {
				//printf("%s\n", line);
				found = 1;
			}
			continue;
		}

		if (!strcmp(line, "")) {
			break;
		}

		// skip any instructions wrapping to a 2nd line
		if (strlen(line) < 32) {
			continue;
		}

		// encrypt the first byte of this instruction
		sscanf(line, "  %06x:       %02x", &addr, &opcode);
		printf("encrypting at %06x %02x to %02x\n", addr, opcode, (opcode ^ (addr & 0xff)));
		fseek(step, addr&0xffff, SEEK_SET);
		fread(&c, 1, 1, step);
		fseek(step, addr&0xffff, SEEK_SET);
		c = c ^ (addr & 0xff);
		fwrite(&c, 1, 1, step);
		
	}

	fclose(in);
	fclose(step);
}

int main(void) {

	// encrypt last instructions of setup after the popf
	enc_setup_end();

	// encrypt setup function entirely
	enc_setup("RotM");

	// encrypt sig_trap_handler entirely
	enc_handler("RotM");

	// encrypt first bytes of remaining functions
	enc_function("go");
	enc_function("unscramble");
	enc_function("CheckString");

	// encrypt first bytes after setup() call in main
	enc_main();
}
