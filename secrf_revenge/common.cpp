#include "common.h"
#include <stdarg.h>

uint8_t g_logLevel = 0;

#define MAX_LOG_LEVELS	(2)
const char *g_szLogLevelText[MAX_LOG_LEVELS] = {
	"ERROR",
	"INFO"
};

void SetLogLevel( uint8_t logLevel )
{
	g_logLevel = logLevel;	
}

void LogMessage( uint8_t messageType, const char *fmt, ... )
{
	if ( messageType > g_logLevel )
		return;

	va_list args;
	va_start(args, fmt);

	char szBuffer[1024];

	vsprintf( szBuffer, fmt, args );

	va_end(args);

	if ( messageType >= MAX_LOG_LEVELS )
		printf( "[DBG][%d]::%s\n", messageType, szBuffer );
	else
		printf( "[DBG][%s]::%s\n", g_szLogLevelText[messageType], szBuffer );
}
