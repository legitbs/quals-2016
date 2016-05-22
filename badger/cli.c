#include "port_helper.h"
#include "hardware.h"
#include "cli.h"
#include "radio_uart.h"
#include "util.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>

// TURN THIS ON TO ENABLE TESTING OF SHELLCODE
// #define DEBUG_TEST_SHELLCODE	1

#define CLI_LINE_NONE		0	// no line to process
#define CLI_LINE_READY		1	// line to process

#define FORWARD_MESSAGE_LENGTH (86*3)-3

#define MAX_LINE_LENGTH		((FORWARD_MESSAGE_LENGTH*2)+20)		// Maximum CLI receive line
#define MAX_OUTPUT_LENGTH	(80)					// Maximum CLI output line

volatile uint8_t g_cliUartBuffer[MAX_LINE_LENGTH];
volatile uint8_t g_cliLineBuffer[MAX_LINE_LENGTH];

volatile uint16_t g_cliUartPos;
volatile uint8_t g_cliLineState;

extern volatile uint8_t g_myTeamID;

uint8_t toupper( uint8_t c )
{
	if ( (c>='a') && (c<='z') )
		return (c+'A'-'a');

	return (c);
}

uint8_t stringbeg( uint8_t *pString, uint8_t *pMatch )
{
	uint16_t i = 0;
	uint16_t match_len = strlen( (const char*)pMatch );
	uint16_t string_len = strlen( (const char *)pString );

	while ( (i<match_len) && (i<string_len) && (toupper( pString[i] ) == toupper( pMatch[i] )) )
		i++;

	if ( i == match_len )
		return 1;
	
	return 0;
}

void cli_print_hex_u16( uint16_t value )
{
	putchar( int_to_hex_char( (value >> 12) & 0xF) );
	putchar( int_to_hex_char( (value >> 8) & 0xF) );
	putchar( int_to_hex_char( (value >> 4) & 0xF) );
	putchar( int_to_hex_char( (value & 0xF) ) );
}

void cli_init_uart( void )
{
	/* PORT -- DISABLE
	CLI_UART_BAUD = CLI_BAUD;
	CLI_UART_CTL = UART_EN | UART_IEN_RX;
	*/

	g_cliUartPos = 0;
	g_cliLineState = CLI_LINE_NONE;
}

int putchar( int txdata )
{
	write( 1, &txdata, 1 );

	/*
	// Wait for TX to finish
	while ( CLI_UART_STAT & UART_TX_FULL )
		;

	// Write output character
	CLI_UART_TXD = txdata;
	*/
	return 0;
}

//--------------------------------------------------//
//        UART RX interrupt service routine         //
//         (receive a byte from the UART)           //
//--------------------------------------------------//

//wakeup interrupt (CLI_UART_RX_VECTOR) INT_cli_uart_rx(void) 
void INT_cli_uart_rx( uint8_t rxdata )
{
	/*
	uint8_t rxdata;

	// Read the received data
	rxdata = CLI_UART_RXD;

	// Clear the receive pending flag
	CLI_UART_STAT = UART_RX_PND;
	*/
	
	g_cliUartBuffer[g_cliUartPos++] = rxdata;

	if ( g_cliUartPos >= MAX_LINE_LENGTH || g_cliUartBuffer[g_cliUartPos-1] == '\n' )
	{
		memcpy( (void *)g_cliLineBuffer, (void *)g_cliUartBuffer, g_cliUartPos-1 );
		g_cliLineBuffer[g_cliUartPos-1] = '\0';

		g_cliLineState = CLI_LINE_READY;

		g_cliUartPos = 0;
	}
	else
		putchar( rxdata );

	// Exit the low power mode
	LPM0_EXIT;
}

void cli_run( void )
{
	uint8_t localBuffer[MAX_LINE_LENGTH];
	uint16_t bufferLen;

	if ( g_cliLineState == CLI_LINE_READY )
	{
		bufferLen = strlen( (const char*)g_cliLineBuffer );

		memcpy( (void *)localBuffer, (void *)g_cliLineBuffer, bufferLen );
		localBuffer[bufferLen] = '\0';
	
		g_cliLineState = CLI_LINE_NONE;

		cli_process_command( localBuffer );

		// Output response
		puts( "\r" );

		// Output prompt!
		putchar( ':' );	
	}
}

