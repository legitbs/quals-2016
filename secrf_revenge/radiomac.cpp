#include "common.h"

#include <unistd.h>

CFragmentBuffer::CFragmentBuffer( uint16_t sequenceNumber )
	: m_sequenceNumber( sequenceNumber ), m_lastFragmentNumber( 0 ), m_bHasLastFragment( false )
{
	for ( uint32_t i = 0; i < MAX_PACKET_FRAGMENTS; i++ )
		m_pSegments[i] = NULL;
}

CFragmentBuffer::~CFragmentBuffer()
{
	for ( uint32_t i = 0; i < MAX_PACKET_FRAGMENTS; i++ )
	{
		if ( m_pSegments[i] )
			delete m_pSegments[i];
	}
}

bool CFragmentBuffer::AddFragment( CPacket *pPacket )
{
	if ( !pPacket )
		return (false);

	// Set the packet segment
	m_pSegments[pPacket->GetFragmentNumber()] = pPacket;

	// Record last fragment number
	if ( pPacket->IsPacketLastFragment() )
	{
		m_bHasLastFragment = true;
		m_lastFragmentNumber = pPacket->GetFragmentNumber();
	}

	// Fragment added
	return (true);
}

bool CFragmentBuffer::HasAllFragments( void )
{
	uint32_t count = 0;

	if ( !m_bHasLastFragment )
		return (false);

	for ( uint8_t i = 0; i < (m_lastFragmentNumber+1); i++ )
	{
		if ( m_pSegments[i] )
			count++;
	}

	if ( count == (m_lastFragmentNumber+1) )
		return (true);
	else
		return (false);
}

uint32_t CFragmentBuffer::GetAssembledSize( void )
{
	uint32_t size = 0;

	for ( uint8_t i = 0; i < (m_lastFragmentNumber+1); i++ )
	{
		if ( m_pSegments[i] )
			size += m_pSegments[i]->GetDataLength();
	}

	return (size);
}

uint32_t CFragmentBuffer::AssemblePacket( uint8_t *pBuffer, uint32_t bufferLen )
{
	uint32_t pos = 0;

	// BUG:: We will assemble packets out of RANGE!
	for ( uint32_t i = 0; i < MAX_PACKET_FRAGMENTS; i++ )
	{
		if ( m_pSegments[i] )
		{
			memcpy( pBuffer+pos, m_pSegments[i]->GetData(), m_pSegments[i]->GetDataLength() );

			pos += m_pSegments[i]->GetDataLength();
		}

		// BUG:: This check should be performed in ADVANCE
		if ( pos > bufferLen )
			break;
	}

	return (pos);
}

CRadioMAC::CRadioMAC()
	: m_messageCounter( 0 )
{

}

CRadioMAC::~CRadioMAC()
{
	m_messageList.DeleteAll();
}

bool CRadioMAC::LoadKeyFile( const char *pszKeyFileName )
{
	int fd;

	fd = open( pszKeyFileName, O_RDONLY );
	if ( fd == -1 )
		THROW( new CCriticalException( "Failed to load crypto key from file", GET_FILE_NAME(), GET_LINE_NUMBER() ) );

	// Read in key file data
	if ( read( fd, m_keyData, CRYPTO_KEY_SIZE ) != CRYPTO_KEY_SIZE )
		THROW( new CCriticalException( "Failed to load crypto key from file", GET_FILE_NAME(), GET_LINE_NUMBER() ) );

	close( fd );

	return (true);
}

void CRadioMAC::DecryptDataToMessage( uint8_t *pData, uint32_t dataLen )
{
	if ( pData == NULL || dataLen == 0 )
		return;

	// Create temporary data for packet
	uint8_t *pTempData = new uint8_t[dataLen];

	// Check for memory allocation failure -- give them a little HINT
	if ( pTempData == NULL )
		THROW( new CCriticalException( "Cannot allocate memory for temporary data during decryption", GET_FILE_NAME(), GET_LINE_NUMBER() ) );

	memcpy( pTempData, pData, dataLen );

	// Now decrypt
	CRC4Engine oCryptoEngine;

	// Initialize key
	oCryptoEngine.InitKSA( m_keyData, CRYPTO_KEY_SIZE, CRYPTO_RC4_DROP_AMOUNT );

	// Decrypt
	oCryptoEngine.RC4Block( pTempData, dataLen );

	// Create new message with decrypted data...
	m_messageCounter++;
	CRadioMessage *pNewMessage = new CRadioMessage( m_messageCounter, pTempData, dataLen );

	// Add to received messages
	m_messageList.AddLast( pNewMessage );

	// Cleanup memory
	if ( pTempData )
		delete pTempData;
}

