#ifndef __RC4_ENGINE_H__
#define __RC4_ENGINE_H__

class CRC4Engine
{
public:
	CRC4Engine( );
	~CRC4Engine( );

	void InitKSA( uint8_t *pKeyData, uint8_t keyLen, uint32_t dropAmount );
	void RC4Block( uint8_t *pBlock, uint8_t blockLen );

private:
	uint32_t	m_state[256];
};

#endif // __RC4_ENGINE_H__