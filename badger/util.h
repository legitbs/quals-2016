#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

uint8_t hex_char_to_int( uint8_t value );
uint8_t read_hex_uint8( char *string );
uint8_t int_to_hex_char( uint8_t nibble );

#endif // __UTIL_H__
