#include "common.h"

#include <sys/select.h>
#include <unistd.h>

#define STDIN	(0)

void sig_alarm_handler( int signum )
{
	printf( "Timeout! Connection closing.\n" );

	exit( 1 );
}

bool TLVFuncDisplayMAC( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	if ( dataLen != 6 )
		return (true);

	char szTemp[256];

	snprintf( szTemp, 256, "%2X:%2X:%2X:%2X:%2X:%2X\n", pData[0], pData[1], pData[2], pData[3], pData[4], pData[5] );

	printf( szTemp );

	return (true);
}

bool TLVFuncDisplayTime( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	if ( dataLen != 4 )
		return (true);

	printf( "%d - %d\n", *((uint32_t*)pData), (uint32_t)time(NULL) );

	return (true);
}

bool TLVFuncDisplayRandom( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	if ( dataLen != 4 )
		return (true);

	printf( "%d - %d\n", *((uint32_t*)pData), rand() );

	return (true);
}

bool TLVFuncCheckCredentials( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	if ( dataLen != 12 )
		return (true);

	char szTemp[12];
	strncpy( szTemp, (char *)pData, 12 );

	if ( strcmp( szTemp, "credential" ) == 0 )
		printf( "CREDENTIAL check OK\n" );
	else
		printf( "CREDENTIAL check FAILED\n" );

	return (true);
}

bool TLVFuncDisplayString( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	char szTemp[32];

	memcpy( szTemp, pData, dataLen );

	printf( "%s\n", szTemp );

	return (true);
}

bool TLVFuncDisplayError( void *pInstance, uint8_t *pData, uint16_t dataLen )
{
	char szErrorText[512];
	sprintf( szErrorText, "Parsing TLV field data length out of range, message length=%X\n", dataLen );

	THROW( new CCriticalException( szErrorText, GET_FILE_NAME(), GET_LINE_NUMBER() ) );

	return (true);
}

void RecvUntil( uint8_t *pBuffer, uint32_t count )
{
	uint32_t pos = 0;

	for ( pos = 0; pos < count; pos++ )
	{
		if ( fread( pBuffer+pos, 1, 1, stdin ) != 1 )
		{
			printf( "Recv error.\n" );
			exit(-1);
			// Exit
		}
	}
}

uint32_t ReadUint32( void )
{
	uint32_t value;

	RecvUntil( (uint8_t*)&value, sizeof(value) );

	return (value);
}

int8_t ReadInt8( void )
{
	int8_t value;

	RecvUntil( (uint8_t*)&value, sizeof(value) );

	return (value);
}

uint8_t ReadUint8( void )
{
	uint8_t value;

	RecvUntil( (uint8_t*)&value, sizeof(value) );

	return (value);
}

void CheckTestMode( void )
{
	fd_set fds;
	int maxfd;

	maxfd = 1;

	FD_ZERO(&fds);
	FD_SET(STDIN, &fds);

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	select( maxfd, &fds, NULL, NULL, &tv );

	if ( FD_ISSET( STDIN, &fds ) )
	{
		uint8_t turnOnLogging = ReadUint8();

		if ( turnOnLogging == '!' )
		{
			printf( "Logging on!\n" );
			SetLogLevel( DEBUG_MESSAGE_INFO );
		}
	
		int c;
		while((c = getchar()) != '\n' && c != EOF);
	} 
	
}

uint32_t ReadSamples( uint8_t *pInputSamples, uint32_t minSampleCount, uint32_t maxSampleCount )
{
	uint32_t recvSampleCount = ReadUint32();

	if ( recvSampleCount > maxSampleCount )
	{
		printf( "Too many samples.\n" );
		exit( -1 );
	}

	if ( recvSampleCount < minSampleCount )
	{
		printf( "Too few samples to process.\n" );
		exit( -1 );
	}

	for ( uint32_t i = 0; i < recvSampleCount; i++ )
	{
		pInputSamples[i] = ReadUint8();
	}

	// Log message
	LogMessage( DEBUG_MESSAGE_INFO, "%d samples received", recvSampleCount );

	return (recvSampleCount);
}

