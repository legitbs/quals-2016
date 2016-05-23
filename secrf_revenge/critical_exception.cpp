#include "common.h"

CCriticalException::CCriticalException( String sExceptionText, String sFileName, uint32_t lineNumber )
	: CException( EXCEPTION_TYPE_CRITICAL ), m_sExceptionText( sExceptionText ), m_sFileName( sFileName ), m_lineNumber( lineNumber )
{

}

CCriticalException::CCriticalException( const char *pszExceptionText, const char *pszFileName, uint32_t lineNumber )
	: CException( EXCEPTION_TYPE_CRITICAL ), m_sExceptionText( pszExceptionText ), m_sFileName( pszFileName ), m_lineNumber( lineNumber )
{

}
