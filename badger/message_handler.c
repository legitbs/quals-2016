#include "message_handler.h"
#include "util.h"
#include "lzssvw.h"
#include "gui.h"
#include "bit_util.h"
#include "radio_uart.h"
#include "hardware.h"
#include <stdio.h>
#include <string.h>

extern volatile uint8_t g_radioState;

volatile tSendForwardData g_forwardMessageQueue[TEAM_COUNT];

extern volatile uint8_t g_myTeamID;

#define COMMAND_SILENT_MSG		1
#define COMMAND_CRC16_IMAGE		2
#define COMMAND_GET_INFO		3
#define COMMAND_GET_TEAM		4
#define COMMAND_CRC16_TEXT		5
#define COMMAND_POLL_STATUS		6

#define POLY 0x1021
/*
//                                      16   12   5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021
*/
uint16_t crc16(uint8_t *data_p, uint8_t *data_end)
{
	uint16_t crc = 0xFFFF;
	char *data_start = data_p;

	if ( data_end < data_p )
	{
		uint8_t *temp = data_p;
		data_p = data_end;
		data_end = temp;
	}
        else if ( data_p == data_end )
            return (~crc);


	while ( data_p < data_end )
	{
		crc ^= *data_p++ << 8;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
	}

	return crc;
}

void add_command_response( tCommandResponseState *state, uint8_t command_num, uint8_t *data, uint8_t data_len )
{
	if ( (state->pos + 1 + data_len) < 86-4 )
	{
		// Add...
		state->command_response_buffer[state->pos++] = command_num;
		
		memcpy( state->command_response_buffer + state->pos, data, data_len );

		state->pos += data_len;
	}
}

uint8_t message_process_command( tRenderedMessage *pRenderedMessage, tCommandResponseState *state, uint8_t *commands, uint8_t data_len, uint8_t *command_num, uint8_t from_tid )
{
	uint8_t pos = 0;
	uint8_t command_len;

	if ( data_len < 2 )
		return (0xFF);

	command_len = commands[0] & 0x3F;
	command_len++;

	if ( command_len > data_len )
		return (0xFF);

	for ( pos = 1; pos < command_len; )
	{
		switch( (commands[pos] >> 4) )
		{
		case COMMAND_SILENT_MSG:
			{
				uint8_t command_response = 1;	
				
				pRenderedMessage->render_type = RENDER_TYPE_SILENT;
			
				add_command_response( state, (*command_num), &command_response, 1 ); 
			}	
			pos++;
			break;

		case COMMAND_CRC16_IMAGE:
			if ( (pos+4) < command_len )
			{
				uint16_t crc16_value;
				int16_t image_start_pos = ((commands[pos+2] << 8) | (commands[pos+1]));
				int16_t image_end_pos = ((commands[pos+4] << 8) | (commands[pos+3]));

			
				// BUG: Had to be modified slightly for port -- this use
				// to wrap around the 16-bit address space of the badge hardware (MSP430) so you could read the flag from the memory mapped peripheral	
				// Limit image_len to maximum size of the image
				//if ( image_start_pos >= 800 )
				//	image_start_pos = 800;
				//if ( image_end_pos >= 800 )
				//	image_end_pos = 800;

				crc16_value = crc16( (pRenderedMessage->image_data + image_start_pos), (pRenderedMessage->image_data + image_end_pos) );

				add_command_response( state, (*command_num), (uint8_t*)&crc16_value, 2 );
			}
			pos+=5;
			break;

		case COMMAND_GET_INFO:
			{
				uint8_t radioState = g_radioState;

				add_command_response( state, (*command_num), &radioState, 1 );	
			}	
			pos++;
			break;

		case COMMAND_GET_TEAM:
			{
				uint8_t team_info[2];
		
				team_info[0] = g_myTeamID;
				team_info[1] = from_tid;
				
				add_command_response( state, (*command_num), team_info, 2 ); 
			}	
			pos++;
			break;

		case COMMAND_CRC16_TEXT:
			if ( (pos+1) < command_len )
			{
				uint16_t crc16_value;
				uint8_t text_len = (commands[pos+1]);

				// Limit image_len to maximum size of the image
				if ( text_len >= MAX_TEXT_CHARACTERS )
					text_len = MAX_TEXT_CHARACTERS;

				crc16_value = crc16( pRenderedMessage->text_data, (pRenderedMessage->text_data + text_len) );

				add_command_response( state, (*command_num), (uint8_t*)&crc16_value, 2 );
			}
			pos+=2;
			break;

		case COMMAND_POLL_STATUS:
			{
				uint16_t status[4];

				/* PORT: Removed because we do not have this memory mapped
				status[0] = POLL_STATUS_0;
				status[1] = POLL_STATUS_1;
				status[2] = POLL_STATUS_2;
				status[3] = POLL_STATUS_3;
				*/
				status[0] = 0x6953;
				status[1] = 0x4772;
				status[2] = 0x6f6f;
				status[3] = 0x006e;
				// ((char *)status) = "SirGoon";

				add_command_response( state, (*command_num), (uint8_t*)&status, 8 );
			}
			pos++;
			break;

		// TODO: Add more command handlers
		
		default:
			return (pos);
			break;

		}
		
		(*command_num)++;
	}

	return (pos);
}

