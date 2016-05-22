#ifndef __UNISTD__
#define __UNISTD__

#define NULL 	(void*)0

#define STDIN 	0
#define STDOUT 	1
#define STDERR 	2

#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002


extern int write(int fd, const void *buf, unsigned int count);
extern void exit(int status) __attribute__ ((noreturn));
extern int read(int fd, void *buf, unsigned int count);
extern int open(const char *pathname, int flags, unsigned int mode);
extern int close(int fd);

#endif
