#include <stdio.h>
#include <stdlib.h>

#include "cpp_helper.h"

void* operator new( size_t count )
{
	return (void *)malloc( count );
}

void* operator new[]( size_t count )
{
	return (void *)malloc( count );
}

void operator delete( void *ptr )
{
	free( ptr );
}

void operator delete[]( void *ptr )
{
	free( ptr );
}