void Demodulate( uint8_t *pInputSamples, uint32_t sampleCount )
{
	CMessageHandler oMessageHandler;
	CRadioMAC oRadioMAC;

	oRadioMAC.LoadKeyFile( CRYPTO_KEY_FILENAME );
	CDigitalReceiver oReceiver( &oRadioMAC );

	// TLV Table
	tTLVField tlvParseTable[] = {
	{ 0x10, 0, &TLVFuncDisplayMAC		},
	{ 0x11, 0, &TLVFuncDisplayTime		},
	{ 0x31, 0, &TLVFuncDisplayRandom		},
	{ 0x69, 0, &TLVFuncCheckCredentials	},
	{ 0x81, 0, &TLVFuncCheckCredentials	},
	{ 0x90, 0, &TLVFuncDisplayString		},
	{ 0x0, TLV_FLAG_ERROR, &TLVFuncDisplayError },
	};

	// OUTPUT2
	bool bDone = false;
	for ( uint32_t sample_num = 0; sample_num < MAX_SAMPLE_POINTS; sample_num++ )
	{
		uint8_t out2 = 0;
		
		// Do the FSK demodulator now!
		double bpf1200_out = CDSPEngine::BandpassFilter1200( (double)pInputSamples[sample_num] );
		double bpf2200_out = CDSPEngine::BandpassFilter2200( (double)pInputSamples[sample_num] );
	
		double lpfFilterOut = CDSPEngine::EnvelopeDetector1( bpf1200_out );
		double lpfFilterOut2 = CDSPEngine::EnvelopeDetector2( bpf2200_out );

		if ( lpfFilterOut > lpfFilterOut2 )
			out2 = 1;
		else
			out2 = 0;

		// Receive a sample
		TRY	
		{
			oReceiver.ReceiveSample( out2 );

			// Check for any messages????
			if ( oRadioMAC.IsMessageAvailable() )
			{
				CRadioMessage *pNewMessage = oRadioMAC.PopMessage();

				oMessageHandler.ParseMessage( tlvParseTable, 7, pNewMessage->GetData(), pNewMessage->GetLength() );

				delete pNewMessage;
			}
		}
		CATCH( EXCEPTION_TYPE_CRITICAL )
		{
			CCriticalException *pException = (CCriticalException *)GetExceptionPointer();
			LogMessage( DEBUG_MESSAGE_ERROR, "Critical exception: %s in file %s at line %d", pException->GetExceptionText().CStr(), pException->GetFileName().CStr(), pException->GetLineNumber() );

			//bDone = true;;
		}
		FINALLY
		{

		}
		ETRY

		if ( bDone )
			break;
	}
}


void ReadDevURandomSeed( void )
{
        FILE *pFile;
        uint32_t rngInitData[16];

        pFile = fopen( "/dev/urandom", "rb" );

        if ( !pFile )
        {
                printf( "Couldn't open /dev/urandom reverting to seed.\n" );
		srand( time(NULL) ^ getpid());

		for ( uint32_t i = 0; i < 16; i++ )
			rngInitData[i] = rand();

		InitWELLRNG512a( rngInitData );
		
		return;
        }

        if ( fread( rngInitData, sizeof(uint32_t), 16, pFile ) != 16 )
        {
                printf( "Couldn't get enough entropy.  Reverting to seed.\n" );
		for ( uint32_t i = 0; i < 16; i++ )
			rngInitData[i] = rand();

		InitWELLRNG512a( rngInitData );

		return;
        }

        fclose( pFile );
		
	InitWELLRNG512a( rngInitData );
}

#define CHANNEL_NOISE_AMPLITUDE	(8.0)
uint8_t AddAWGNToSample( uint8_t sample )
{
	double S, V1, V2;

	do
	{
		double U1 = WELLRNG512a();
		double U2 = WELLRNG512a();

		V1 = (2 * U1) - 1.0;
		V2 = (2 * U2) - 1.0;

		S = (V1 * V1) + (V2 * V2);
	} while ( S >= 1.0 );	

	double X = sqrt( -2.0 * log(S) / S ) * V1;
	// double Y = sqrt( -2.0 * log(S) / S ) * V2;

	int16_t sample_noise = (int16_t)(X * sqrt( CHANNEL_NOISE_AMPLITUDE ));

	int16_t test_sample_value = sample;
	test_sample_value += sample_noise;

	if ( test_sample_value < 0 )
		sample = 0;
	else if ( test_sample_value > 255 )
		sample = 255;
	else
		sample = (uint8_t)test_sample_value;

	return sample;
}

void AddChannelNoise( uint8_t *pInputSamples, uint32_t sampleCount )
{	
	// Apply channel noise
	for ( uint32_t i = 0; i < sampleCount; i++ )
		pInputSamples[i] = AddAWGNToSample( pInputSamples[i] ); 
}

int main( void )
{
	// Setup WELL RNG
	ReadDevURandomSeed();

	// Check if they entered test mode
	CheckTestMode();
	
	// Setup sig alarm handler
	signal( SIGALRM, sig_alarm_handler );
	alarm( MAX_IDLE_SECS );

	// Setup no buffering for XINETD service
	setvbuf( stdout, NULL, _IONBF, 0 );
	
	// Print banner
	printf( "Send baseband RF capture in format 13.2Khz/8-bit samples:\n" );
	printf( "Format is [number of samples][capture data ...]\n" );
	printf( "Max capture size is 10 seconds: 132,000 samples.\n" );

	srand( time(NULL) );

	// Read in samples
	uint8_t pInputSamples[MAX_SAMPLE_POINTS];
	
	// INFO:: sample buffer
	LogMessage( DEBUG_MESSAGE_INFO, "%16llX sample buffer", pInputSamples );

	// Read in samples	
	uint32_t sampleCount = ReadSamples( pInputSamples, MIN_SAMPLE_POINTS, MAX_SAMPLE_POINTS );

	// AWGN Noise
	AddChannelNoise( pInputSamples, sampleCount );

	// Demodulate
	TRY	
	{
		Demodulate( pInputSamples, sampleCount );

	}
	CATCH( EXCEPTION_TYPE_CRITICAL )
	{
		CCriticalException *pException = (CCriticalException *)GetExceptionPointer();
		LogMessage( DEBUG_MESSAGE_ERROR, "Critical exception: %s in file %s at line %d", pException->GetExceptionText().CStr(), pException->GetFileName().CStr(), pException->GetLineNumber() );
	}
	FINALLY
	{

	}
	ETRY
	
	// Free memory
	// delete pInputSamples;

	return 0;
}
