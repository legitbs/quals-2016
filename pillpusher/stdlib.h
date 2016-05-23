#ifndef __STDLIB__
#define __STDLIB__

#include "unistd.h"

int strlen( const char *ptr );
void *memset( void *s, int c, unsigned int n);
int atoi(const char *nptr);
int isdigit( char c );
char *strcpy(char *dest, const char *src);
void *memcpy(void *dest, const void *src, unsigned int n);
long int random(void);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned int n);
char *strcat( char *dest, const char *src);
void *memcpy_r(void *dest, const void *src, unsigned int n);

#endif