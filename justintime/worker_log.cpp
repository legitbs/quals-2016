#include "common.h"

CWorkerLogger::CWorkerLogger( )
	: m_bEnabled( false ), m_pLogFP( NULL )
{
	
}

CWorkerLogger::~CWorkerLogger( )
{
	if ( m_pLogFP )
	{
		fclose( m_pLogFP );
		m_pLogFP = false;
	}

	m_bEnabled = false;
}

void CWorkerLogger::Disable( void )
{
	if ( m_pLogFP )
	{	
		m_oLogFileMutex.lock();

		fclose( m_pLogFP );
		m_pLogFP = NULL;
	
		m_oLogFileMutex.unlock();
	}
	
	m_bEnabled = false;	
}

void CWorkerLogger::Enable( void )
{
	if ( m_bEnabled )
		return;

	if ( m_pLogFP )
		Disable();

	m_pLogFP = fopen( LOG_FILE_NAME, "w+" );

	if ( !m_pLogFP )
		return;

	time_t build_time = BUILD_TIMESTAMP;
	struct tm lt;
	char res[64];

	localtime_r(&build_time, &lt);

	strftime( res, 64, "%a %b %d %Y", &lt );

	fprintf( m_pLogFP, "LOG STARTED\nTool version %d.%d.%d, build timestamp=%s\n", 1, 0, 0, res );
	fflush( m_pLogFP );

	m_bEnabled = true;	
}

void CWorkerLogger::WriteLogData( uint8_t workerID, const char *pszFormat, ... )
{
	if ( !m_bEnabled )
		return;

	m_oLogFileMutex.lock();
	if ( !m_pLogFP )
	{
		m_oLogFileMutex.unlock();
		return;
	}

	va_list args;
	va_start( args, pszFormat );

	int32_t chars_written;

	char szOutput[512];

	sprintf( szOutput, "WORKER[%d]::%s\n", workerID, pszFormat );

	chars_written = vfprintf( m_pLogFP, szOutput, args );

	fflush( m_pLogFP );
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

	va_end( args );

	m_oLogFileMutex.unlock();
}
