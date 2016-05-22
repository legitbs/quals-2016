#ifndef __PORT_HELPER_H__
#define __PORT_HELPER_H__

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <stdint.h>

void eint( void );
void dint( void );

void interrupt_handler( void );

void LEDOn( uint16_t ledBits );
void LEDOff( uint16_t ledBits );

void radio_uart_putc( char byte );

#define LPM0	;
#define LPM0_EXIT	;

#endif // __PORT_HELPER_H__
