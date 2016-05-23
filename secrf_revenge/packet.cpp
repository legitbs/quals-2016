#include "common.h"

CPacket::CPacket( uint8_t hdrByte, uint8_t len, uint8_t *pData, uint16_t crc16 )
{
	m_header.hdr = hdrByte;
	m_header.len = len;

	m_pData = new uint8_t[len];
	memcpy( m_pData, pData, len );

	m_crc16 = crc16;
}

CPacket::~CPacket( )
{
	if ( m_pData )
		delete [] m_pData;
}

bool CPacket::IsHeaderValid( void )
{
	uint8_t bitpos;

	// Check header signature
	if ( (m_header.hdr & PACKET_HEADER_MASK) != PACKET_HEADER_SIGNATURE )
		return (false);

	// Check parity
	uint8_t parity_value = 0;

	for ( bitpos = 0; bitpos < 8; bitpos++ )
	{
		if ( (1 << bitpos) & PACKET_PARITY_MASK )
			parity_value += (((m_header.hdr & (1 << bitpos)) >> bitpos) & 0x1);
	}

	// Even or odd
	parity_value = parity_value % 2;

	// CHECK
	if ( parity_value == ((m_header.hdr & PACKET_PARITY_BIT) >> PACKET_PARITY_BIT_POS) )
		return (true);
	else
		return (false);
}

bool CPacket::IsCRCValid( void )
{
	const uint16_t POLY = 0x8408;

	/*
	//                                      16   12   5
	// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
	// This works out to be 0x1021, but the way the algorithm works
	// lets us use 0x8408 (the reverse of the bit pattern).  The high
	// bit is always assumed to be set, thus we only use 16 bits to
	// represent the 17 bit value.
	*/
	uint8_t i;
	uint8_t *pData = m_pData;
	uint32_t data = 0;
	uint32_t crc = 0xffff;
	uint32_t length = m_header.len;

	if (length == 0)
	{
		return (m_crc16 == (uint16_t)(~crc));
	}


	do
	{
		for ( i=0, data=(unsigned int)0xff & *pData++; i < 8; i++, data >>= 1 )
		{
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ POLY;
			else  
				crc >>= 1;
		}
	} while (--length);

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | (data >> 8 & 0xff);

	return (m_crc16 == (uint16_t)(crc));
}

bool CPacket::IsValidPacket( void )
{
	if ( !IsHeaderValid() )
		return (false);

	if ( !IsCRCValid() )
		return (false);

	if ( m_header.hdr & PACKET_FRAGMENT_BIT )
	{
		if ( m_header.len < 2 )
			return (false);
		else
			return (true);
	}
	else
		return (true);
}

uint8_t CPacket::GetDataLength( void )
{
	if ( m_header.hdr & PACKET_FRAGMENT_BIT )
	{
		if ( m_header.len < 2 )
			return 0;
		else
			return (m_header.len - 2);
	}
	else
		return (m_header.len); // Not fragmented -- full length
}

uint8_t *CPacket::GetData( void )
{
	if ( m_header.hdr & PACKET_FRAGMENT_BIT )
	{
		if ( m_header.len < 2 )
			return NULL;
		else
			return (m_pData + 2);
	}
	else
		return (m_pData); // Not fragmented -- full length
}

uint8_t CPacket::GetFragmentNumber( void )
{
	if ( m_header.len < 2 )
	{
		// ERROR
		THROW( new CCriticalException( "Cannot extract packet fragment number when packet is less than 2 bytes in length", GET_FILE_NAME(), GET_LINE_NUMBER() ) );
	}

	return (*((uint16_t*)m_pData) & 0xF);
}

uint16_t CPacket::GetSequenceNumber( void )
{
	if ( m_header.len < 2 )
	{
		// ERROR
		THROW( new CCriticalException( "Cannot extract packet sequence number when packet is less than 2 bytes in length", GET_FILE_NAME(), GET_LINE_NUMBER() ) );
	}

	return ((*((uint16_t*)m_pData) >> 4) & 0xFFF);
}
