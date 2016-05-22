#include "port_helper.h"
#include "radio_uart.h"
#include "hardware.h"
#include "message_handler.h"
#include "util.h"
#include "gui.h"
//#include "delay.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH		(540)

#define MAX_SEND_DATA_LENGTH	(FORWARD_DATA_SIZE+32)

#define RADIO_UART_MODE_LINE	0
#define RADIO_UART_MODE_DATA	1

#define RADIO_UART_LINE_NONE	0
#define RADIO_UART_LINE_AVAIL	1

#define WAIT_RADIO_READY_TIMEOUT	(65000)
#define WAIT_RADIO_READY_TRIES		(8)

volatile char g_radioUartLine[MAX_LINE_LENGTH];
volatile char g_radioUartLastLine[MAX_LINE_LENGTH];
volatile uint16_t g_radioUartPos;
volatile uint16_t g_radioUartAmount;
volatile uint8_t g_radioUartLineStatus;
volatile uint8_t g_radioUartDebugLevel;
volatile uint8_t g_radioUartTXReady;
volatile uint8_t g_radioUartSendWait;		// Delays sending of data messages until MQ: OK received from last one

volatile uint8_t g_radioState;
volatile uint8_t g_radioLastState;

extern volatile tSendForwardData g_forwardMessageQueue[TEAM_COUNT];

extern volatile uint8_t g_myTeamID;

void radio_init_uart( void )
{
	uint8_t i;

	/*
	APP_UART_BAUD = APP_BAUD;
	APP_UART_CTL = UART_EN | UART_IEN_RX;
	*/

	g_radioUartPos = 0;
	g_radioUartSendWait = 0;

	g_radioUartLineStatus = RADIO_UART_LINE_NONE;

	g_radioUartDebugLevel = RU_DEBUG_OFF;

	g_radioState = RU_STATE_OFF;
	g_radioLastState = RU_STATE_OFF;

	// Clear the message pending buffer
	for ( i = 0; i < TEAM_COUNT; i++ )
	{
		g_forwardMessageQueue[i].status = FORWARD_MESSAGE_STATUS_EMPTY;
		g_forwardMessageQueue[i].state = FORWARD_MESSAGE_STATE_NONE;
	}
}

void radio_uart_write( uint8_t *pData, uint16_t dataLen )
{
	uint16_t pos = 0;
	uint16_t wait_count;
	for ( pos = 0; pos < dataLen; pos++ )
	{
		//APP_UART_TXD = pData[pos];
		radio_uart_putc( pData[pos] ); //write( 1, &pData[pos], 1 );

		g_radioUartTXReady = 0;

		/* PORT -- Don't wait
		for ( wait_count = 0; wait_count < 65000; wait_count++ )
		{
			if ( g_radioUartTXReady != 0 )
				break;
			else
				delay( 0, 0x8 );
		}
		*/
	}
}

void radio_uart_clear_last_line( void )
{
	g_radioUartLastLine[0] = '\0';
}  


//wakeup interrupt (RADIOUART_RX_VECTOR) INT_radio_uart_rx(void) 
void INT_radio_uart_rx( uint8_t in_char ) 
{
	/* OLD PORT CODE
	uint8_t in_char;

	in_char = APP_UART_RXD;

	// Clear the receive pending flag
	APP_UART_STAT = UART_RX_PND;
	*/
	if ( in_char != '>' )
	{
		// Read the received data
		g_radioUartLine[g_radioUartPos++] = in_char;

		if ( g_radioUartPos >= MAX_LINE_LENGTH || g_radioUartLine[g_radioUartPos-1] == '\n' )
		{
			memcpy( (void *)g_radioUartLastLine, (void *)g_radioUartLine, g_radioUartPos-1 );
			g_radioUartLastLine[g_radioUartPos-1] = '\0';
		
			g_radioUartPos = 0;

			g_radioUartLineStatus = RADIO_UART_LINE_AVAIL;

			LPM0_EXIT;
			return;
		}
	}
	else
		g_radioUartTXReady = 1;
}

