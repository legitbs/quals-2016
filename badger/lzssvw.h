#ifndef __LZSSVW_H__
#define __LZSSVW_H__

#include <stdint.h>

int16_t lzssvw_decompress( uint8_t *pCompressData, uint8_t *pUncompressData, uint16_t inBufSize, uint16_t outBufSize );

#endif // __LZSSVW_H__
