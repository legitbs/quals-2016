#include "common.h"

extern uint32_t g_systemState;
extern tTaskParameters g_threadTaskList[THREAD_COUNT];
extern CWorkerLogger g_workerLog;

bool (*g_fpSystemHandler[SYSTEM_STATE_MAX])( uint32_t *, uint8_t, uint8_t, CDummyThread * ) = {
	&HandleStateRun,
	&HandleStateSleep,
	&HandleStateTerminate
};

struct cntr_desc {
	uint32_t type;
	uint32_t config;
	const char *name;
	unsigned long long val;
} cntr_desc[] = {
	{ PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_CLOCK,
	  "sw-cpu-clock", 0 },
	{ PERF_TYPE_SOFTWARE, PERF_COUNT_SW_TASK_CLOCK,
	  "sw-task-clock", 0 },
};

struct cntr {
    pid_t pid;
    struct cntr_desc *desc;
    int fd;
};


uint32_t EuclidGCD( uint32_t a, uint32_t b, uint64_t &iterCount )
{
	if ( a == 0 || b == 0 )
		return (0);

	if ( b > a )
	{
		uint32_t temp = a;
		a = b;
		b = temp;
	}

	iterCount = 0;
	while ( b > 0 )
	{
		uint32_t t = (a % b);
		a = b;
		b = t;

		iterCount++;
	}

	return (a);
}

void DoLCDTask( uint32_t *pTaskArgs, uint8_t numberCount, CDummyThread *pThread )
{
	if ( numberCount == 0 )
	{
		g_workerLog.WriteLogData( pThread->GetThreadID(), "LCD TASK no parameters" );
		printf( "\nWorker received LCD task without parameters, task aborted\n" );
	}
	else if ( numberCount == 1 )
	{
		g_workerLog.WriteLogData( pThread->GetThreadID(), "LCD TASK, LCD is: %u", pTaskArgs[0] );
		printf( "\nWorker %d, LCD is: %u\n", pThread->GetThreadID(), pTaskArgs[0] );
	}
	else
	{
		// First do GCD
		uint64_t a, b, c, d;
		uint64_t iterCount;
		uint64_t totalIterCount = 0;
		a = pTaskArgs[0];
		b = pTaskArgs[1];
		c = pTaskArgs[2];
		d = pTaskArgs[3];

		for ( uint32_t i = 1; i < numberCount; i++ )
		{
			uint64_t gcd_ab = EuclidGCD( a, b, iterCount );

			totalIterCount += iterCount;

			if ( gcd_ab == 0 )
			{
				a = 0;
				break;
			}
	
			uint64_t lcm_numerator = ((uint64_t)a) * ((uint64_t)b);
			
			a = (lcm_numerator / gcd_ab);

			if ( (i+1) < numberCount )
				b = pTaskArgs[i+1];	
		
			std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
		}

		g_workerLog.WriteLogData( pThread->GetThreadID(), "LCM TASK, LCD is: %llu (iterCount=%llu)", a, totalIterCount );
		printf( "\nWorker %d, LCD is: %llu\n", pThread->GetThreadID(), a );
	}	
}

void DoGCDTask( uint32_t *pTaskArgs, uint8_t numberCount, CDummyThread *pThread )
{
	if ( numberCount == 0 )
	{
		g_workerLog.WriteLogData( pThread->GetThreadID(), "GCD TASK no parameters" );
		printf( "\nWorker received GCD task without parameters, task aborted\n" );
	}
	else if ( numberCount == 1 )
	{
		g_workerLog.WriteLogData( pThread->GetThreadID(), "GCD TASK: GCD is %u", pTaskArgs[0] );
		printf( "\nWorker %d, GCD is: %u\n", pThread->GetThreadID(), pTaskArgs[0] );
	}
	else
	{
		uint32_t a, b;
		uint64_t iterCount;
		uint64_t totalIterCount = 0;
		a = pTaskArgs[0];
		b = pTaskArgs[1];
		for ( uint32_t i = 1; i < numberCount; i++ )
		{
			uint32_t gcd_ab = EuclidGCD( a, b, iterCount );

			totalIterCount += iterCount;

			a = gcd_ab;
			if ( (i+1) < numberCount )
				b = pTaskArgs[i+1];	
		
			std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
		}

		g_workerLog.WriteLogData( pThread->GetThreadID(), "GCD TASK: GCD is %u (iterCount=%llu)", a, totalIterCount );
		printf( "\nWorker %d, GCD is: %u\n", pThread->GetThreadID(), a );
	}	
}

