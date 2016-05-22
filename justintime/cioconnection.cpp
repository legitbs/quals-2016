#include "common.h"	

#include <fcntl.h>      /* Needed only for _O_RDWR definition */
#include <stdlib.h>
#include <stdio.h>

int32_t CIOStdioConnection::ReadLine( uint8_t *pBuffer, uint32_t maxLen )
{
	uint32_t linePos = 0;
	unsigned char curChar;
	int readCount;

	for ( ; linePos < maxLen; linePos++ )
	{
		readCount = read( 0, &curChar, 1 );
		
		if ( readCount != 1 )
		{
			exit(1);
			break;
		}
 
		pBuffer[linePos] = curChar;

		if ( curChar == '\n' )
			break;
	}

	pBuffer[linePos] = '\0';

	return (linePos);
}
	
int32_t CIOStdioConnection::WriteLine( uint8_t *pBuffer, uint32_t maxLen )
{
	return write( 1, pBuffer, maxLen );
}

int32_t CIOStdioConnection::RecvUntil( uint8_t *pBuffer, uint32_t count )
{
	uint32_t pos = 0;

	for ( pos = 0; pos < count; pos++ )
	{
		if ( read( 0, pBuffer+pos, 1 ) != 1 )
			return (-1);
	}

	return (pos);
}

bool CIOStdioConnection::IsInputAvailable( void )
{
	fd_set fds;
	int maxfd;
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	maxfd = fileno(stdin);
	
	FD_ZERO( &fds );
	FD_SET( fileno(stdin), &fds );

	select( maxfd+1, &fds, NULL, NULL, &timeout );

	if ( FD_ISSET( fileno(stdin), &fds ) )
		return (true);
	else
		return (false);
}

int32_t CIOStdioConnection::ReadAvailableInput( uint8_t *pBuffer, uint32_t maxLen )
{
	// Read up to max length -- return amount read...
	return (read( fileno(stdin), pBuffer, maxLen ));
}


/*
int32_t CIOFileConnection::ReadLine( uint8_t *pBuffer, uint32_t maxLen )
{
	uint32_t linePos = 0;

	for ( ; linePos < maxLen; linePos++ )
	{
		int curChar = fgetc( m_pFile );
		
		if ( curChar == EOF )
			break;

		pBuffer[linePos] = curChar & 0x7F;

		if ( curChar == '\r' )
		{
			linePos++;
			if ( linePos < maxLen )
			{
				curChar = fgetc( m_pFile );
		
				if ( curChar == EOF )
					break;

				pBuffer[linePos] = curChar & 0x7F;

				if ( curChar == '\n' )
					break;
			}
		}
	}

	return (linePos);
}
	
int32_t CIOFileConnection::WriteLine( uint8_t *pBuffer, uint32_t maxLen )
{
	return fwrite( pBuffer, 1, maxLen, m_pFile );
}

int32_t CIOFileConnection::RecvUntil( uint8_t *pBuffer, uint32_t count )
{
	if ( fread( pBuffer, 1, count, m_pFile ) != count )
		return (-1);

	return (count);	
}
*/
