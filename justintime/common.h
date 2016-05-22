#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <map>
#include <vector>
#include <string>

#define ENABLE_SIG_ALARM_HANDLER	(1)

#define THREAD_COUNT		(5)
#define TASK_PARAMETER_COUNT	(4)

#define TASK_NONE		(0)
#define TASK_GCD		(1)
#define TASK_LCD		(2)
#define TASK_PRIME		(3)

// Set the MAX_LINE_LEN
#define MAX_LINE_LEN		1024

// Signal alarm timeout for service
#define MAX_IDLE_SECS		60

#define SYSTEM_STATE_RUN	(0)
#define SYSTEM_STATE_SLEEP	(1)
#define SYSTEM_STATE_TERMINATE	(2)
#define SYSTEM_STATE_MAX	(3)

#define LOG_FILE_NAME 	"/tmp/worker_log.log"

#define BUILD_TIMESTAMP	0x56c35c5b

// This is used to debug input commands
//#define DEBUG_INPUT_HELPER	1

// Debug output data
//#define DEBUG_OUTPUT		1

using namespace std;

typedef struct
{
        uint8_t taskType;
        uint8_t taskParameterCount;
        uint32_t taskParameters[TASK_PARAMETER_COUNT];
} tTaskParameters;

#include "doublelist.h"
#include "cioconnection.h"
#include "input_thread.h"
#include "dummy_thread.h"
#include "worker_log.h"

uint32_t ReadDevURandom32( void );
void ReadDevURandom256( uint8_t *, uint32_t );

uint32_t DoCRC32( uint32_t crc, const uint8_t *pData, uint32_t size );

int32_t debug_log( uint32_t priority, const char *format, ... );
void set_log_priority( uint32_t priority );

void FreeMessageWrapper( void * );

vector<string> SplitString( string sInStr );

bool HandleStateRun( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread * );
bool HandleStateSleep( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread * );
bool HandleStateTerminate( uint32_t *pTaskArgs, uint8_t argCount, uint8_t taskType, CDummyThread * );

#endif // __COMMON_H__
