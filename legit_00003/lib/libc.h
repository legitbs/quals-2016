#ifndef LIBC_H
#define LIBC_H
int senddata(int fd, const char *buf, const size_t size);
int recvdata(int fd, char *buf, const size_t size);
int sendstring( int fd, const char *str);
int strlen( const char *str);

#endif