uint16_t process_data_message( uint8_t *pMessage, uint16_t message_length, tCommandResponseState *pCommandResponse, uint16_t *session_id )
{
	uint8_t payload_data_length;
	uint16_t pos=0;
	uint8_t process_fail;
	uint8_t *pData;
	uint8_t base_response[1];
	char pOutput[8];
	uint8_t command_num;
	tReceiveForwardData *pForwardData;
	tRenderedMessage oRenderedMessage;
	uint16_t address;

	// Clear command response
	pCommandResponse->pos = 0;

	if ( message_length > sizeof(tReceiveForwardData) )
		return 0;

	if ( message_length < 4 )
		return (0);


	// Process data message
	pForwardData = (tReceiveForwardData *)pMessage;

	if ( message_length != pForwardData->data_len+4 )
		return (0);


#ifdef DEBUG_APP_MESSAGES
	// DEBUG
	address = (uint16_t)(oRenderedMessage.image_data);	
	pOutput[0] = int_to_hex_char( ((address >> 12) & 0xF) );
        pOutput[1] = int_to_hex_char( ((address >> 8) & 0xF) );
        pOutput[2] = int_to_hex_char( ((address >> 4) & 0xF) );
        pOutput[3] = int_to_hex_char( ((address) & 0xF) );
	pOutput[4] = '\0';
	puts( "DBG ADDR" );
	puts( pOutput );
#endif


	// Store session ID and From TID
	oRenderedMessage.session_id = pForwardData->session_id;	
	oRenderedMessage.from_tid = pForwardData->from_tid;

#ifdef DEBUG_APP_MESSAGES
	// DEBUG
	pOutput[0] = int_to_hex_char( ((pForwardData->from_tid>>4) & 0xF) );
        pOutput[1] = int_to_hex_char( ((pForwardData->from_tid) & 0xF) );
	pOutput[2] = '\0';

	puts( pOutput );
#endif

	// Setup render (incase we don't have these sections)
	oRenderedMessage.render_type = RENDER_TYPE_NORMAL;
	oRenderedMessage.text_character_count = 0;
	oRenderedMessage.image_type = RENDER_IMAGE_TYPE_NONE;
	oRenderedMessage.image_x_size = 0;
	oRenderedMessage.image_y_size = 0;
	
	// Process!
	payload_data_length = pForwardData->data_len;
	pData = pForwardData->payload_data;

	// Send a default response...
	base_response[0] = g_myTeamID;
	add_command_response( pCommandResponse, 0, base_response, 1 );

	command_num = 1;	
	// PROCESS data
	process_fail = 0;
	for ( pos = 0; pos < payload_data_length; )
	{
		// READ first byte...
		uint8_t message_type_header = pData[pos];
		uint8_t part_length;

		switch( (message_type_header & MESSAGE_TYPE_MASK) )
		{
		case MESSAGE_TYPE_IMAGE:
#ifdef DEBUG_APP_MESSAGES
			puts("img");
#endif
			// PROCESS IMAGE
			part_length = message_process_image( &oRenderedMessage, pData+pos, payload_data_length-pos );
			break;
			
		case MESSAGE_TYPE_TEXT:
#ifdef DEBUG_APP_MESSAGES
			puts("txt");
#endif
			// PROCESS TEXT
			part_length = message_process_text( &oRenderedMessage, pData+pos, payload_data_length-pos );
			break;	

		case MESSAGE_TYPE_COMMAND:
#ifdef DEBUG_APP_MESSAGES
			puts("cmd");
#endif
			part_length = message_process_command( &oRenderedMessage, pCommandResponse, pData+pos, payload_data_length-pos, &command_num, pForwardData->from_tid );
			break;

		default:
#ifdef DEBUG_APP_MESSAGES
			puts("unk");
#endif
			part_length = 0xFF;	
			break;
		}

		if ( part_length == 0xFF )
		{
#ifdef DEBUG_APP_MESSAGES
			puts("fail");
#endif
			process_fail = 1;
			break;
		}

		if ( part_length == 0 )
		{
			process_fail = 1;
			break;
		}

		pos += part_length;
	}

	if ( oRenderedMessage.render_type == RENDER_TYPE_NORMAL && process_fail == 0 )
	{
		// Save message and inform them they got a new message~
		write_message_to_sram( &oRenderedMessage );
	}

	// Set the session ID for the parent function for sending response
	(*session_id) = oRenderedMessage.session_id;

	return (process_fail);
}

void recv_data_message( uint8_t *pMessage, uint16_t message_length )
{
	tCommandResponseState oCommandResponse;
	uint16_t session_id;

	if ( !process_data_message( pMessage, message_length, &oCommandResponse, &session_id ) )
	{
		// Now write out the command response (if it exists)		
		if ( oCommandResponse.pos > 0 )
		{
			radio_send_response_message( oCommandResponse.command_response_buffer, oCommandResponse.pos, session_id, g_myTeamID );
		}
	}
}

