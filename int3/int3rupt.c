#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>

#include <sys/mman.h>

#define __USE_GNU
#include <ucontext.h>
#include <signal.h>
#include <errno.h>

int readUntil(int fd, char *buf, int num, char delim);

int loop = 0;

typedef struct _score {
	char score;
	char name[64];
} score;


typedef struct _breakpoint_list {
	char * addr;
	char orig;
	struct _breakpoint_list *next;
} breakpoint_list;

score highscores[20];
breakpoint_list *bps;
int started = 0;

void showHighScores() {
	int i;
	for(i=0;i<20;i++) {
		if(highscores[i].score == 0)
			break;
		printf("%d. %s - %d\n", i+1, highscores[i].name, highscores[i].score);
	}
}

int readInt() {
	char line[32];
	int ret;
	memset(line, 0, 32);
	readUntil(fileno(stdin), line, 32, '\n');
	line[31] = 0;
	ret= strtol(line, NULL, 10);
	return ret;
}

void doScores(char score) {
	int i = 0;
	int place;
	int k = 0;
	for(i=0;i<20;i++) {
		if(score > highscores[i].score) {
			place = i;
			printf("Congrats! You got a high score!\nPlease enter your name: ");
			for(k=20;k>i;k--) {
				highscores[k].score = highscores[k-1].score;
				memcpy(highscores[k].name, highscores[k-1].name, 16);
			}
			highscores[i].score = score;
			readUntil(fileno(stdin), highscores[i].name, 16, '\n');
			break;
		}
	}
	return;
}

void Game() {
	FILE *rng;
	unsigned char gameToPlay;
	unsigned int result;
	unsigned int guessed;
	char score = 0;
	unsigned char quit = 0;

	rng = fopen("/dev/urandom", "r");
	printf("Shall we play a game?\n");
	while(quit == 0) {
		fread(&gameToPlay, 1, 1, rng);
		switch(gameToPlay % 4) {
			case 0:
				printf("Roll a dice!\n");
				fread(&result, 1, 4, rng);
				printf("What did I get?\n");
				guessed = readInt();
				if(guessed < 1 || guessed > 6) {
					printf("You're not even trying.\n");
					exit(-1);
				} else {
					if(guessed == (result % 6) + 1) {
						printf("So much yes!\n");
						score++;
					} else {
						printf("Nope.\n");
						doScores(score);
						score = 0;
					}
				}
				break;
			case 1:
				printf("Roll a d20!\n");
				fread(&result, 1, 4, rng);
				printf("What did I get?\n");
				guessed = readInt();
				if(guessed < 1 || guessed > 20) {
					printf("You're not even trying.\n");
					exit(-1);
				} else {
					if(guessed == (result % 20) + 1) {
						printf("So much yes!\n");
						score++;
					} else {
						printf("Nope.\n");
						doScores(score);
						score = 0;
					}
				}
				break;
			case 2:
				printf("I'm thinking of a number!\n");
				fread(&result, 1, 4, rng);
				printf("It's between 1 and 10000. What is it?\n");
				guessed = readInt();
				if(guessed < 1 || guessed > 10000) {
					printf("You're not even trying.\n");
					exit(-1);
				} else {
					if(guessed == (result % 10000) + 1) {
						printf("So much yes!\n");
						score++;
					} else {
						printf("Nope.\n");
						doScores(score);
						score = 0;
					}
				}
				break;
			case 3:
				printf("How about a nice game of chess?\n");
				sleep(2);
				printf("I'm kidding. I'm way too lazy to write a chess bot for a CTF challenge.\n");
				break;
		}
		printf("Would you like to play again? (0 for no, 1 for yes)\n");

		if(readInt() == 0)
		{
			started = 0;
			quit = 1;
			doScores(score);
		}
	}
	showHighScores();
}

