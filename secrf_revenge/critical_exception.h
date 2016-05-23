#ifndef __CRITICAL_EXCEPTION_H__
#define __CRITICAL_EXCEPTION_H__

#include "string.h"
#include "exception_handler.h"

#define EXCEPTION_TYPE_CRITICAL	(1)

class CCriticalException : public CException
{
public:
	CCriticalException( String sExceptionText, String sFileName, uint32_t lineNumber );
	CCriticalException( const char *pszExceptionText, const char *pszFileName, uint32_t lineNumber );

	const String &GetExceptionText( void ) const { return m_sExceptionText; };
	uint32_t GetLineNumber( void ) const { return m_lineNumber; };
	const String &GetFileName( void ) const { return m_sFileName; };

private:
	String m_sExceptionText;
	String m_sFileName;
	uint32_t m_lineNumber;
};

#endif // __CRITICAL_EXCEPTION_H__
