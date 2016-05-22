#ifndef __CIO_CONNECTION_H__
#define __CIO_CONNECTION_H__

class CIOConnection
{
public:

	virtual int32_t ReadLine( uint8_t *pBuffer, uint32_t maxLen ) = 0;
	virtual int32_t WriteLine( uint8_t *pBuffer, uint32_t maxLen ) = 0;

	virtual int32_t RecvUntil( uint8_t *pBuffer, uint32_t count ) = 0;

	virtual bool IsInputAvailable( void ) = 0;

	virtual int32_t ReadAvailableInput( uint8_t *pBuffer, uint32_t maxLen ) = 0;

private:
};

class CIOStdioConnection : public CIOConnection
{
public:

	int32_t ReadLine( uint8_t *pBuffer, uint32_t maxLen );
	int32_t WriteLine( uint8_t *pBuffer, uint32_t maxLen );

	int32_t RecvUntil( uint8_t *pBuffer, uint32_t count );

	bool IsInputAvailable( void );
	
	int32_t ReadAvailableInput( uint8_t *pBuffer, uint32_t maxLen );
};

#endif // __CIO_CONNECTION_H__

