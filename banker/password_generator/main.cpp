#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "base64.h"
#include "well_rng.h"

void ReadDevURandomSeed( void )
{
        FILE *pFile;
        uint32_t rngInitData[16];

        pFile = fopen( "/dev/urandom", "rb" );

        if ( !pFile )
        {
                printf( "Couldn't open /dev/urandom reverting to seed.\n" );
                srand( time(NULL) ^ getpid());

                for ( uint32_t i = 0; i < 16; i++ )
                        rngInitData[i] = rand();

                InitWELLRNG512a( rngInitData );

                return;
        }

        if ( fread( rngInitData, sizeof(uint32_t), 16, pFile ) != 16 )
        {
                printf( "Couldn't get enough entropy.  Reverting to seed.\n" );
                for ( uint32_t i = 0; i < 16; i++ )
                        rngInitData[i] = rand();

                InitWELLRNG512a( rngInitData );

                return;
        }

        fclose( pFile );

        InitWELLRNG512a( rngInitData );
}

uint32_t GetRandomCharacterIndex( void )
{
	return (WELLRNG512a() * 62.0);
}

int main( void )
{
	ReadDevURandomSeed();

	char szPassword[12];

	uint32_t i;
	for ( i = 0; i < 8; i++ )
	{
		uint32_t charIdx = GetRandomCharacterIndex();

		if ( charIdx < 26 )
			szPassword[i] = 'a' + charIdx;
		else if ( charIdx < 52 )
			szPassword[i] = 'A' + (charIdx-26);
		else
			szPassword[i] = '0' + (charIdx-52);
	}	

	szPassword[i] = '\0';

	FILE *pUserFile = fopen( "/tmp/users.txt", "w" );

	if ( !pUserFile )
	{
		printf( "Error: Couldn't open users.txt for initial container startup, contact an admin!\n" );
		return 0;
	}

	uint8_t *pEncodedPassword = base64_encode( (uint8_t *)szPassword, strlen(szPassword), NULL );
	fprintf( pUserFile, "admin %s 1\n", pEncodedPassword );

	delete pEncodedPassword;

	fclose( pUserFile );
}
