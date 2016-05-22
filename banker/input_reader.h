#ifndef __INPUT_READER_H__
#define __INPUT_READER_H__

#define MAX_READ_BUFFER_LEN	(4096)

#include <stdint.h>
#include "string.h"

class CInputReader
{
public:
	CInputReader( )
	{
		m_bufferLoc = 0;
		m_bEOF = false;
	}

	String ReadLine( uint32_t maxLen );

	bool IsEOF( void ) const { return m_bEOF; };

private:
	char m_pszBuffer[MAX_READ_BUFFER_LEN];
	uint32_t m_bufferLoc;

	bool m_bEOF;
};

#endif // __INPUT_READER_H__
