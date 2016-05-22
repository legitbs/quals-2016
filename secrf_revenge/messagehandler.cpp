#include "common.h"

CMessageHandler::CMessageHandler( )
{

}

CMessageHandler::~CMessageHandler()
{

}

void CMessageHandler::ParseMessage( tTLVField *pTLVTable, uint32_t numElements, uint8_t *pMessageData, uint32_t messageLen )
{
	uint32_t i;
	uint32_t pos = 0;

	tTLVElement *pElements = new tTLVElement[numElements];

	fpTLVHandler fpErrorHandler = NULL;

	// Remember elements that have been populated
	for ( i = 0; i < numElements; i++ )
	{
		if ( pTLVTable[i].tlv_flags & TLV_FLAG_ERROR )
			fpErrorHandler = pTLVTable[i].pHandlerFunc;
		else
		{
			pElements[i].bPopulated = false;
			pElements[i].tlv_flags = pTLVTable[i].tlv_flags;
			pElements[i].tlv_id = pTLVTable[i].tlv_flags;
			pElements[i].pHandlerFunc = pTLVTable[i].pHandlerFunc;
		}
	}

	bool bError = false;

	for ( pos = 0; pos < messageLen; )
	{
		// Begin parsing TLV field
		if ( pos+3 > messageLen )
		{
			bError = true;
			break;
		}

		uint8_t typeID = pMessageData[pos];
		uint16_t tlvDataLen = *((uint16_t *)(pMessageData + 1));

		if ( pos+3+tlvDataLen > messageLen )
		{
			bError = true;
			break;
		}

		// Find the appropriate TLV element
		bool bFound = false;
		for ( i = 0; i < numElements; i++ )
		{
			if ( !(pElements[i].tlv_flags & TLV_FLAG_ERROR) && pElements[i].tlv_id == typeID )
			{
				pElements[i].pData = pMessageData+3;
				pElements[i].dataLen = tlvDataLen;
				pElements[i].bPopulated = true;
				bFound = true;
				break;
			}

		}

		if ( !bFound )
		{
			for ( i = 0; i < numElements; i++ )
			{
				if ( pElements[i].bPopulated == false && pElements[i].tlv_flags & TLV_FLAG_NOTFOUND )
				{
					pElements[i].pData = pMessageData+3;
					pElements[i].dataLen = tlvDataLen;
					pElements[i].bPopulated = true;
					bFound = true;
					break;
				}
			}
		}

		// Unknown TLV field ignore
		// IGNORE

		// Advance to next data element
		pos += tlvDataLen;
	}

	if ( bError )
	{
		if ( fpErrorHandler )
			(*fpErrorHandler)( NULL, pMessageData, messageLen );

		goto exit_cleanup;
	}

	for ( i = 0; i < numElements; i++ )
	{
		if ( pElements[i].bPopulated == false && pElements[i].tlv_flags & TLV_FLAG_REQUIRED )
			goto exit_cleanup;	// Don't process any further

	}

	for ( i = 0; i < numElements; i++ )
	{
		if ( pElements[i].bPopulated )
		{
			bool bContinue = (*pElements[i].pHandlerFunc)( NULL, pElements[i].pData, pElements[i].dataLen );

			if ( !bContinue )
				break;
		}
	}

exit_cleanup:
	delete pElements;
}
