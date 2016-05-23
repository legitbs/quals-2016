#ifndef __RADIO_MESSAGE_H__
#define __RADIO_MESSAGE_H__

#define RADIO_MESSAGE_SIZE	(4096)

class CRadioMessagePoolData : public CDoubleLink
{
public:
	CRadioMessagePoolData( );
	~CRadioMessagePoolData( );

	uint8_t *GetData( void ) const { return m_pData; };

private:
	uint8_t *m_pData;	
};

class CRadioMessagePool
{
public:
	CRadioMessagePool( );
	~CRadioMessagePool( );

	CRadioMessagePoolData *GetMessageData( void );
	
	void FreeMessageData( CRadioMessagePoolData *pItem );

private:
	CDoubleList m_poolList;
	uint32_t m_poolAvailable;  
};

class CRadioMessage : public CDoubleLink
{
public:
	CRadioMessage( uint32_t messageID, uint8_t *pData, uint32_t dataLen );
	~CRadioMessage( );

	uint8_t *GetData( void ) const { return m_pPoolData->GetData(); };
	uint32_t GetLength( void ) const { return m_dataLen; };
	uint32_t GetMessageID( void ) const { return m_messageID; };

private:
	//uint8_t *m_pData;
	uint32_t m_messageID;
	CRadioMessagePoolData *m_pPoolData;
	uint32_t m_dataLen;
};

#endif // __RADIO_MESSAGE_H__
