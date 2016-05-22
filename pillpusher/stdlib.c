#include "stdlib.h"
#include "errno.h"
#include "printf.h"

char *strcat( char *dest, const char *src)
{
	int i = 0;
	int j = 0;

	if ( !dest || !src ) {
		return dest;
	}

	while ( dest[i] ) {
		i++;
	}

	while ( src[j] ) {
		dest[i] = src[j];
		i++;
		j++;
	}

	dest[j] = 0x00;
	
	return dest;
}

void *memset( void *s, int c, unsigned int n)
{
	unsigned int index = 0;

	if ( s == NULL ) {
		return NULL;
	}

	for ( index = 0; index < n; index++) {
		((char*)s)[index] = c;
	}

	return s;
}

int strlen( const char *ptr )
{
	int val = 0;

	if ( ptr == NULL ) {
		return 0;
	}

	while ( ptr[val] ) {
		val++;
	}

	return val;
}

int isdigit( char c )
{
	int result = 1;

	if ( c < '0' || '9' < c ) {
		result = 0;
	}

	return result;
}

int atoi(const char *nptr)
{
	int index = 0;
	int value = 0;
	int sign = 1;

	if ( nptr == NULL ) {
		errno = EINVAL;
		return -1;
	}

	if (nptr[0] == '-' ) {
		sign = -1;
		index++;
	}

	while ( nptr[index] ) {
		if ( !isdigit( nptr[index] ) ) {
			return value;
		}

		if ( value != 0 ) {
			value *= 10;
		}

		value += (nptr[index] - 0x30);

		index++;		
	}

	return value * sign;
}

char *strcpy(char *dest, const char *src)
{
	int index = 0;

	if ( dest == NULL || src == NULL ) {
		errno = EINVAL;
		return dest;
	}

	while ( src[index] ) {
		dest[index] = src[index];
		index++;
	}

	dest[index] = '\0';

	return dest;
}

void *memcpy(void *dest, const void *src, unsigned int n)
{
	int index = 0;

	if ( !dest || !src ) {
		errno = EINVAL;
		return dest;
	}

	while ( index < n ) {
		((char*)dest)[index] = ((char*)src)[index];
		index++;
	}

	return dest;
}

long int random(void)
{
	int fd = 0;
	long int r;

	fd = open("/dev/urandom", O_RDONLY, 0);

	if ( fd <= 0 ) {
		/// Chosen by random dice roll
		return 4;
	}

	read( fd, &r, sizeof(long int));

	close(fd);

	return r;
}

int strcmp(const char *s1, const char *s2)
{
	int index = 0;

	if ( !s1 || !s2 ) {
		errno = EINVAL;
		return -1;
	}

	while ( s1[index] && s2[index]) {
		if ( s1[index] != s2[index] ) {
			return s1[index]-s2[index];
		}
		index++;
	}

	return s1[index]-s2[index];
}

int strncmp(const char *s1, const char *s2, unsigned int n)
{
	int index = 0;

	if ( !s1 || !s2 ) {
		errno = EINVAL;
		return -1;
	}

	while ( (s1[index] && s2[index]) && ( index < n ) ) {
		if ( s1[index] != s2[index] ) {
			return s1[index]-s2[index];
		}
		index++;
	}

	if ( index == n ) {
		return 0;
	} else {
		return s1[index]-s2[index];
	}
}

asm(".globl memcpy_r\n\t"
    ".type memcpy_r, @function\n\t"
    "memcpy_r:\n\t"
    ".cfi_startproc\n\t"
    "pushq %rcx\n\t"
    "pushq %rdi\n\t"
    "movq %rdx, %rcx\n\t"
    "rep movsb\n\t"
    "popq %rax\n\t"
    "popq %rcx\n\t"
    "ret\n\t"
    ".cfi_endproc");