//----------------------------------------------//
// Processes input commands on the command line
// interface
//----------------------------------------------//
void cli_process_command( uint8_t *pCommand )
{
	if ( stringbeg( pCommand, (uint8_t*)"?" ) )
		cli_command_help( pCommand+1 );
	else if ( stringbeg( pCommand, (uint8_t*)"msg" ) )
		cli_command_msg( pCommand+3 );
	else if ( stringbeg( pCommand, (uint8_t*)"token" ) )
		cli_command_token( pCommand+5 );
	else if ( stringbeg( pCommand, (uint8_t*)"debug" ) )
		cli_command_debug( pCommand+5 );
	else if ( stringbeg( pCommand, (uint8_t*)"id" ) )
		cli_command_id( pCommand+2 );
	else
	{
		// Unknown command
		puts( "Unknown command" );
	}	
}

#ifdef DEBUG_TEST_SHELLCODE
/* C:\Users\jdub\Documents\Defcon\Finals\badge_analysis\exploit_test\overflow_msg.bin (8/3/2014 2:03:06 PM)
   StartOffset: 00000000, EndOffset: 0000007B, Length: 0000007C */

unsigned char rawData[124] = {
	0x34, 0x02, 0x10, 0x78, 0x7F, 0x76, 0x81, 0xD0, 0xE3, 0xF4, 0x2D, 0x04,
	0x07, 0x03, 0x4F, 0x81, 0x40, 0x1E, 0x10, 0x68, 0x9E, 0x40, 0xC1, 0x1E,
	0x80, 0xE9, 0x1E, 0xC0, 0xC1, 0x9F, 0x00, 0xE9, 0x9F, 0x40, 0xC2, 0x20,
	0x13, 0xFA, 0x06, 0x12, 0x85, 0x3E, 0xA1, 0x4B, 0x68, 0x33, 0xCA, 0x04,
	0x2A, 0x01, 0x3B, 0xA0, 0x54, 0x3C, 0x70, 0xB8, 0x94, 0xC2, 0x83, 0xAA,
	0x05, 0x7E, 0x1B, 0x7C, 0x57, 0xF8, 0xF1, 0xF2, 0x67, 0xE5, 0xD7, 0xCD,
	0xBF, 0x9F, 0x9F, 0x47, 0x7E, 0x9F, 0x7D, 0x5F, 0xFB, 0x01, 0xF6, 0x87,
	0xEE, 0x17, 0xDE, 0x3F, 0x80, 0x9F, 0x09, 0x7E, 0x23, 0x7C, 0x67, 0xF9,
	0x11, 0xF2, 0xA7, 0xE6, 0x57, 0xCE, 0xB8, 0xE8, 0x34, 0x3A, 0x2D, 0x1E,
	0x93, 0x4B, 0xA6, 0xD3, 0xEA, 0x35, 0x3A, 0xAD, 0x5E, 0xB3, 0x5B, 0xAE,
	0xD7, 0xD4, 0x27, 0xA0
};
#endif

void cli_command_help( uint8_t *pArgs )
{
#ifdef DEBUG_TEST_SHELLCODE
	recv_data_message( rawData, sizeof(rawData) );
#endif

	if ( pArgs[0] == 0 )
		puts( "Valid Commands: ?,msg,token,id" );
	else
	{
		while ( pArgs[0] == ' ' )
			pArgs++;

		if ( stringbeg( (uint8_t*)"msg", pArgs ) )
			puts( "Usage: msg <to team id>,<from team id>,<hex msg>\nEx: msg 1,0,3a5caf\nSends data message to team 0 from team 1" );
		else if ( stringbeg( (uint8_t*)"token", pArgs ) )
			puts( "Current scoring token" );
		else if ( stringbeg( (uint8_t*)"id", pArgs ) )
			puts( "This Team ID" );
		else
			puts( "Unknown command" );
	}
}

