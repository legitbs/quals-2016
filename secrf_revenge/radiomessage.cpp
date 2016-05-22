#include "common.h"

#define RADIO_MESSAGE_POOL_INIT_SIZE	16

void *__dso_handle = 0;

CRadioMessagePool g_messagePool;

CRadioMessagePoolData::CRadioMessagePoolData( )
{
	m_pData = new uint8_t[RADIO_MESSAGE_SIZE];
}

CRadioMessagePoolData::~CRadioMessagePoolData( )
{
	if ( m_pData )
		delete m_pData;
}

CRadioMessagePool::CRadioMessagePool( )
{
	uint32_t i;
	for ( i = 0; i < RADIO_MESSAGE_POOL_INIT_SIZE; i++ )
	{
		m_poolList.AddLast( new CRadioMessagePoolData() );
	}

	m_poolAvailable = i;
}

CRadioMessagePool::~CRadioMessagePool( )
{
	// Do nothing -- pool deletes all items
}

CRadioMessagePoolData *CRadioMessagePool::GetMessageData( void )
{
	if ( m_poolAvailable == 0 )
	{
		uint32_t i;
		for ( i = 0; i < RADIO_MESSAGE_POOL_INIT_SIZE; i++ )
		{
			m_poolList.AddLast( new CRadioMessagePoolData() );
		}

		m_poolAvailable = i;
	}

	CRadioMessagePoolData *pNewPoolData = (CRadioMessagePoolData *)m_poolList.RemoveFirst();

	m_poolAvailable--;

	return pNewPoolData;
}

void CRadioMessagePool::FreeMessageData( CRadioMessagePoolData *pItem )
{
	if ( !pItem )
		return;

	m_poolList.AddLast( pItem );

	m_poolAvailable++;
}

CRadioMessage::CRadioMessage( uint32_t messageID, uint8_t *pData, uint32_t dataLen )
	: m_messageID( messageID ), CDoubleLink()
{
	m_pPoolData = g_messagePool.GetMessageData();

	memcpy( m_pPoolData->GetData(), pData, dataLen );

	m_dataLen = dataLen;

	/*
	m_pData = new uint8_t[dataLen];

	memcpy( m_pData, pData, dataLen );

	m_dataLen = dataLen;
	*/
}

CRadioMessage::~CRadioMessage( )
{
	g_messagePool.FreeMessageData( m_pPoolData );

	/*
	if ( m_pData )
		delete m_pData;
	*/
}
