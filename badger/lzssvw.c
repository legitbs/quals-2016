#include "lzssvw.h"
#include "bit_util.h"

#define LEN_WIDTH_MAX		9
#define COPY_WIDTH		5
#define COPY_WINDOW_SIZE	(1<<5)

int16_t lzssvw_decompress( uint8_t *pCompressData, uint8_t *pUncompressData, uint16_t inBufSize, uint16_t outBufSize )
{
	int i, j, c;
	int compress_len_width = 1;	// Variable width length encoding (up to maximum)
	int out_pos;
	tBitState inBitState;

	out_pos = 0;

	text_init_state( &inBitState, pCompressData ); 

	while ( (c = text_getbit( 1, &inBitState, inBufSize )) != -1 )
	{
		if ( c )
		{
 			if ( (c = text_getbit( 8, &inBitState, inBufSize )) == -1 )
				break;

			pUncompressData[out_pos++] = c;

			// FIXED OVERFLOW FROM DEF CON FINALS 2014 (Added this check)
			if ( out_pos >= outBufSize )
				return (out_pos);
		}
		else
		{
			int lookup_distance, repeat_amount;

			// Lookup
			if ( (c = text_getbit( compress_len_width, &inBitState, inBufSize )) == -1 )
				break;

			lookup_distance = c;
			
			if ( (c = text_getbit( COPY_WIDTH, &inBitState, inBufSize )) == -1 )
				break;

			repeat_amount = c+1;

			if ( lookup_distance+1 > out_pos )
				return -1;

			j = (out_pos-(lookup_distance+1));
			for ( i = 0; i < repeat_amount; i++ )
			{
				pUncompressData[out_pos++] = pUncompressData[j];

				// FIXED OVERFLOW FROM DEF CON FINALS 2014 (Added this check)
				if ( out_pos >= outBufSize )
					return (out_pos);

				j++;
			}
		}

		// Update after!
		if ( compress_len_width < LEN_WIDTH_MAX )
		{
			while( out_pos >= (1<<compress_len_width) )
				compress_len_width++;
		}
	}

	return (out_pos);
}
