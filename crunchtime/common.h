#ifndef DEFCON_COMMON
#define DEFCON_COMMON

#ifndef __MACH__
#define _GNU_SOURCE 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <err.h>
#include <stdarg.h>

#ifdef _SEED_
#include <time.h>
#endif

// socket read and write functions
ssize_t sendMsg(char *);
ssize_t sendMsgf(const char *, ...);
//#define sendMsgf printf
ssize_t readUntil(char *buf, size_t len, char sentinal);

#endif
