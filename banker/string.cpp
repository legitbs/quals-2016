#include "string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

String::String( )
{
	m_pData = new char[1];
	m_pData[0] = '\0';

	m_size = 0;
}

String::String( const String &rhs )
{
	m_pData = new char[rhs.m_size+1];
	
	memcpy( m_pData, rhs.m_pData, rhs.m_size );
	m_pData[rhs.m_size] = '\0';

	m_size = rhs.m_size;
}

String::String( const char *rhs )
{
	m_size = strlen(rhs);

	m_pData = new char[m_size+1];
	
	memcpy( m_pData, rhs, m_size );
	m_pData[m_size] = '\0';
}

String::~String( )
{
	if ( m_pData )
		delete m_pData;

	m_pData = NULL;
	m_size = 0;
}

uint32_t String::GetLength( void ) const
{
	return (m_size);
}

String String::SubString( uint32_t start, uint32_t length )
{
	String sTemp;

	if ( start >= m_size )
		return sTemp;

	if ( start+length > m_size )
		length = (m_size-start);

	sTemp.m_size = length;
	delete sTemp.m_pData;
	
	sTemp.m_pData = new char[length+1];
	
	memcpy( sTemp.m_pData, m_pData+start, length );
	sTemp.m_pData[length] = '\0';

	return sTemp;
}

void String::Reverse( void )
{
	if ( m_size < 2 )
		return;

	char *pszTemp = new char[m_size+1];

	uint32_t t = (m_size-1);
	for ( uint32_t i = 0; i < m_size; i++ )
	{
		pszTemp[i] = m_pData[t];
		t--;
	}

	pszTemp[m_size] = '\0';

	delete m_pData;
	m_pData = pszTemp;
}

bool String::IsEmpty( void )
{
	if ( m_size == 0 )
		return (true);
	else
		return (false);
}

char String::GetAt( uint32_t pos )
{
	if ( pos < m_size )
		return m_pData[pos];
	else
		return '\0';
}

void String::ToLower( void )
{
	for ( uint32_t i = 0; i < m_size; i++ )
	{
		if ( m_pData[i] >= 'A' && m_pData[i] <= 'Z' )
			m_pData[i] = (m_pData[i]-'A') + 'a';
	}
}

void String::ToUpper( void )
{
	for ( uint32_t i = 0; i < m_size; i++ )
	{
		if ( m_pData[i] >= 'a' && m_pData[i] <= 'z' )
			m_pData[i] = (m_pData[i]-'a') + 'A';
	}
}

String &String::operator=( const String &rhs )
{
	// Copy assignment
	if ( m_pData )
		delete m_pData;

	m_pData = new char[rhs.m_size+1];
	
	memcpy( m_pData, rhs.m_pData, rhs.m_size );
	m_pData[rhs.m_size] = '\0';

	m_size = rhs.m_size;

	return (*this);
}

String String::operator+( String &rhs )
{
	char *pNewData = new char[m_size+rhs.m_size+1];

	memcpy( pNewData, m_pData, m_size );
	memcpy( pNewData+m_size, rhs.m_pData, rhs.m_size );

	pNewData[m_size+rhs.m_size] = '\0';

	String sTemp(pNewData);
	delete pNewData;

	return sTemp;
}

String &String::operator+=( String &rhs )
{
	char *pNewData = new char[m_size+rhs.m_size+1];

	memcpy( pNewData, m_pData, m_size );
	memcpy( pNewData+m_size, rhs.m_pData, rhs.m_size );
	
	pNewData[m_size+rhs.m_size] = '\0';

	delete m_pData;
	m_pData = pNewData;

	m_size += rhs.m_size;

	return *this;
}

bool String::operator==( String &rhs ) const
{
	return (Compare(rhs) == 0);
}

bool String::operator==( const String &rhs ) const
{
	return (Compare(rhs) == 0);
}

int32_t String::Compare( const String &rhs ) const
{
	if ( m_size < rhs.m_size )
		return -1;
	
	if ( m_size > rhs.m_size )
		return 1;

	for ( uint32_t i = 0; i < m_size; i++ )
	{
		if ( m_pData[i] < rhs.m_pData[i] )
			return -1;

		if ( m_pData[i] > rhs.m_pData[i] )
			return 1;
	}

	return (0);
}

const char *String::CStr( void ) const
{
	return (const char*)m_pData;
}

int32_t String::ToInt( void ) const
{
	if ( !m_pData )
		return (0);

	return atoi( m_pData );
}
