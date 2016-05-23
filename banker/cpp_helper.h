#ifndef CPP_HELPER_H__
#define CPP_HELPER_H__

void* operator new( size_t count );
void* operator new[]( size_t count );

void operator delete( void *ptr );
void operator delete[]( void *ptr );

#endif // CPP_HELPER_H__
