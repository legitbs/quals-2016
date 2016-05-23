#ifndef __RADIO_UART_H__
#define __RADIO_UART_H__

#include <stdint.h>

#define RU_DEBUG_OFF		0
#define RU_DEBUG_LVL1		1
#define RU_DEBUG_LVL2		2
#define RU_DEBUG_LVL_MAX	3

// These must match the RADIO_UART_STATE_NONE... etc defines in /radio/app_uart.c
#define RU_STATE_OFF			0	// RADIO off
#define RU_STATE_FINDSYNC		1	// RADIO scanning for sync
#define RU_STATE_WAIT			2	// RADIO waiting (idle)
#define RU_STATE_READY_DATA		3	// RADIO accepting data forard messages
#define RU_STATE_READY_RESPONSE		4	// RADIO accepting response messages
#define RU_STATE_MAX			5	// MAXIMUM number of radio states

#define FORWARD_DATA_SIZE		(254)	// 86*3-4
#define FRESPONSE_DATA_SIZE		(82)	// 86-4

void radio_init_uart( void );
void radio_uart_write( uint8_t *data, uint16_t dataLen );
uint8_t radio_send_response_message( uint8_t *pData, uint8_t dataLen, uint16_t session_id, uint8_t from_tid );
uint8_t radio_send_data_message( uint8_t *pData, uint8_t dataLen, uint8_t from_tid, uint8_t to_tid );

void radio_uart_run( void );

void radio_uart_set_debug( uint8_t level );
uint8_t radio_uart_get_debug( void );

uint8_t radio_add_forward_message( uint8_t *payload, uint8_t payload_length, uint8_t from_tid, uint8_t to_tid );

void INT_radio_uart_rx( uint8_t in_char );

#endif // __RADIO_UART_H__
