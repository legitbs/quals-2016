#include "common.h"

CDigitalReceiver::CDigitalReceiver( CRadioMAC *pRadioMAC )
	: m_clockTransitionCounter( 0 ), m_preambleCounter( 0 ), m_rxState( RX_STATE_PREAMBLE ), m_sampleRate( SAMPLE_FS ),
	m_lastTransitionSymbol( RX_MARK_VALUE ), m_lastPreambleSymbol( RX_MARK_VALUE ), m_clocksPerSymbol( SAMPLE_FS / RX_BIT_RATE ), m_clocksUntilNextSymbol( 0 ), m_byteLockPos( 0 ),
	m_pRadioMAC( pRadioMAC ), m_symbolsSinceLastTransition( 0 )
{

}

CDigitalReceiver::~CDigitalReceiver()
{

}

void CDigitalReceiver::ResetLock( void )
{
	 m_rxState = RX_STATE_PREAMBLE;
	 m_clocksPerSymbol = SAMPLE_FS / RX_BIT_RATE;
	 m_lastTransitionSymbol = RX_MARK_VALUE;
	 m_preambleCounter = 0;
	 m_clockTransitionCounter = 0;
	 m_symbolsSinceLastTransition = 0;
}

void CDigitalReceiver::ReceiveSample( uint8_t sample )
{
	if ( m_rxState > RX_STATE_PREAMBLE )
	{
		// Keep clock trained!
		// Relatively simple algorithm here -- track the transition points (cross overs) and use that information to adjust the clocks per symbol amount --
		// if we go out of range -- drop lock
		if ( sample != m_lastTransitionSymbol )
		{
			// Adjust clocks per symbol
			double result = (double)(m_clockTransitionCounter+1) / (m_clocksPerSymbol * (double)m_symbolsSinceLastTransition);

			// Try to track it -- slowly
			if ( result < 1.0 )
				m_clocksPerSymbol -= 0.1;
			else if ( result > 1.0 )
				m_clocksPerSymbol += 0.1;

			if ( m_clocksPerSymbol < GetSampleClockSkewMin() || m_clocksPerSymbol > GetSampleClockSkewMax() )
			{
				ResetLock();
				return;
			}

			// Reset clocks until next symbol
			m_clocksUntilNextSymbol = (m_clocksPerSymbol / 2.0);

			// Reset symbols since last transition counter
			m_symbolsSinceLastTransition = 0;

			// Record valid transition
			m_lastTransitionSymbol = sample;
			m_clockTransitionCounter = 0; 
		}
		else
			m_clockTransitionCounter++;
	}

	switch( m_rxState )
	{
	case RX_STATE_PREAMBLE:
		DoRxPreamble( sample );
		break;

	case RX_STATE_BYTE_SYNC:
		DoRxByteSync( sample );
		break;

	case RX_STATE_HEADER:
		DoRxHeader( sample );
		break;

	case RX_STATE_LENGTH:
		DoRxLength( sample );
		break;

	case RX_STATE_DATA:
		DoRxData( sample );
		break;

	case RX_STATE_CRC:
		DoRxCRC( sample );
		break;
	}
}

void CDigitalReceiver::DoRxPreamble( uint8_t sample )
{
	// Run the clock recovery training and start preamble detection -- in this case we use a 3-byte preamble
	m_clockTransitionCounter++;

	if ( m_lastTransitionSymbol != sample )
	{
		// Current sample differs -- transition???
		// Check clock counter is in range
		if ( m_clockTransitionCounter >= GetSampleClockSkewMin() && m_clockTransitionCounter <= GetSampleClockSkewMax() )
		{
			m_preambleCounter++;

			if ( m_preambleCounter == PREAMBLE_LOCK_BIT_COUNT )
			{
				m_preambleCounter = 0;
				m_rxState = RX_STATE_BYTE_SYNC;

				// Start clock counter
				m_clocksUntilNextSymbol = m_clocksPerSymbol / 2.0;

				// Reset symbols since last transition counter
				m_symbolsSinceLastTransition = 0;

				// Start byte search...
				m_byteLockPos = 0;

				// DEBUG:
				m_preambleSymbolCounter = PREAMBLE_LOCK_BIT_COUNT;
			}

			// Adjust clocks per symbol -- rounding and tracking
			uint32_t samplesPerClock = floor( m_clocksPerSymbol + 0.5 );

			if ( samplesPerClock < m_clockTransitionCounter )
				m_clocksPerSymbol += 0.1;
			else if ( samplesPerClock > m_clockTransitionCounter )
				m_clocksPerSymbol -= 0.1;
		}
		else
			m_preambleCounter = 0;

		m_clockTransitionCounter = 0;
		m_lastTransitionSymbol = sample;
	}
}