uint8_t message_process_image( tRenderedMessage *pRenderedMessage, uint8_t *payload, uint8_t payload_length )
{
	uint8_t image_x_size;
	uint8_t image_y_size;
	uint8_t image_type;
	uint8_t image_data_length;

	if ( payload_length < 2 )
		return 0xFF;

	// Begin parsing header
	image_x_size = (payload[0] & IMAGE_HEADER_X_SIZE_MASK) >> IMAGE_HEADER_X_SIZE_SHIFT;
	image_y_size = (payload[0] & IMAGE_HEADER_Y_SIZE_MASK) >> IMAGE_HEADER_Y_SIZE_SHIFT;

	image_x_size = 28 + (image_x_size * 4);

	image_y_size = 28 + (image_y_size * 4);

	if ( payload[0] & IMAGE_HEADER_TYPE_MASK )
		image_type = RENDER_IMAGE_TYPE_COLOR;
	else
		image_type = RENDER_IMAGE_TYPE_BW;

	// NOW DECOMPRESS	
	image_data_length = payload[1];

	// CHECK SIZE!!!
	if ( image_data_length > payload_length-2 )
		return (0xFF);	// FAIL PARSING!!!

	// OPTIONALLY decompress!
	if ( payload[0] & IMAGE_HEADER_COMPRESSION_MASK )
	{
		// Decompress!
		lzssvw_decompress( payload+2, pRenderedMessage->image_data, image_data_length, (image_x_size*image_y_size) );
	}
	else
		memcpy( pRenderedMessage->image_data, payload+2, image_data_length );

	// STORE image data information
	pRenderedMessage->image_x_size = image_x_size;
	pRenderedMessage->image_y_size = image_y_size;

	pRenderedMessage->image_type = image_type;

	return (image_data_length+2);	 
}

uint8_t message_process_text( tRenderedMessage *pRenderedMessage, uint8_t *payload, uint8_t payload_length )
{
	uint8_t read_payload_length;
	uint8_t character_count;

	if ( payload_length < 2 )
		return 0xFF;

	// Read character count
	character_count = payload[1];

	// Process text (based on charset)
	if ( payload[0] & TEXT_HEADER_CHARSET_MASK )
	{
		uint8_t charPos;
		uint16_t data_len;
		tBitState bit_state;
		// Process 7bit charset

		// Calculate data_len
		data_len = (character_count*7);
		
		if ( data_len % 8 != 0 )
		{
			data_len = (data_len / 8);
			data_len++;
		}
		else
			data_len = (data_len / 8);

		// Check data length
		if ( data_len > (payload_length-2) )
			return (0xFF);

		// Remember read payload length
		read_payload_length = data_len;

		text_init_state( &bit_state, payload+2 );
		for ( charPos = 0; charPos < character_count; charPos++ )
		{
			int16_t data_char;

			data_char = text_getbit( 7, &bit_state, data_len );

			if ( data_char == -1 )
				return (0xFF);

			pRenderedMessage->text_data[charPos] = (uint8_t)(data_char & 0x7F);
		}
	}
	else
	{
		// Process 6bit charset
		uint8_t charPos;
		uint16_t data_len;
		tBitState bit_state;

		// Calculate data_len
		data_len = (character_count*6);
		
		if ( data_len % 8 != 0 )
		{
			data_len = (data_len / 8);
			data_len++;
		}
		else
			data_len = (data_len / 8);

		// Check data length
		if ( data_len > (payload_length-2) )
			return (0xFF);

		// Remember read payload length
		read_payload_length = data_len;

		text_init_state( &bit_state, payload+2 );
		for ( charPos = 0; charPos < character_count; charPos++ )
		{
			int16_t data_char;

			data_char = text_getbit( 6, &bit_state, data_len );

			if ( data_char == -1 )
				return (0xFF);

			if ( data_char < 26 )
				pRenderedMessage->text_data[charPos] = 'a'+data_char;
			else if ( data_char < 52 )
				pRenderedMessage->text_data[charPos] = 'A'+(data_char-26);
			else if ( data_char < 62 )
				pRenderedMessage->text_data[charPos] = '0'+(data_char-52);
			else if ( data_char == 62 )
				pRenderedMessage->text_data[charPos] = ' ';
			else
				pRenderedMessage->text_data[charPos] = ',';
		}
	}

	pRenderedMessage->text_character_count = character_count;
	pRenderedMessage->text_color = (payload[0] & TEXT_HEADER_COLOR_MASK);

	return (read_payload_length+2);	
}

void recv_response_message( uint8_t *pMessage, uint16_t message_length )
{
	// DO NOTHING
}

uint8_t get_message_queue_available_at( uint8_t team_id )
{
	if ( team_id >= TEAM_COUNT )
		team_id = TEAM_COUNT-1;

	return (g_forwardMessageQueue[team_id].status);
}