void DoPrimeTask( uint32_t *pTaskArgs, uint8_t numberCount, CDummyThread *pThread )
{
	if ( numberCount == 0 )
	{
		g_workerLog.WriteLogData( pThread->GetThreadID(), "PRIME TASK %d no parameters" );
		printf( "\nWorker received PRIME task without parameters, task aborted\n" );
	}

	for ( uint32_t curNumber = 0; curNumber < numberCount; curNumber++ )
	{
		bool bPrime = true;
		uint32_t testNumber = pTaskArgs[curNumber];
		uint32_t iterCount = 0;	
		uint32_t factorForNumber = 0;
		for ( uint32_t i = 2; i <= (testNumber/2); i++ )
		{
			if ( testNumber % i == 0 )
			{
				factorForNumber = i;
				bPrime = false;
				break;
			}	

			iterCount++;

			if ( (iterCount % 10000000) == 0 )
				std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
		}

		if ( bPrime )
		{
			g_workerLog.WriteLogData( pThread->GetThreadID(), "PRIME TASK: FOUND %u is prime", testNumber );
			printf( "\nWorker %d, number %u is prime\n", pThread->GetThreadID(), testNumber );
		}
		else
		{
			g_workerLog.WriteLogData( pThread->GetThreadID(), "PRIME TASK: FOUND %u is not prime (Factor is: %u)", testNumber, factorForNumber );
			printf( "\nWorker %d, number %u is not prime (it has a factor %u)\n", pThread->GetThreadID(), testNumber, factorForNumber ); 
		}
	}
		
}

bool HandleStateRun( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread *pThread )
{
	switch( taskType )
	{
	case TASK_NONE:
		// Do nothing
		break;

	case TASK_LCD:
		DoLCDTask( pTaskArgs, argCount, pThread );
		break;

	case TASK_GCD:
		DoGCDTask( pTaskArgs, argCount, pThread );
		break;

	case TASK_PRIME:
		DoPrimeTask( pTaskArgs, argCount, pThread );
		break;

	default:
		return (true);
		break;
	}

	// Task handled
	return (false);
}

bool HandleStateSleep( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread *pThread )
{
	g_workerLog.WriteLogData( pThread->GetThreadID(), "Sleeping" );
	printf( "Worker %d sleeping\n", pThread->GetThreadID() );

	while ( g_systemState == SYSTEM_STATE_SLEEP )
		std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );

	g_workerLog.WriteLogData( pThread->GetThreadID(), "Waking" );
	printf( "Worker %d waking\n", pThread->GetThreadID() );
	
	// Do nothing	
	return (false);
}

bool HandleStateTerminate( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread *pThread )
{
	g_workerLog.WriteLogData( pThread->GetThreadID(), "Terminating" );
	printf( "Worker %d terminating\n", pThread->GetThreadID() );
	return (true);
}

CDummyThread::CDummyThread( uint32_t threadID )
	: m_threadID( threadID )
{

}

void CDummyThread::RunThread( void )
{
	do	
	{
		if ( g_systemState >= SYSTEM_STATE_MAX )
		{
			// Reset
			g_systemState = SYSTEM_STATE_RUN;
			continue;	
		}


		g_workerLog.WriteLogData( GetThreadID(), "CHECKING FOR INPUT" );

		if ( (*g_fpSystemHandler[g_systemState])( (g_threadTaskList[GetThreadID()].taskParameters), g_threadTaskList[GetThreadID()].taskParameterCount, g_threadTaskList[GetThreadID()].taskType, this ) )
			break;
		else
			g_threadTaskList[GetThreadID()].taskType = TASK_NONE;
		
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	} while ( true );

	printf( "Worker exiting.\n" );
}
