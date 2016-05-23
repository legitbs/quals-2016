#ifndef __BIT_UTIL_H__
#define __BIT_UTIL_H__
#include <stdint.h>

typedef struct BIT_STATE_STRUCT
{
        uint8_t mask;
        uint8_t bit_buffer;
        uint16_t cur_pos;
        uint8_t *data;
} tBitState;

void text_init_state( tBitState *state, uint8_t *data );
void text_writeflush( tBitState *state );
void text_writebit( uint16_t bit_data, int16_t n, tBitState *state );
int16_t text_getbit( int16_t n, tBitState *state, uint16_t maxLen );

#endif // __BIT_UTIL_H__