uint8_t radio_data_transfer_wait_ready( void )
{
	uint16_t waitTime = 0;

	for ( waitTime = 0; waitTime < WAIT_RADIO_READY_TIMEOUT; waitTime++ )
	{
		// PORT: Don't delay
		// delay( 0, 0xF );
		if ( g_radioUartLastLine[0] == 'R' && g_radioUartLastLine[1] == 'D' && g_radioUartLastLine[2] == 'Y' )
				break;
	}

	if ( waitTime == WAIT_RADIO_READY_TIMEOUT )
		return (0xFF);

	return 0;
}

void print_line_data( uint8_t *line, uint8_t line_length )
{
	uint8_t i;

	for ( i = 0; i < line_length; i++ )
		putchar( line[i] );
}

void print_send_data( char *line_text, uint16_t line_length, uint8_t *pre_data, uint16_t pre_data_len, uint8_t *data, uint16_t data_len )
{
	uint16_t i, pos;
	uint8_t temp[FORWARD_DATA_SIZE*2+40];
	uint8_t checksum;

	for ( i = 0; i < line_length; i++ )
		temp[pos++] = line_text[i];

	checksum = 0;
	for ( i = 0; i < pre_data_len; i++ )
	{
		temp[pos++] = int_to_hex_char( (pre_data[i] >> 4) & 0xF );
		temp[pos++] = int_to_hex_char( (pre_data[i] & 0xF) );	

		checksum += pre_data[i];
	}
		
	for ( i = 0; i < data_len; i++ )
	{
		temp[pos++] = int_to_hex_char( (data[i] >> 4) & 0xF );
		temp[pos++] = int_to_hex_char( (data[i] & 0xF) );	

		checksum += data[i];
	}

	temp[pos++] = ',';
	temp[pos++] = int_to_hex_char( (checksum >> 4) & 0xF );
	temp[pos++] = int_to_hex_char( (checksum & 0xF) );
	temp[pos] = '\0';

	puts( "TX:" );
	puts( temp );
}

uint8_t radio_send_response_message( uint8_t *pData, uint8_t dataLen, uint16_t session_id, uint8_t from_tid )
{
	uint8_t radioStartMessage[] = "RMXX:\n";
	uint8_t tmpData[4];
	uint8_t try_count;
	uint8_t send_amount;

	// Add on session id and from TID
	send_amount = dataLen+3;
	radioStartMessage[2] = int_to_hex_char( (send_amount >> 4) & 0xF );
	radioStartMessage[3] = int_to_hex_char( (send_amount & 0xF) );

	// Wait to send
	g_radioUartSendWait = 1;

	try_count = 3;
	do
	{
		radio_uart_write( radioStartMessage, 6 );

		// WAIT for RADIO ready
		// if ( radio_data_transfer_wait_ready() == 0 )
		if ( 1 )
		{
			tmpData[0] = (session_id & 0xFF);
			tmpData[1] = ((session_id >> 8) & 0xFF);
			tmpData[2] = from_tid;

			// Transfer To Team ID and From Team ID
			radio_uart_write( tmpData, 3 );
			radio_uart_write( pData, dataLen );

			// Remember sent data to echo to team
			if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
				print_send_data( "RM:", 3, tmpData, 3, pData, dataLen );

			break;
		}
#ifdef DEBUG_APP_MESSAGES
		else
			puts( "WAIT RDY FAIL" );
#endif
	}
	while ( try_count-- > 0 );

	if ( try_count == 0 )
	{		
		if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
			puts( "ERR: RDY TIMEOUT" );
		
		return (0xFF);
	}

	return (0); // Message ready

}

uint8_t radio_send_data_message( uint8_t *pData, uint8_t dataLen, uint8_t from_tid, uint8_t to_tid )
{
	uint8_t radioStartMessage[] = "DMXX:\n";
	uint8_t tmpData[2];
	uint8_t try_count;


	radioStartMessage[2] = int_to_hex_char( (dataLen >> 4) & 0xF );
	radioStartMessage[3] = int_to_hex_char( (dataLen & 0xF) );

	// Wait to send				
	g_radioUartSendWait = 1;
	
	try_count = 3;
	do
	{
		// WAIT for RADIO ready
		radio_uart_write( radioStartMessage, 6 );
		radio_uart_clear_last_line(); 

		if ( radio_data_transfer_wait_ready() == 0 )
		{
			tmpData[0] = to_tid;
			tmpData[1] = from_tid;

			// Transfer To Team ID and From Team ID
			radio_uart_write( tmpData, 2 );
			radio_uart_write( pData, dataLen );

			if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
				print_send_data( "DM:", 3, tmpData, 2, pData, dataLen );

			break;
		}
	} while ( try_count-- > 0 );

	if ( try_count == 0 )
	{
		if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
			puts( "ERR: RDY TIMEOUT" );
		
		return (0xFF);
	}

	return (0); // Message ready
}

