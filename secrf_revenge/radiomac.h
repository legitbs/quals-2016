#ifndef __RADIO_MAC_H__
#define __RADIO_MAC_H__

class CFragmentBuffer : public CDoubleLink
{
public:
	CFragmentBuffer( uint16_t sequenceNumber );
	~CFragmentBuffer();

	uint32_t GetSequenceNumber( void ) { return m_sequenceNumber; };

	bool AddFragment( CPacket *pPacket );

	bool HasAllFragments( void );

	uint32_t GetAssembledSize( void );

	uint32_t AssemblePacket( uint8_t *pBuffer, uint32_t bufferLen );

private:
	CPacket *m_pSegments[MAX_PACKET_FRAGMENTS];
	uint16_t m_sequenceNumber;
	uint8_t m_lastFragmentNumber;
	bool m_bHasLastFragment;
};

class CRadioMAC
{
public:
	CRadioMAC();
	~CRadioMAC();

	bool LoadKeyFile( const char *pszKeyFileName );

	void ReceivePacket( CPacket *pPacket );

	bool IsMessageAvailable( void );

	CRadioMessage *PopMessage( void );

	void DecryptDataToMessage( uint8_t *pData, uint32_t dataLen );

	void AddFragment( CPacket *pPacket );

	CFragmentBuffer *GetFragmentBufferForPacket( CPacket *pPacket );

	void AssembleFragmentToMessage( CFragmentBuffer *pFragmentBuffer );

private:
	CDoubleList m_messageList;
	CDoubleList m_fragmentList;

	uint8_t m_keyData[CRYPTO_KEY_SIZE];
	uint32_t m_messageCounter;
};

#endif // __RADIO_MAC_H__
