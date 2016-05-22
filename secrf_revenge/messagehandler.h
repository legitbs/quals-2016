#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

typedef bool (*fpTLVHandler)( void *pInstance, uint8_t *pData, uint16_t dataLen );

#define TLV_FLAG_REQUIRED		(1<<0)
#define TLV_FLAG_NOTFOUND		(1<<1)
#define TLV_FLAG_ERROR			(1<<2)

typedef struct TLV_FIELD_HEADER
{
	uint8_t tlv_id;
	uint8_t tlv_flags;
	fpTLVHandler pHandlerFunc;
} tTLVField;

typedef struct TLV_ELEMENT
{
	bool bPopulated;
	uint8_t tlv_flags;
	uint8_t tlv_id;
	fpTLVHandler pHandlerFunc;
	uint8_t *pData;
	uint32_t dataLen;
} tTLVElement;

class CMessageHandler
{
public:
	CMessageHandler();
	~CMessageHandler();

	void ParseMessage( tTLVField *pTLVTable, uint32_t numElements, uint8_t *pMessageData, uint32_t messageLen );

private:
};

#endif // __MESSAGE_HANDLER_H__
