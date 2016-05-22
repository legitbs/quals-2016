#include "common.h"

extern uint32_t g_systemState;
extern tTaskParameters g_threadTaskList[THREAD_COUNT];
extern CWorkerLogger g_workerLog;
char g_lineBuffer[512+1];

bool DoLog( CInputThread *pInputThread, vector<string> &sArgs )
{
	// Get first argument
	if ( sArgs.size() < 2 )
	{
		printf( "Insufficient arguments for log? Use: log <on|off>\n" );
		return false;
	}

	if ( sArgs[1] == "on" )
	{
		if ( g_workerLog.IsEnabled() )
			printf( "Logging is already on\n" );
		else
		{
			g_workerLog.Enable();
			printf( "Worker logging is now on\n" );
		}
	}
	else if ( sArgs[1] == "off" )
	{
		if ( !g_workerLog.IsEnabled() )
			printf( "Logging is already off\n" );
		else
		{
			g_workerLog.Disable();
			printf( "Worker logging is now off\n" );
		}
	}
	else
	{
		printf( "Incorrect arguments for log? Use: log <on|off>\n" );
		return false;
	}

	return false;
}

bool DoProcess( CInputThread *pInputThread, vector<string> &sArgs )
{
	// Get first argument
	if ( sArgs.size() < 3 )
	{
		printf( "Insufficient arguments for process? Use: process <worker> <task> <arguments>\n" );
		return false;
	}

	int32_t threadID = atoi( sArgs[1].c_str() );

	if ( threadID < 1 || threadID >= THREAD_COUNT )
	{
		printf( "Invalid worker ID\n" );
		return false;
	}

	uint8_t taskType;

	// Get task
	if ( sArgs[2] == "gcd" )
	{
		// Perform greatest common divisor
		taskType = TASK_GCD;
	}
	else if ( sArgs[2] == "lcd" )
	{
		// Perform least common divisor
		taskType = TASK_LCD;
	}
	else if ( sArgs[2] == "prime" )
	{
		// Test prime for each number
		taskType = TASK_PRIME;
	}
	else
	{
		printf( "Unknown task\n" );
		return false;
	}

	// BUG::sArgs.size()-3
	if ( sArgs.size()-4 > TASK_PARAMETER_COUNT )
	{
		printf( "Too many arguments!\n" );
		return false;
	}	

	// Get task count
	uint32_t taskCount = sArgs.size()-3;
	
	g_threadTaskList[threadID].taskType = taskType;
	g_threadTaskList[threadID].taskParameterCount = taskCount;

	for ( uint32_t i = 0; i < taskCount; i++ )
	{
		g_threadTaskList[threadID].taskParameters[i] = strtoul( sArgs[i+3].c_str(), NULL, 10 );
	}

	//printf( "g_systemState ADDR: %X | g_threadTaskList[4].taskParameters[4] ADDR: %X\n", &g_systemState, &(g_threadTaskList[4].taskParameters[4]) );
	printf( "Task %s sent to worker %d\n", sArgs[2].c_str(), threadID );
}

bool DoQuit( CInputThread *pInputThread, vector<string> &sArgs )
{
	g_systemState = SYSTEM_STATE_TERMINATE;				
	printf( "Quitting.\n" );

	return true;	
}

bool DoSleep( CInputThread *pInputThread, vector<string> &sArgs )
{
	g_systemState = SYSTEM_STATE_SLEEP;
	printf( "Sleeping workers.\n" );

	return false;
}

bool DoWake( CInputThread *pInputThread, vector<string> &sArgs )
{
	g_systemState = SYSTEM_STATE_RUN;
	printf( "Waking, running workers.\n" );

	return false;
}

CInputThread::CInputThread( uint32_t threadID, CIOStdioConnection *pConnection )
	: m_threadID( threadID ), m_pConnection( pConnection )
{

}

void CInputThread::RunThread( void )
{
	char *lineBuffer = g_lineBuffer;
	
	m_cmdMap["quit"] = DoQuit;
	m_cmdMap["q"] = DoQuit;
	m_cmdMap["process"] = DoProcess;
	m_cmdMap["p"] = DoProcess;
	m_cmdMap["s"] = DoSleep;
	m_cmdMap["sleep"] = DoSleep;
	m_cmdMap["w"] = DoWake;
	m_cmdMap["wake"] = DoWake;
	m_cmdMap["log"] = DoLog;
	m_cmdMap["l"] = DoLog;

	bool bDone = false;
	do
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

		printf( "Enter command: " );
		int32_t dataRead = m_pConnection->ReadLine( (uint8_t *)lineBuffer, 512 );

		vector<string> sArgs = SplitString( lineBuffer );

		if ( sArgs.size() == 0 )
			continue;

		map<string, tCmdHandler>::const_iterator command_iter = m_cmdMap.find ( sArgs[0] );
		if ( command_iter == m_cmdMap.end () )
		{
			printf( "Command not found!\n" );
		}
		else
		{
			bDone = command_iter->second( this, sArgs );
        	}

	} while ( !bDone );

	return;
}