void CRadioMAC::ReceivePacket( CPacket *pPacket )
{
	if ( pPacket->IsPacketFragmented() )
	{
		// Handle fragmented packets
		AddFragment( pPacket );
	}
	else
	{
		// Decrypt and receive message
		DecryptDataToMessage( pPacket->GetData(), pPacket->GetDataLength() );

		// Done processing packet -- delete it
		delete pPacket;
	}
}

bool CRadioMAC::IsMessageAvailable( void )
{
	if ( m_messageList.GetFirst() )
		return (true);
	else
		return (false);
}

CRadioMessage *CRadioMAC::PopMessage( void )
{
	return ((CRadioMessage *)m_messageList.RemoveFirst());
}

void CRadioMAC::AddFragment( CPacket *pPacket )
{
	if ( !pPacket )
		return;

	// Are there already packet fragments in the buffer???
	CFragmentBuffer *pFragmentBuffer = GetFragmentBufferForPacket( pPacket );

	if ( pFragmentBuffer == NULL )
	{
		// New packet not in buffer!

		// Is the buffer too full? Drop the oldest sequence number still being reassembled
		if ( m_fragmentList.GetItemCount() >= MAX_FRAGMENT_BUFFER )
			delete m_fragmentList.RemoveLast();

		// Now add a new one in!
		CFragmentBuffer *pNewFragmentBuffer = new CFragmentBuffer( pPacket->GetSequenceNumber() );

		if ( pNewFragmentBuffer == NULL )
			THROW( new CCriticalException( "Out of memory during fragmented packet reassembly", GET_FILE_NAME(), GET_LINE_NUMBER() ) );

		pNewFragmentBuffer->AddFragment( pPacket );

		if ( pNewFragmentBuffer->HasAllFragments() )
		{
			AssembleFragmentToMessage( pNewFragmentBuffer );
		
			// Delete fragment buffer -- it is assembled
			delete pNewFragmentBuffer;
		}
		else
		{
			// Add to fragment list
			m_fragmentList.AddFirst( pNewFragmentBuffer );	
		}
	}
	else
	{
		// Add fragment
		pFragmentBuffer->AddFragment( pPacket );

		// Check to see if all fragments have been received
		if ( pFragmentBuffer->HasAllFragments() )
		{
			AssembleFragmentToMessage( pFragmentBuffer );
	
			// Delete fragment buffer -- it is assembled
			delete pFragmentBuffer;
		}
	}
}

CFragmentBuffer *CRadioMAC::GetFragmentBufferForPacket( CPacket *pPacket )
{
	CFragmentBuffer *pCur;

	if ( !pPacket || !pPacket->IsPacketFragmented() )
		return (NULL);

	for ( pCur = (CFragmentBuffer *)m_fragmentList.GetFirst(); pCur; pCur = (CFragmentBuffer *)m_fragmentList.GetNext( pCur ) )
	{
		if ( pCur->GetSequenceNumber() == pPacket->GetSequenceNumber() )
			return pCur;
	}

	return (NULL);
}

void CRadioMAC::AssembleFragmentToMessage( CFragmentBuffer *pFragmentBuffer )
{
	if ( !pFragmentBuffer || !pFragmentBuffer->HasAllFragments() )
		return;

	// Assemble
	uint32_t assembledLength = pFragmentBuffer->GetAssembledSize();

	uint8_t *pTempData = new uint8_t[assembledLength];

	pFragmentBuffer->AssemblePacket( pTempData, assembledLength );

	CRC4Engine oCryptoEngine;

	// Initialize key
	oCryptoEngine.InitKSA( m_keyData, CRYPTO_KEY_SIZE, CRYPTO_RC4_DROP_AMOUNT );

	// Decrypt
	oCryptoEngine.RC4Block( pTempData, assembledLength );

	// Create new message with decrypted data...
	m_messageCounter++;
	CRadioMessage *pNewMessage = new CRadioMessage( m_messageCounter, pTempData, assembledLength );

	// Add to received messages
	m_messageList.AddLast( pNewMessage );

	// Cleanup memory
	if ( pTempData )
		delete pTempData;
}
