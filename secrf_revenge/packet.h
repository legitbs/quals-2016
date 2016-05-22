#ifndef __PACKET_H__
#define __PACKET_H__

class CPacket
{
public:
	static const uint8_t PACKET_FRAGMENT_BIT		= (1<<7);
	static const uint8_t PACKET_LAST_FRAGMENT_BIT	= (1<<6);
	static const uint8_t PACKET_PARITY_BIT			= (1<<5);
	static const uint8_t PACKET_PARITY_BIT_POS		= 5;
	static const uint8_t PACKET_HEADER_SIGNATURE	= 0x19;
	static const uint8_t PACKET_HEADER_MASK			= 0x1F;
	static const uint8_t PACKET_PARITY_MASK			= 0xDF;

public:
	CPacket( uint8_t hdrByte, uint8_t len, uint8_t *pData, uint16_t crc16 );
	~CPacket( );

	bool IsHeaderValid( void );
	bool IsCRCValid( void );
	bool IsValidPacket( void );

	uint8_t GetDataLength( void );
	uint8_t *GetData( void );

	uint8_t GetHeader( void ) const { return m_header.hdr; };
	uint16_t GetCRC( void ) const { return m_crc16; };

	bool IsPacketFragmented( void )
	{
		if ( m_header.hdr & PACKET_FRAGMENT_BIT )
			return (true);
		else
			return (false);
	}

	bool IsPacketLastFragment( void )
	{
		if ( m_header.hdr & PACKET_LAST_FRAGMENT_BIT )
			return (true);
		else
			return (false);
	}

	uint8_t GetFragmentNumber( void );
	uint16_t GetSequenceNumber( void );

private:
	typedef struct
	{
		uint8_t hdr;
		uint8_t len;
	} tPacketHeader;

	tPacketHeader m_header;
	uint8_t *m_pData;
	uint16_t m_crc16;
};

#endif // __PACKET_H__