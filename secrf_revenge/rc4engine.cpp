#include "common.h"

CRC4Engine::CRC4Engine( )
{
	memset( m_state, 0, 256 );
}

CRC4Engine::~CRC4Engine( )
{

}

void CRC4Engine::InitKSA( uint8_t *pKeyData, uint8_t keyLen, uint32_t dropAmount )
{
	uint32_t i,j=0,t; 
	   
	for (i=0; i < 256; ++i)
		m_state[i] = i; 
	
	for (i=0; i < 256; ++i) 
	{
		j = (j + m_state[i] + pKeyData[i % keyLen]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
	}   

	uint32_t x;
	for (x=0; x < dropAmount; ++x)  
	{
		i = (i + 1) % 256; 
		j = (j + m_state[i]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
	}
}

void CRC4Engine::RC4Block( uint8_t *pBlock, uint8_t blockLen )
{
	uint32_t i=0,j=0,x,t; 
   
	for (x=0; x < blockLen; ++x)  
	{
		i = (i + 1) % 256; 
		j = (j + m_state[i]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
		pBlock[x] = m_state[(m_state[i] + m_state[j]) % 256] ^ pBlock[x];
	}   
}