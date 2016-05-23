#ifndef __CLI_H__
#define __CLI_H__

#include <stdint.h>

void cli_init_uart( void );
void cli_run( void );

void cli_process_command( uint8_t *pCommand );

void cli_command_help( uint8_t *pArgs );
void cli_command_msg( uint8_t *pArgs );
void cli_command_token( uint8_t *pArgs );
void cli_command_debug( uint8_t *pArgs );
void cli_command_id( uint8_t *pArgs );

void INT_cli_uart_rx( uint8_t rxdata );

#endif // __CLI_H__
