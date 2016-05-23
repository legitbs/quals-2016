#ifndef __DIGITAL_RECEIVER_H__
#define __DIGITAL_RECEIVER_H__

class CDigitalReceiver
{
public:
	static const uint32_t RX_BIT_RATE		= 330;
	static const uint32_t RX_BIT_RATE_SKEW	= 50;		// Allow +/- 50 bps skew

	static const uint8_t RX_STATE_PREAMBLE	= 0;
	static const uint8_t RX_STATE_BYTE_SYNC = 1;
	static const uint8_t RX_STATE_HEADER	= 2;
	static const uint8_t RX_STATE_LENGTH	= 3;
	static const uint8_t RX_STATE_DATA		= 4;
	static const uint8_t RX_STATE_CRC		= 5;

	static const uint8_t RX_MARK_VALUE		= 0;
	static const uint8_t RX_SPACE_VALUE		= 1;

	static const uint8_t PREAMBLE_LOCK_BIT_COUNT	= 24;

	static const uint16_t BYTE_SYNC_VALUE	= 0xC3AC;

public:
	CDigitalReceiver( CRadioMAC *pRadioMAC );
	~CDigitalReceiver();

	void SetSampleRate( uint32_t sampleRate ) { m_sampleRate = sampleRate; };

	void ReceiveSample( uint8_t sample );

private:
	void ResetLock( void );
	void DoRxPreamble( uint8_t sample );
	void DoRxByteSync( uint8_t sample );
	void DoRxHeader( uint8_t sample );
	void DoRxLength( uint8_t sample );
	void DoRxData( uint8_t sample );
	void DoRxCRC( uint8_t sample );

	bool DoSample( void );

	uint32_t GetSampleClockSkewMax( void );
	uint32_t GetSampleClockSkewMin( void );

private:
	CRadioMAC *m_pRadioMAC;

	double m_clocksPerSymbol;
	uint32_t m_clockTransitionCounter;
	uint32_t m_preambleCounter;
	uint8_t m_lastTransitionSymbol;

	double m_clocksUntilNextSymbol;
	uint32_t m_symbolsSinceLastTransition;
	uint8_t m_lastPreambleSymbol;

	uint8_t m_byteLockPos;

	uint32_t m_preambleSymbolCounter;

	uint8_t m_rxState;
	uint32_t m_sampleRate;

	uint8_t m_packetBitPos;
	uint8_t m_packetDataBytePos;

	uint8_t m_currentPacketHdr;
	uint8_t m_currentPacketLen;
	uint8_t m_currentPacketData[256];
	uint16_t m_currentPacketCRC;
};

#endif // __DIGITAL_RECEIVER_H__
