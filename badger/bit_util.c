#include "bit_util.h"

void text_init_state( tBitState *state, uint8_t *data )
{
	state->mask = 0;
	state->bit_buffer = 0;
	state->data = data;
	state->cur_pos = 0;
}

void text_writeflush( tBitState *state )
{
	// Check if there is any data left to write out??
	if ( state->mask == 0 )
		return;

	state->mask++;
	
	while ( state->mask < 8 )
	{
		state->mask++;
		state->bit_buffer <<=1;
	}

	// FLUSH
	state->data[state->cur_pos] = state->bit_buffer;
	state->cur_pos++;

	state->mask = 0;
	state->bit_buffer = 0;
}

void text_writebit( uint16_t bit_data, int16_t n, tBitState *state )
{
        int16_t i;

	for ( i = 0; i < n; i++ )
        {
		if ( bit_data & (1<<((n-1)-i)) )
			state->bit_buffer++;

		state->mask++;
		if ( state->mask == 8 )
		{
			state->data[state->cur_pos] = state->bit_buffer;
			state->cur_pos++;

			state->bit_buffer = 0;
                	state->mask = 0;
            	}

		state->bit_buffer <<= 1;
	}
}

int16_t text_getbit( int16_t n, tBitState *state, uint16_t maxLen )
{
        int16_t i, x;

        x = 0;
        for (i = 0; i < n; i++)
        {
                if ( state->mask == 0)
                {
                        if ( state->cur_pos >= maxLen )
                                return (-1);

                        state->bit_buffer = state->data[state->cur_pos];
						state->cur_pos++;

                        state->mask = 128;
                }
                x <<= 1;

                if ( state->bit_buffer & state->mask )
                        x++;

                state->mask >>= 1;
        }
        return x;
}