void restoreAllBreakpoints()
{
	char *addr;
	breakpoint_list *ptr;

	ptr = bps;

	while(ptr != NULL)
	{
		addr = (char *)((long long)(ptr->addr) & 0xfffffffffffff000);
		mprotect(addr, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
		*ptr->addr = '\xcc';
		mprotect(addr, 0x1000, PROT_READ | PROT_EXEC);
		ptr = ptr->next;
	}
}

void addBreakpoint(char *addr, char orig) {
	breakpoint_list *ptr;
	ptr = bps;
	if(ptr->next == NULL && ptr->addr == NULL)
	{
		ptr->addr = addr;
		ptr->orig = orig;
		return;
	}

	while(ptr->next != NULL) {
		ptr = ptr->next;
	}
	ptr->next = calloc(sizeof(breakpoint_list), 1);
	ptr = ptr->next;
	ptr->addr = addr;
	ptr->orig = orig;
}

breakpoint_list *findBreakpoint(long long addr) {
	breakpoint_list *ptr;
	ptr = bps;
	while((long long)ptr->next != (long long)NULL && (long long)ptr->addr != addr) {
		ptr = ptr->next;
	}
	if((long long)ptr->addr == addr) {
		return ptr;
	} else {
		return NULL;
	}
}

int readUntil(int fd, char *buf, int num, char delim) {
	int size = 0;
	char *tmp;
	tmp = buf;
	while(read(fd, tmp, 1)) {
		if(*tmp == delim) {
			*tmp = '\x00';
			return size;
		} else if (num == size) {
			return size;
		}
		size++;
		tmp++;
	}
	exit(-1);
}

int isGoodPtr(void * addr) {
	int ret;
	errno = 0;
	if((long long)addr == 0)
		return 0;
	ret = stat(addr, addr);
	if(ret == -1) {
		if(errno == EFAULT)
			return 0;
	}
	return 1;
	
}

void dumpAddressRange(char *addr, int size)
{
	char *tmpAddr;
	int i = 0;

	tmpAddr = addr;

	printf("%p: ", addr);
	while(i < size) {
		printf("%.2hhX", *(char *)(addr+i));
		if(((i+1) % 8) == 0 && ((i+1) % 16) != 0)
			printf("-");
		else if ((i+1) % 16 == 0 && i != size-1) {
			printf("\n%p: ", addr+i+1);
		} else {
			printf(" ");
		}
		i++;
	}
	puts("");
}

void ITSATRAP(int signum, siginfo_t* siginfo, void * context)
{
	ucontext_t *cptr;
	cptr = context;
	breakpoint_list *bp;
	char *prot_addr;

	if(siginfo->si_code == TRAP_TRACE) {
		restoreAllBreakpoints();
		cptr->uc_mcontext.gregs[REG_EFL] &= ~0x100;
		debugMenu();
	} else {
		printf("RAX:\t%.16lx\tRBX:\t%.16lx\tRCX:\t%.16lx\tRDX:\t%.16lx\n"
			"RSI:\t%.16lx\tRDI:\t%.16lx\tRBP:\t%.16lx\tRSP:\t%.16lx\n"
			"R8:\t%.16lx\tR9:\t%.16lx\tR10:\t%.16lx\tR11:\t%.16lx\n"
			"R12:\t%.16lx\tR13:\t%.16lx\tR14:\t%.16lx\tR15:\t%.16lx\n"
			"RIP:\t%.16lx\tFLAGS:\t%.16lx\n", 
			(long unsigned int)cptr->uc_mcontext.gregs[REG_RAX], (long unsigned int)cptr->uc_mcontext.gregs[REG_RBX], (long unsigned int)cptr->uc_mcontext.gregs[REG_RCX], (long unsigned int)cptr->uc_mcontext.gregs[REG_RDX],
			(long unsigned int)cptr->uc_mcontext.gregs[REG_RSI], (long unsigned int)cptr->uc_mcontext.gregs[REG_RDI], (long unsigned int)cptr->uc_mcontext.gregs[REG_RBP], (long unsigned int)cptr->uc_mcontext.gregs[REG_RSP],
			(long unsigned int)cptr->uc_mcontext.gregs[REG_R8], (long unsigned int)cptr->uc_mcontext.gregs[REG_R9], (long unsigned int)cptr->uc_mcontext.gregs[REG_R10], (long unsigned int)cptr->uc_mcontext.gregs[REG_R11],
			(long unsigned int)cptr->uc_mcontext.gregs[REG_R12], (long unsigned int)cptr->uc_mcontext.gregs[REG_R13], (long unsigned int)cptr->uc_mcontext.gregs[REG_R14], (long unsigned int)cptr->uc_mcontext.gregs[REG_R15],
			(long unsigned int)cptr->uc_mcontext.gregs[REG_RIP]-1, (long unsigned int)cptr->uc_mcontext.gregs[REG_EFL]);
		cptr->uc_mcontext.gregs[REG_EFL] |= 0x100; 
		cptr->uc_mcontext.gregs[REG_RIP]--;
		bp = findBreakpoint(cptr->uc_mcontext.gregs[REG_RIP]);
		if(bp)
		{
			prot_addr = (char *)((long long)(bp->addr) & 0xfffffffffffff000);
			mprotect(prot_addr, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
			*(bp->addr) = bp->orig;
			mprotect(prot_addr, 0x1000, PROT_READ | PROT_EXEC);
		} else {
			printf("Could not find BP!!!");
		}
	}
}
int debugMenu() {
	char line[256];
	long long addr;
	char *target;
	char *token;
	
	while (1) {
		memset(line, 0, 256);
		printf("> ");
		readUntil(fileno(stdin), line, 256, '\n');

		token = strtok(line, " ");
		if(token && *token) {
			if(strcmp(token, "bp") == 0) {
				addr = 0;
				token = strtok(NULL, " \n");
				if(token) {
					addr = strtoul(token, NULL, 16);
				}
				target = (char *)addr;
				if(!isGoodPtr(addr)) {
					printf("Not a good ptr.\n");
					continue;
				}
				if(!findBreakpoint((long long)target)) {
					addBreakpoint(target, *target);
					addr = addr & 0xfffffffffffff000;
					mprotect((void *)addr, 0x1000, PROT_READ | PROT_EXEC | PROT_WRITE);
					*target = '\xcc';
					mprotect((void *)addr, 0x1000, PROT_READ | PROT_EXEC);
				}
				addr = 0;
			}
			if(strcmp(token, "db") == 0) {
				token = strtok(NULL, " \n");
				if(token) {
					addr = strtoul(token, NULL, 16);
				}
				if(isGoodPtr((void *)addr) && isGoodPtr((void *)addr+0x40))
					dumpAddressRange((char *)addr, 0x40);
				else
					printf("%p is not readable.\n", (void *)addr);
				addr+=0x40;
			}
			if(strcmp(token, "g") == 0) {
				if(started == 0) {
					started = 1;
					Game();
				} else {
					printf("Process is already running.\n");
				}
			}
			if(strcmp(token, "c") == 0) {
				return 1;
			}
			if(strcmp(token, "q") == 0) {
				_exit(0);
			}
			if(strcmp(token, "?") == 0) {
				printf("Help menu:\n");
				printf("\tg - Begin execution\n");
				printf("\tdb <addr> - Dump bytes at addr\n");
				printf("\tbp addr - Set breakpoint at addr\n");
				printf("\tc - Continue from breakpoint.\n");
				printf("\tq - Quit\n");
				printf("\t? - This text\n");
			}
		}
	}
}
int main(int argc, char **argv) {
	struct sigaction new_action, old_action;

	bzero(highscores, sizeof(highscores));
	new_action.sa_handler = &ITSATRAP;
	new_action.sa_flags = SA_SIGINFO;
	sigaction(SIGTRAP, &new_action, NULL);
	bps = calloc(sizeof(breakpoint_list), 1);
	setvbuf(stdout, NULL, _IONBF, 0);
	debugMenu();
}
