#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

// NOTE:
// !!!!!!!!!!!!!!!!!!!!!!!!!
// CHANGE these two defines before release
//#define	CRYPTO_KEY_FILENAME	"/home/jdub/quals_15/source_code/sirgoon/quals_15/secrf/server/crypto.key"
//#define	CRYPTO_KEY_FILENAME	"/home/secrf/crypto.key\0aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define	CRYPTO_KEY_FILENAME	"/home/secrfrevenge/crypto.key\0aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define MAX_IDLE_SECS		(30)

//#define TEST_MODE_ON		(1)

#define CLOCKS_PER_SYMBOL	(33)

#define SAMPLE_FS			(13200)
#define MAX_TIME			(10)
#define MAX_SAMPLE_POINTS	(MAX_TIME * SAMPLE_FS)
#define MIN_SAMPLE_POINTS	(128)

#define PI_VALUE			3.1415926535897932384626433832795

#define MAX_PACKET_FRAGMENTS		(16)

#define CRYPTO_KEY_SIZE			(16)		// 128 bits
#define CRYPTO_RC4_DROP_AMOUNT	(256)		// Drop first 256 bytes

#define MAX_FRAGMENT_BUFFER		(16)		// Maximum number of sequence numbers that can be in packet reassembly buffer

#ifdef _WIN32
#define snprintf _snprintf

#define GET_FILE_NAME()		__FILE__
#define GET_LINE_NUMBER()	__LINE__
#else
#define GET_FILE_NAME()		__FILE__
#define GET_LINE_NUMBER()	__LINE__
#endif

#define DEBUG_MESSAGE_ERROR	(0)
#define DEBUG_MESSAGE_INFO	(1)

#include "cpp_helper.h"
#include "exception_handler.h"
#include "well_rng.h"
#include "critical_exception.h"
#include "doublelist.h"
#include "dsp_engine.h"
#include "packet.h"
#include "rc4engine.h"
#include "radiomessage.h"
#include "radiomac.h"
#include "digital_receiver.h"
#include "messagehandler.h"

#ifdef _WIN32
static inline double round(double val)
{    
    return floor(val + 0.5);
}
#endif

void SetLogLevel( uint8_t logLevel );
void LogMessage( uint8_t messageType, const char *fmt, ... );

#endif // __COMMON_H__