void CDigitalReceiver::DoRxByteSync( uint8_t sample )
{
	if ( DoSample() )
	{
		 if ( m_byteLockPos == 0 )
		 {
			 // Track preamble...
			 if ( sample != m_lastPreambleSymbol )
			 {
				 // Stay tracking preamble
				 m_lastPreambleSymbol = sample;

				 // Update preamble counter
				 m_preambleSymbolCounter++;
			 }
			 else
			 {
				 // Possible byte lock starting...
				 if ( m_lastPreambleSymbol == 1 )
				 {
					 // Last was 1... we are now receiving a 1
					 m_byteLockPos = 2;
				 }
				 else
				 {
					 // Last was a zero and this symbol is now a zero -- we are not in a correct sequence!
					 ResetLock();
					 return;
				 }
			 }
		 }
		 else
		 {
			 if ( m_lastPreambleSymbol == 1 && m_byteLockPos == 2 && sample == 1 )
			 {
				 m_lastPreambleSymbol = 0; // Prevent infinite loop
				 // Ignore
				 // Received -- 3 1's in a row -- wait one more bit (could have been preamble sequence 1010101 11
			 }
			 else
			 {
				 if ( sample != ((BYTE_SYNC_VALUE >> (15-m_byteLockPos)) & 0x1) )
				 {
					 ResetLock();
					 return;
				 }

				 m_byteLockPos++;

				 // Begin parsing header!
				 if ( m_byteLockPos == 16 )
				 {
					 m_currentPacketHdr = 0;
					 m_packetBitPos = 0;
					 m_rxState = RX_STATE_HEADER;
				 }
			 }
		 }
	}
}

void CDigitalReceiver::DoRxHeader( uint8_t sample )
{
	if ( DoSample() )
	{
		 m_currentPacketHdr |= ((sample & 0x1) << (7-m_packetBitPos));

		 m_packetBitPos++;

		 if ( m_packetBitPos == 8 )
		 {
			 m_currentPacketLen = 0;
			 m_packetBitPos = 0;
			 m_rxState = RX_STATE_LENGTH;
		 }
	}
}

void CDigitalReceiver::DoRxLength( uint8_t sample )
{
	if ( DoSample() )
	{
		 m_currentPacketLen |= ((sample & 0x1) << (7-m_packetBitPos));

		 m_packetBitPos++;

		 if ( m_packetBitPos == 8 )
		 {
			 if ( m_currentPacketLen == 0 )
			 {
				 // Don't allow 0 length packet data
				 ResetLock();
				 return;
			 }

			 // Reset packet data
			 memset( m_currentPacketData, 0, 256 );

			 m_packetDataBytePos = 0;
			 m_packetBitPos = 0;
			 m_rxState = RX_STATE_DATA;
		 }
	}
}

void CDigitalReceiver::DoRxData( uint8_t sample )
{
	if ( DoSample() )
	{
		 m_currentPacketData[m_packetDataBytePos] |= ((sample & 0x1) << (7-m_packetBitPos));

		 m_packetBitPos++;

		 if ( m_packetBitPos == 8 )
		 {
			 m_packetBitPos = 0;

			 m_packetDataBytePos++;

			 if ( m_packetDataBytePos == m_currentPacketLen )
			 {
				 m_packetBitPos = 0;
				 m_currentPacketCRC = 0;
				 m_rxState = RX_STATE_CRC;
			 }
		 }
	}
}

void CDigitalReceiver::DoRxCRC( uint8_t sample )
{
	if ( DoSample() )
	{
		 m_currentPacketCRC |= ((sample & 0x1) << (15-m_packetBitPos));

		 m_packetBitPos++;

		 if ( m_packetBitPos == 16 )
		 {
			 // RX Complete
			 ResetLock();

			 CPacket *pNewPacket = new CPacket( m_currentPacketHdr, m_currentPacketLen, m_currentPacketData, m_currentPacketCRC );

			 // Check if packet is valid
			 if ( pNewPacket->IsValidPacket() )
			 {
				 // Add to packet receive list
				 m_pRadioMAC->ReceivePacket( pNewPacket );

				 LogMessage( DEBUG_MESSAGE_INFO, "Packet received" );
			 }
			 else
			 {
				 // Use a debug trace
				 LogMessage( DEBUG_MESSAGE_INFO, "Discard invalid packet" );

				 delete pNewPacket;
			 }
		 }
	}
}

uint32_t CDigitalReceiver::GetSampleClockSkewMax( void )
{
	double skewMax = (double)m_sampleRate / (double)(RX_BIT_RATE-RX_BIT_RATE_SKEW);

	return (uint32_t)(round( skewMax ));
}

uint32_t CDigitalReceiver::GetSampleClockSkewMin( void )
{
	double skewMin = (double)m_sampleRate / (double)(RX_BIT_RATE+RX_BIT_RATE_SKEW);

	return (uint32_t)(round( skewMin ));
}

bool CDigitalReceiver::DoSample( void )
{
	if ( m_clockTransitionCounter == (uint32_t)round( m_clocksUntilNextSymbol ) )
	{
		// Update clocks til next symbol
		// UPDATED (old below)	
		m_clocksUntilNextSymbol = m_clockTransitionCounter + m_clocksPerSymbol;
		//m_clocksUntilNextSymbol += m_clocksPerSymbol;
		
		// Update symbol counter since last transition
		m_symbolsSinceLastTransition++;

		return (true);
	}
	else
		return (false);
}