void radio_uart_run( void )
{
	uint8_t lineTemp[MAX_LINE_LENGTH];
	if ( g_radioUartLineStatus == RADIO_UART_LINE_AVAIL )
	{
		// SYNC processing of the line received
		dint();
		uint16_t lineLength = strlen( (const char*)g_radioUartLastLine );

		if ( lineLength >= MAX_LINE_LENGTH-1 )
			lineLength = MAX_LINE_LENGTH-1;

		memcpy( (void *)lineTemp, (void *)g_radioUartLastLine, lineLength  );
		eint();

		lineTemp[lineLength] = '\0';

		g_radioUartLineStatus = RADIO_UART_LINE_NONE;

		if ( g_radioUartDebugLevel >= RU_DEBUG_LVL1 )
		{
			puts( "RX:" );
			puts( lineTemp );
		}

		// PROCESS LINE
		if ( lineTemp[0] == 'D' && lineTemp[1] == 'M' && lineTemp[2] == ':' )
		{
			// INCOMING DATA MESSAGE
			uint8_t checksum = 0;
			uint16_t pos = 0;
			uint16_t outpos = 0;
			uint8_t value;

			for ( pos = 3; pos < MAX_LINE_LENGTH; pos+=2 )
			{
				if ( lineTemp[pos] == ',' )
					break;
				else if ( lineTemp[pos] == '\0' || lineTemp[pos+1] == '\0' )
				{
					pos = MAX_LINE_LENGTH;
					break;
				}
			
				value = (hex_char_to_int( lineTemp[pos] ) << 4) | hex_char_to_int( lineTemp[pos+1] );

				lineTemp[outpos++] = value;
				checksum += value;
			}

			if ( pos == MAX_LINE_LENGTH )
			{
				// ERROR
				if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
					puts( "ERR: DM SYNTAX ERROR" );
			}
			else
			{
				// READ checksum
				pos++;

				if ( lineTemp[pos] == '\0' || lineTemp[pos+1] == '\0' )
				{	
					// ERROR
					if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
						puts( "ERR: DM NO CHECKSUM" );
				}
				else
				{
					uint8_t compareChecksum;

					compareChecksum = (hex_char_to_int(lineTemp[pos]) << 4) | hex_char_to_int(lineTemp[pos+1]);

					if ( compareChecksum != checksum )
					{
						// ERROR
						if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
							puts( "ERR: DM BAD CHECKSUM" );

					}
					else
					{
						// RECV and process a data message!!!
						recv_data_message( lineTemp, outpos );
					}	
				}
			}
				
		}
		else if ( lineTemp[0] == 'R' && lineTemp[1] == 'M' && lineTemp[2] == ':' )
		{
			// INCOMING RESPONSE MESSAGE
		}
		else if ( lineTemp[0] == 'R' && lineTemp[1] == 'S' && lineTemp[2] == ':' )
		{
			// INCOMING RADIO STATE UPDATE
			// PARSE the state #
			uint8_t stateNum = lineTemp[3] - '0';

			if ( stateNum < RU_STATE_MAX )
			{
				g_radioState = stateNum;
			
				// Update the GUI display of the radio connection state	
				if ( stateNum > RU_STATE_FINDSYNC )
					gui_update_radio_state( GUI_RADIOSTATE_CON );
				else
					gui_update_radio_state( GUI_RADIOSTATE_NOCON );

				if ( g_radioUartDebugLevel >= RU_DEBUG_LVL1 )
				{
					puts( "INF: " );

					switch( g_radioState )
					{
					case RU_STATE_OFF:
						puts( "RS=OFF" );
						break;

					case RU_STATE_FINDSYNC:
						puts( "RS=FINDSYNC" );
						break;

					case RU_STATE_WAIT:
						puts( "RS=IDLE" );
						break;

					case RU_STATE_READY_DATA:
						puts( "RS=DATA" );
						break;

					case RU_STATE_READY_RESPONSE:
						puts( "RS=RESPONSE" );
						break;
					}	
				}
			}
			else
			{
				if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
					puts( "ERR: RADIO STATE ERROR" );
			}
				
		}
		else if ( lineTemp[0] == 'E' && lineTemp[1] == 'R' && lineTemp[2] == ':' )
		{
			// ERROR
			if ( g_radioUartDebugLevel >= RU_DEBUG_LVL2 )
			{
				puts( "ERR: " );
				puts( lineTemp+3 );
			}
		}
		else if ( lineTemp[0] == 'M' && lineTemp[1] == 'Q' && lineTemp[2] == ':' )
		{
			g_radioUartSendWait = 0;
		
		}
		else if ( lineTemp[0] == 'R' && lineTemp[1] == 'Q' && lineTemp[2] == ':' )
		{
			g_radioUartSendWait = 0;
	
		}
		else if ( lineTemp[0] == 'M' && lineTemp[1] == 'F' && lineTemp[2] == ':' )
		{
			g_radioUartSendWait = 0;
		}
	}

	// Clear send wait for first send
	if ( g_radioState == RU_STATE_READY_DATA && g_radioState != g_radioLastState )
		g_radioUartSendWait = 0;
		
	if ( g_radioState == RU_STATE_READY_DATA )
	{
		uint8_t i;

		if ( g_radioUartSendWait == 0 )
		{
			for ( i = 0; i < TEAM_COUNT; i++ )
			{
				if ( g_forwardMessageQueue[i].status == FORWARD_MESSAGE_STATUS_FULL && g_forwardMessageQueue[i].state != FORWARD_MESSAGE_STATE_PENDING )
				{
#ifdef DEBUG_APP_MESSAGES
					puts( "SENDING DATA" );
#endif
				
					if ( radio_send_data_message( g_forwardMessageQueue[i].payload_data, g_forwardMessageQueue[i].data_len, g_forwardMessageQueue[i].from_tid, i ) != 0 )
						g_forwardMessageQueue[i].state = FORWARD_MESSAGE_STATE_RETRY;	
					else
					{
						// MESSAGE SENT
						g_forwardMessageQueue[i].state = FORWARD_MESSAGE_STATE_PENDING;
					}

				break;
				}
			}
		}
	}
	else if ( g_radioState != g_radioLastState && g_radioLastState == RU_STATE_READY_DATA )
	{
		uint8_t i;
		for ( i = 0; i < TEAM_COUNT; i++ )
		{
			if ( g_forwardMessageQueue[i].status == FORWARD_MESSAGE_STATUS_FULL && g_forwardMessageQueue[i].state != FORWARD_MESSAGE_STATE_NONE )
			{
				g_forwardMessageQueue[i].status = FORWARD_MESSAGE_STATUS_EMPTY;
				g_forwardMessageQueue[i].state = FORWARD_MESSAGE_STATE_NONE;
			}
		}

	}

	// Lastly update state!
	if ( g_radioState != g_radioLastState )
		g_radioLastState = g_radioState;
}

void radio_uart_set_debug( uint8_t level )
{
	if ( level < RU_DEBUG_LVL_MAX )
		g_radioUartDebugLevel = level;
}

uint8_t radio_uart_get_debug( void )
{
	return (g_radioUartDebugLevel);
}

uint8_t radio_add_forward_message( uint8_t *payload, uint8_t payload_length, uint8_t from_tid, uint8_t to_tid )
{
	if ( to_tid == g_myTeamID || to_tid >= TEAM_COUNT )
		return 0;

	if ( g_forwardMessageQueue[to_tid].status == FORWARD_MESSAGE_STATUS_EMPTY )
	{
		g_forwardMessageQueue[to_tid].data_len = payload_length;
		g_forwardMessageQueue[to_tid].from_tid = from_tid;
		g_forwardMessageQueue[to_tid].status = FORWARD_MESSAGE_STATUS_FULL;
		g_forwardMessageQueue[to_tid].state = FORWARD_MESSAGE_STATE_NONE;	

		memcpy( g_forwardMessageQueue[to_tid].payload_data, payload, payload_length );

		return 1; // GOOD
	}
}
