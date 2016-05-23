#include "input_reader.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define STDIN	(0)
#define STDOUT	(1)
#define STDERR	(2)

String CInputReader::ReadLine( uint32_t maxLen )
{
	char *pszTemp = new char[maxLen+1];

	uint32_t lastPos = 0;
	bool bLineFound = false;
	for (;;)
	{
		bLineFound = false;
		uint32_t curPos = 0;
		for ( curPos = lastPos; curPos < m_bufferLoc; curPos++ )
		{
			if ( m_pszBuffer[curPos] == '\n' )	
			{
				bLineFound = true;
				break;
			}
		}

		if ( bLineFound )
		{
			lastPos = curPos;
			break;
		}

		if ( curPos == m_bufferLoc && m_bufferLoc >= MAX_READ_BUFFER_LEN )
		{
			// Out of line buffer
			lastPos = curPos;
			break;
		}

		lastPos = curPos;
	
		int readRemaining = (maxLen - curPos);
		if ( readRemaining > (MAX_READ_BUFFER_LEN-m_bufferLoc) )
			readRemaining = (MAX_READ_BUFFER_LEN-m_bufferLoc);

		if ( readRemaining == 0 )
		{
			// Max length reached without newline
			break;
		}
	
		// Read more into input buffer
		int dataRead = read( STDIN, (m_pszBuffer+m_bufferLoc), readRemaining );
	
		if ( dataRead == 0 )	
		{
			// EOF
			break;
		}

		if ( dataRead == -1 )
		{
			// Critical error
			exit(-1);
		}

		m_bufferLoc += dataRead;
	}

	if ( lastPos > maxLen )
		lastPos = maxLen;

	memcpy( pszTemp, m_pszBuffer, lastPos );
	pszTemp[lastPos] = '\0';

	String sTemp(pszTemp);

	delete pszTemp;

	// copy back -- make sure to use memmove due to overlapping
	if ( bLineFound )
	{
		memmove( m_pszBuffer, m_pszBuffer+(lastPos+1), (m_bufferLoc-(lastPos+1)) );
		m_bufferLoc -= (lastPos+1);
	}
	else
	{
		// else-- only copy what we've read 
		memmove( m_pszBuffer, m_pszBuffer+lastPos, (m_bufferLoc-lastPos) );
		m_bufferLoc -= lastPos;
	}

	return sTemp;
}
