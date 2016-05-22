#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t *base64_encode( const uint8_t *src, size_t len, size_t *out_len );
uint8_t *base64_decode( const uint8_t *src, size_t len, size_t *out_len );

#endif // __BASE64_H__
