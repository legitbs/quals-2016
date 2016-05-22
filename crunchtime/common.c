#include "common.h"

ssize_t sendMsg(char *msg) {
	return write(1, msg, strlen(msg));
}

ssize_t sendMsgf(const char *format, ...) {
	ssize_t status = 0;
	va_list list;
	char *ptr = NULL;
	
	va_start(list, format);
	status = vasprintf(&ptr, format, list);
	va_end(list);
    
	if (status == -1)
		goto ret;
    
	status = sendMsg(ptr);
    
ret:
	free(ptr);
	return status;
}

ssize_t readUntil(char *buf, size_t len, char sentinal) {
    char _buf;
    size_t i = 0;
    ssize_t count;
    
    if (len > 0) {
        for (i = 0; i < len; i++) {
            count = read(0, &_buf, 1);
            
            if (count <= 0) {
                exit(0);
                break;
            }
            
            buf[i] = _buf;
            
            if (_buf == sentinal)
                break;
        }
    }
    
    return i;
}
