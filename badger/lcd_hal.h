#ifndef __LCD_HAL_H__
#define __LCD_HAL_H__

#include "hardware.h"
#include "lcd.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define LCD_PORT		(P2OUT)

#define LCD_CSN_BIT		(0x04)
#define LCD_BACKLIGHT_BIT	(0x10)
#define LCD_RS_BIT		(0x20)
#define LCD_RESET_BIT		(0x40)

void lcd_init_screen( unsigned char options );
//void lcd_setAddrWindow( unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1 );
void lcd_fillRect( unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color );
void lcd_fillScreen( unsigned int color ); 
void lcd_drawPixel( unsigned int x, unsigned int y, unsigned int color); 

void lcd_DrawSkin( void );

Display *GetX11Display( void );
void X11Redraw( void );
void X11Flush( void );

int GetX11ConnectionNumber( void );

#endif // __LCD_HAL_H__