void cli_command_msg( uint8_t *pArgs )
{
	uint8_t pos=0;
	uint16_t tid_loc = 0;	
	uint16_t fid_loc = 0;
	uint8_t from_tid = 0, to_tid = 0;
	uint8_t msgBuffer[FORWARD_MESSAGE_LENGTH];
	uint16_t msgBufferPos = 0;

	while ( pArgs[pos] != '\0' && pArgs[pos+1] != '\0' )
	{
		if ( pArgs[pos] == ' ' )
			pos++;
		else if ( pArgs[pos] == ',' )
		{
			if ( tid_loc == 0 )
				tid_loc = pos;
			else
				fid_loc = pos;

			pos++;
		}
		else if ( fid_loc > 0 )
		{
			msgBuffer[msgBufferPos++] = (hex_char_to_int( pArgs[pos] ) << 4) | hex_char_to_int( pArgs[pos+1] );
			pos += 2;
		}
		else if ( pArgs[pos] >= '0' && pArgs[pos] <= '9' )
		{
			if ( tid_loc == 0 )
				to_tid = (to_tid * 10) + (pArgs[pos] - '0');
			else if ( fid_loc == 0 )
				from_tid = (from_tid * 10) + (pArgs[pos] - '0');

			pos++;
		}
		else
		{
			puts( "Syntax error. Try ? msg" );
			return;
		}
	}

	if ( tid_loc == 0 || fid_loc == 0 )
	{
		puts( "Syntax error. Try ? msg" );
		return;
	}

	if ( to_tid == g_myTeamID )
	{
		puts( "Invalid to team ID, not self" );
		return;
	}

	if ( to_tid >= TEAM_COUNT )
	{
		puts( "Invalid to team ID, 0-19" );
		return;
	}

	// Queue message to send!
	if ( msgBufferPos > FORWARD_DATA_SIZE )
	{
		puts( "Fail. Too large" );
		return;
	}

	if ( radio_add_forward_message( msgBuffer, msgBufferPos, from_tid, to_tid ) != 0 )
	{
		puts( "Queued. Wait MQ: OK for session ID" );
		return;
	}
	else
	{
		puts( "Fail. Bad TID or message in queue" );
		return;
	}
}

void cli_command_token( uint8_t *pArgs )
{
	char token_string[32];

	puts( "Token:" );
	/*
	cli_print_hex_u16( TOKEN_0 );
	cli_print_hex_u16( TOKEN_1 );
	cli_print_hex_u16( TOKEN_2 );
	cli_print_hex_u16( TOKEN_3 );
	*/
	puts( "You thought it would be that easy... eh?" );
	puts( "" );

	/*
	// TODO: Keep this in only for debugging	
	memcpy( token_string, "Status: ", 7 );
	stringpos = 7;	
	stringpos += cli_write_hex_u16( POLL_STATUS_0, (uint8_t*)token_string+stringpos, (32-stringpos) ); 
	stringpos += cli_write_hex_u16( POLL_STATUS_1, (uint8_t*)token_string+stringpos, (32-stringpos) ); 
	stringpos += cli_write_hex_u16( POLL_STATUS_2, (uint8_t*)token_string+stringpos, (32-stringpos) ); 
	stringpos += cli_write_hex_u16( POLL_STATUS_3, (uint8_t*)token_string+stringpos, (32-stringpos) ); 
	token_string[stringpos] = '\0';
	puts( token_string );
	*/
}

void cli_command_debug( uint8_t *pArgs )
{
	uint8_t debugLevel;
	uint16_t pos = 0;

	while ( pArgs[pos] != '\0' )
	{
		if ( pArgs[pos] == ' ' )
			pos++;
		else
			break;
	}

	if ( pArgs[pos] >= '0' && pArgs[pos] <= '9' )
		debugLevel = pArgs[pos] - '0';
	else
	{
		puts( "Syntax error" );
		return;
	}

	radio_uart_set_debug( debugLevel );

	puts( "Debug Level: " );
	cli_print_hex_u16( radio_uart_get_debug() );
	puts( "" );
}

void cli_command_id( uint8_t *pArgs )
{
	uint8_t teamID = g_myTeamID;

	puts( "Team ID: " );
	cli_print_hex_u16( teamID );
	puts( "" );
}
