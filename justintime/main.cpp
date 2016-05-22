#include "common.h"

tTaskParameters g_threadTaskList[THREAD_COUNT];
uint32_t g_systemState = SYSTEM_STATE_RUN;
CWorkerLogger g_workerLog;

void sig_alarm_handler( int signum )
{
	printf( "Timeout! Connection closing.\n" );

	exit( 1 );
}


int main( void )
{
#ifdef ENABLE_SIG_ALARM_HANDLER
	signal( SIGALRM, sig_alarm_handler );
	alarm( MAX_IDLE_SECS );
#endif

	// Setup I/O for xinetd service
	setvbuf( stdout, NULL, _IONBF, 0 );

#if 0
	printf( "g_threadTaskList[THREAD_COUNT-1].taskParameters[TASK_PARAMETER_COUNT-1] ADDRESS: %X\n", &(g_threadTaskList[THREAD_COUNT-1].taskParameters[TASK_PARAMETER_COUNT-1]) );
	printf( "g_systemState ADDRESS: %X\n", &(g_systemState) );
#endif

	// Seed random number generator...
	srand( ReadDevURandom32() );

	CIOStdioConnection oIOConnection;

	uint32_t spawnThreadCount = THREAD_COUNT;

	std::thread *pThreadList[THREAD_COUNT];
	CDummyThread *pDummyClassList[THREAD_COUNT];

	CInputThread oInputThread( 0, &oIOConnection );

	pThreadList[0] = new std::thread( &CInputThread::RunThread, &oInputThread );

	for ( uint32_t i = 1; i < spawnThreadCount; i++ )
	{
		g_threadTaskList[i].taskType = TASK_NONE;

		pDummyClassList[i] = new CDummyThread( i );
		pThreadList[i] = new std::thread( &CDummyThread::RunThread, pDummyClassList[i] );
	}

	pThreadList[0]->join();

	return (0);
}
