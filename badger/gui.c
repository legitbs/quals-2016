#include "port_helper.h"
#include "gui.h"
//#include "omsp_system.h"
#include "hardware.h"
#include "lcd_hal.h"
#include "lcd_gfx.h"
#include "sram_hal.h"
#include "util.h"
#include "bit_util.h"
#include "radio_uart.h"
#include "lzssvw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPLASH_STATUS_X_LOC	(29)
#define SPLASH_STATUS_Y_LOC	(32)

#define SPLASH_IMAGE_BOX_X_LOC	(119)
#define SPLASH_IMAGE_BOX_Y_LOC	(40)

#define COLOR_LEGIT_ORANGE	(0xFBE0) // DCE9 possible??
#define COLOR_LEGIT_WHITE	(0xFFFF)
#define COLOR_LEGIT_PURPLE	(0xF07F)

volatile tMessageSramInfo g_messageTable[MAX_SAVED_MESSAGES];

volatile uint8_t g_messageCount;	// Number of received messages
volatile uint8_t g_messageCur;		// Currently displayed message
volatile uint8_t g_messageLast;		// Last message received!
volatile uint8_t g_guiLastState; // Last state of the GUI
volatile uint8_t g_guiState;	// State of the GUI (modified by button interrupts)

volatile uint8_t g_updateMenu;	// Set to 1 to update the menu

volatile uint8_t g_guiRadioState;	// State of the radio (connected or not connected)

// Tracks message composition selections
volatile uint8_t g_composeTeamPos;		// The chose team #
volatile uint8_t g_composeImagePos;		// The chosen image #
volatile uint8_t g_composeCharTotal;		// The total number of text characters there can be
volatile uint8_t g_composeCharPos;		// The position of the text character 
volatile uint8_t g_composeCharSelected;		// The currently selected text character
volatile uint8_t g_composeTextBuffer[201];	// The text buffer for the composed message

volatile uint8_t g_guiDelayCounter;
volatile uint8_t button_next, button_up, button_down, button_back;

volatile uint8_t g_renderImageBuffer[800];
volatile uint8_t g_renderImageType;
volatile uint8_t g_renderImageXSize;
volatile uint8_t g_renderImageYSize;

volatile uint8_t g_ledFlashTimer = 0;

volatile uint16_t g_buttonBit = 0;

extern volatile uint8_t g_myTeamID;

const char *g_teamNameTable[TEAM_COUNT] = {
"ppp",			// 0
"team9447",		// 1
"reckless",		// 2
"routards",		// 3
"raon_asrt",		// 4
"kaist",		// 5
"shellphish",		// 6
"codered",		// 7
"hitcon",		// 8
"blue-lotus",		// 9
"hfcm",			// 10
"mmibh",		// 11
"w3stormz",		// 12
"mslc",			// 13
"dragonsect",		// 14
"penthackon",		// 15
"stratum",		// 16
"gallopsled",		// 17
"balacr3w",		// 18
"binja"			// 19
};

const uint16_t g_colorPalette[16] = {
0x1864,
0x4927,
0x31AE,
0x526A,
0x8A66,
0x3B25,
0xD249,
0x7B8C,
0x5BFA,
0xD3E6,
0x8CB4,
0x7566,
0xD573,
0x7639,
0xDEAC,
0xE79B
};

/* dick_butt_40x40_bw_comp.raw
   StartOffset: 00000000, EndOffset: 000000A4, Length: 000000A5 */

const uint8_t g_imageDickButt_40x40_bw[165] = {
	0xFF, 0x84, 0x83, 0xB0, 0xBF, 0xDF, 0xB5, 0x8B, 0xFB, 0xFD, 0x40, 0x5F,
	0xBF, 0xF2, 0xA2, 0xFB, 0x88, 0x4F, 0x1D, 0xFC, 0x48, 0xB9, 0xB7, 0xF3,
	0x41, 0xF1, 0x84, 0x0F, 0xD7, 0xFF, 0xCD, 0xEE, 0xDF, 0xFE, 0xC1, 0x03,
	0xE3, 0xDF, 0x84, 0x08, 0x80, 0x84, 0x17, 0x8B, 0xFE, 0x24, 0x5E, 0xEB,
	0xB8, 0x70, 0xBD, 0xFC, 0xFE, 0xFF, 0xB8, 0x24, 0x33, 0xF0, 0x40, 0xCF,
	0xFC, 0x3F, 0xDB, 0x74, 0x3B, 0xBF, 0xEF, 0xFE, 0xDB, 0x7D, 0xFD, 0xFF,
	0xBF, 0xF6, 0xE7, 0xE0, 0xF7, 0x82, 0x07, 0x7F, 0xDF, 0xB7, 0xA5, 0x07,
	0x7F, 0xBF, 0xCF, 0xA7, 0x87, 0xBE, 0x66, 0x16, 0x20, 0xD7, 0xCC, 0xC2,
	0xCE, 0x1B, 0x79, 0xC0, 0x59, 0xC3, 0x77, 0xFF, 0xDF, 0xDB, 0x03, 0x5B,
	0xF7, 0xDF, 0xCD, 0x81, 0xA3, 0xFD, 0xC0, 0x82, 0xDF, 0xFE, 0x91, 0x41,
	0x36, 0x04, 0xFC, 0x2D, 0xF9, 0xAC, 0x29, 0x40, 0xF7, 0xFF, 0xF9, 0xF4,
	0xC8, 0x7C, 0xFF, 0xE1, 0x40, 0x7F, 0xFF, 0x88, 0x0E, 0xBD, 0x46, 0x2A,
	0xFE, 0xDC, 0x08, 0x3E, 0xB8, 0x24, 0x7D, 0xB0, 0x71, 0xBF, 0x9D, 0xFE,
	0x7A, 0xA8, 0x42, 0x80, 0x82, 0x45, 0x1F, 0x62, 0x84
};


/* smile_40x40_bw_comp.raw
   StartOffset: 00000000, EndOffset: 000000BA, Length: 000000BB */

const uint8_t g_imageSmile_40x40_bw[187] = {
	0xFF, 0xFF, 0xE0, 0x08, 0x3F, 0xFF, 0xE4, 0x02, 0x7E, 0x80, 0xF8, 0x20,
	0x10, 0x76, 0x87, 0x81, 0x00, 0x81, 0xA4, 0x20, 0x03, 0xFF, 0xFF, 0x40,
	0x23, 0x90, 0x0B, 0xFA, 0xE1, 0xBF, 0x99, 0x0F, 0xE2, 0x02, 0x54, 0x31,
	0xFF, 0x81, 0x60, 0xF8, 0x21, 0xFE, 0x04, 0x01, 0xF8, 0x41, 0xC2, 0x07,
	0x3D, 0x70, 0x83, 0xF0, 0x21, 0xF0, 0xC9, 0xC4, 0x0F, 0x85, 0x0F, 0xCF,
	0x5E, 0x24, 0x78, 0x78, 0x7F, 0xFF, 0xF1, 0x71, 0xC3, 0xC6, 0x7F, 0xFC,
	0xCB, 0x8C, 0x3F, 0x1B, 0xFF, 0xEC, 0x5C, 0x23, 0xFC, 0xDF, 0xFF, 0x67,
	0xF1, 0x1F, 0xF2, 0x7F, 0xF3, 0x3F, 0xC8, 0x67, 0xDA, 0xFF, 0x33, 0xCE,
	0x40, 0x27, 0x73, 0xEB, 0x7C, 0x62, 0x01, 0x0D, 0xC0, 0x77, 0x14, 0x83,
	0x06, 0xFF, 0xEE, 0x15, 0xC2, 0x94, 0x25, 0xC1, 0x2A, 0x04, 0x10, 0xD0,
	0x3F, 0xB9, 0xD0, 0x38, 0x08, 0x7F, 0x04, 0xC2, 0x1E, 0x34, 0x7F, 0x01,
	0xFC, 0xB1, 0xF3, 0xEF, 0xD0, 0x09, 0xF7, 0xCF, 0xCF, 0xF8, 0x80, 0x43,
	0xFC, 0xFE, 0x3F, 0x82, 0x2C, 0x3C, 0x7F, 0x82, 0x40, 0x30, 0x3C, 0x7F,
	0xE0, 0x08, 0x2C, 0xFF, 0xF1, 0x34, 0x18, 0x0E, 0x7F, 0xFD, 0x40, 0x1E,
	0x85, 0x4A, 0x11, 0xE8, 0xFF, 0xF8, 0x02, 0x38, 0x95, 0x25, 0x16, 0x06,
	0x3E, 0xC1, 0x0A, 0xA8, 0x21, 0x20, 0x80
};


/* pound_36x36_4p_comp.raw
   StartOffset: 00000000, EndOffset: 000000AE, Length: 000000AF */

const uint8_t g_imagePound_36x36_4p[175] = {
	0x91, 0x04, 0xF7, 0x78, 0x8E, 0x39, 0x77, 0xB9, 0xA5, 0x28, 0x85, 0x8C,
	0x51, 0x12, 0x40, 0xA1, 0x24, 0x11, 0x15, 0x48, 0xC8, 0x94, 0x5C, 0xA6,
	0x87, 0x11, 0x94, 0xC8, 0x5C, 0xED, 0x44, 0xA7, 0x29, 0x11, 0x61, 0xDE,
	0x33, 0x55, 0xBC, 0x28, 0xF3, 0x5D, 0x50, 0xE2, 0xF5, 0xA5, 0x09, 0xE4,
	0x54, 0x0D, 0x2C, 0x30, 0x11, 0x12, 0x83, 0x05, 0x8F, 0xC4, 0x10, 0x38,
	0x1D, 0x22, 0x34, 0x0A, 0xCE, 0xE1, 0x15, 0x09, 0x9E, 0x65, 0x92, 0xA4,
	0x25, 0xE9, 0x4E, 0x71, 0x44, 0x8E, 0x84, 0xA1, 0x82, 0x37, 0x06, 0x5C,
	0x05, 0x0B, 0x40, 0x22, 0x64, 0x75, 0x41, 0xA9, 0x93, 0x04, 0x02, 0x3F,
	0x4C, 0x0A, 0x2C, 0x9B, 0x04, 0x96, 0x9C, 0xB7, 0x54, 0x45, 0x62, 0x21,
	0x8B, 0xF6, 0x9C, 0xDE, 0x56, 0xD8, 0x51, 0xE4, 0x78, 0x3B, 0x26, 0xF4,
	0x32, 0x76, 0xB8, 0xC8, 0xB9, 0x58, 0x93, 0x82, 0x33, 0x79, 0x0A, 0x11,
	0xAE, 0x4A, 0x11, 0xC9, 0x72, 0x1B, 0xB2, 0x10, 0xD5, 0x4F, 0x88, 0x5F,
	0xD2, 0x88, 0xE9, 0x70, 0x82, 0x08, 0xBD, 0xF8, 0x0D, 0x48, 0x17, 0x42,
	0xAE, 0x74, 0xF4, 0x61, 0xD9, 0x5F, 0x92, 0xC9, 0xDC, 0x11, 0xBF, 0x02,
	0xB9, 0x12, 0x0E, 0x7F, 0xC2, 0xFB, 0x10
};

/* dealwithit-smug-36x36_4p_comp.raw
   StartOffset: 00000000, EndOffset: 000000AF, Length: 000000B0 */

const uint8_t g_imageDealWithIt_36x36_4p[176] = {
	0xDD, 0x9F, 0x41, 0xF4, 0x0F, 0xB0, 0x7D, 0x01, 0xF5, 0x07, 0xD8, 0x1B,
	0xDA, 0x51, 0x00, 0x04, 0xA5, 0x84, 0x15, 0xCC, 0x00, 0x10, 0x24, 0x71,
	0x00, 0x81, 0x02, 0x82, 0x42, 0x0C, 0x4B, 0x4C, 0x03, 0xCE, 0x14, 0x07,
	0x88, 0x1B, 0x93, 0x63, 0x11, 0x00, 0x16, 0x25, 0x0C, 0x54, 0x14, 0x02,
	0x61, 0x30, 0x80, 0x00, 0x02, 0x81, 0xC0, 0xC0, 0x51, 0x02, 0x02, 0xA4,
	0x86, 0xA1, 0xE0, 0x01, 0x63, 0x86, 0x73, 0x01, 0x81, 0xC1, 0x06, 0xF0,
	0xD2, 0x73, 0x22, 0x38, 0x82, 0x11, 0x8D, 0xC1, 0xE0, 0x00, 0xA3, 0x04,
	0x4C, 0x05, 0x88, 0x22, 0x09, 0x02, 0x1B, 0xBD, 0xDE, 0xF8, 0x2E, 0x04,
	0x23, 0x28, 0x16, 0x21, 0xC1, 0x14, 0xB0, 0xD4, 0x44, 0x72, 0x01, 0x13,
	0xD1, 0x38, 0x85, 0x01, 0xB8, 0x13, 0x39, 0x81, 0x26, 0x30, 0x45, 0x43,
	0x60, 0x44, 0xC1, 0x82, 0x22, 0xCA, 0x02, 0x28, 0xE0, 0x16, 0xE4, 0x04,
	0x45, 0xCC, 0x31, 0xCE, 0x1F, 0x12, 0x89, 0xB4, 0x23, 0xC5, 0xC4, 0x36,
	0x8D, 0x4C, 0x38, 0x84, 0x83, 0x04, 0xC5, 0x09, 0xC2, 0x30, 0x10, 0xC0,
	0x38, 0x68, 0x87, 0xB8, 0x51, 0xA3, 0xA6, 0x62, 0x21, 0x18, 0x06, 0x7C,
	0x2C, 0x71, 0x3A, 0x81, 0x13, 0xE1, 0xAB, 0x40
};



#define IMAGE_LOOKUP_TABLE_COUNT	(5)
const tImageLookupData g_imageLookupTable[IMAGE_LOOKUP_TABLE_COUNT] = 
{
	{ RENDER_IMAGE_TYPE_NONE, 0, 0, 0, 0, NULL },
	{ RENDER_IMAGE_TYPE_BW, 1, 40, 40, sizeof(g_imageSmile_40x40_bw), g_imageSmile_40x40_bw },
	{ RENDER_IMAGE_TYPE_BW, 1, 40, 40, sizeof(g_imageDickButt_40x40_bw), g_imageDickButt_40x40_bw },
	{ RENDER_IMAGE_TYPE_COLOR, 1, 36, 36, sizeof(g_imageDealWithIt_36x36_4p), g_imageDealWithIt_36x36_4p },
	{ RENDER_IMAGE_TYPE_COLOR, 1, 36, 36, sizeof(g_imagePound_36x36_4p), g_imagePound_36x36_4p },
};

/*
wakeup interrupt (TIMERA0_VECTOR) TimerA_Interrupt( void )
{
	static uint8_t timerCount = 0;

	if ( timerCount == 0 )
		run_gui();

	timerCount++;
	if ( timerCount == 5 )
		timerCount = 0;
	
	LPM0_EXIT;
}
*/

//----------------------------------------------------//
// PORT1 Interrupt -- Button Depress
//----------------------------------------------------//
/* PORT CODE
 * TODO: MAKE BUTTONS WORK
interrupt (PORT1_VECTOR) INT_button(void)
{
    if ( BUTTON_FLAG & BUTTON_NEXT_BIT )
    {
        button_next =1;
        BUTTON_FLAG ^= BUTTON_NEXT_BIT;
    }
    if (BUTTON_FLAG & BUTTON_DOWN_BIT){
        button_down = 1;
        BUTTON_FLAG ^= BUTTON_DOWN_BIT;
    }
    if ( BUTTON_FLAG & BUTTON_UP_BIT ){
        button_up = 1;
        BUTTON_FLAG ^= BUTTON_UP_BIT;
    }
    if (BUTTON_FLAG & BUTTON_BACK_BIT){
        button_back = 1;
        BUTTON_FLAG ^= BUTTON_BACK_BIT;
    }
}
*/
void SetButtonUp( void )
{
	button_up = 1;
	g_buttonBit |= BUTTON_UP_BIT;
}

void SetButtonDown( void )
{
	button_down = 1;
	g_buttonBit |= BUTTON_DOWN_BIT;
}

void SetButtonNext( void )
{
	button_next = 1;
	g_buttonBit |= BUTTON_NEXT_BIT;
}

void SetButtonBack( void )
{
	button_back = 1;
	g_buttonBit |= BUTTON_BACK_BIT;
}

void ReleaseButtonUp( void )
{
	g_buttonBit &= ~BUTTON_UP_BIT;
}

void ReleaseButtonDown( void )
{
	g_buttonBit &= ~BUTTON_DOWN_BIT;
}

void ReleaseButtonNext( void )
{
	g_buttonBit &= ~BUTTON_NEXT_BIT;
}

void ReleaseButtonBack( void )
{
	g_buttonBit &= ~BUTTON_BACK_BIT;
}

void gui_draw_lines( tLinePoint *line_array, uint8_t line_count, uint16_t color )
{
	uint8_t i = 0;
	for ( i = 0; i < line_count; i++ )
		lcd_gfx_drawLine( line_array[i].start_x, line_array[i].start_y, line_array[i].end_x, line_array[i].end_y, color );
}

void gui_draw_status_border( void )
{
	static const tLinePoint border_lines[10] = {
	{ SPLASH_STATUS_X_LOC-1, 0, SPLASH_STATUS_X_LOC-1, 127 },
	{ SPLASH_STATUS_X_LOC, 0, SPLASH_STATUS_X_LOC, 127 },
	{ 0, SPLASH_STATUS_Y_LOC, SPLASH_STATUS_X_LOC-1, SPLASH_STATUS_Y_LOC },
	{ 0, SPLASH_STATUS_Y_LOC-1, SPLASH_STATUS_X_LOC-1, SPLASH_STATUS_Y_LOC-1 },
	{ 0, (SPLASH_STATUS_Y_LOC*2)-1, SPLASH_STATUS_X_LOC-1, (SPLASH_STATUS_Y_LOC*2)-1 },
	{ 0, (SPLASH_STATUS_Y_LOC*2), SPLASH_STATUS_X_LOC-1, (SPLASH_STATUS_Y_LOC*2) },
	{ 0, (SPLASH_STATUS_Y_LOC*3)-1, SPLASH_STATUS_X_LOC-1, (SPLASH_STATUS_Y_LOC*3)-1 },
	{ 0, (SPLASH_STATUS_Y_LOC*3), SPLASH_STATUS_X_LOC-1, (SPLASH_STATUS_Y_LOC*3) },
	{ SPLASH_IMAGE_BOX_X_LOC, 0, SPLASH_IMAGE_BOX_X_LOC, SPLASH_IMAGE_BOX_Y_LOC-1 },
	{ SPLASH_IMAGE_BOX_X_LOC, SPLASH_IMAGE_BOX_Y_LOC, SPLASH_IMAGE_BOX_X_LOC+40, SPLASH_IMAGE_BOX_Y_LOC }
	};

	gui_draw_lines( border_lines, 10, ST7735_BLACK );
	/*
	// DRAW status border
	lcd_gfx_drawFastVLine( SPLASH_STATUS_X_LOC-1, 0, 128, ST7735_BLACK );
	lcd_gfx_drawFastVLine( SPLASH_STATUS_X_LOC, 0, 128, ST7735_BLACK );

	lcd_gfx_drawFastHLine( 0, SPLASH_STATUS_Y_LOC, SPLASH_STATUS_X_LOC, ST7735_BLACK );  	
	lcd_gfx_drawFastHLine( 0, SPLASH_STATUS_Y_LOC-1, SPLASH_STATUS_X_LOC, ST7735_BLACK );  	
	
	lcd_gfx_drawFastHLine( 0, (SPLASH_STATUS_Y_LOC*2)-1, SPLASH_STATUS_X_LOC, ST7735_BLACK );  	
	lcd_gfx_drawFastHLine( 0, SPLASH_STATUS_Y_LOC*2, SPLASH_STATUS_X_LOC, ST7735_BLACK );  	
	
	lcd_gfx_drawFastHLine( 0, (SPLASH_STATUS_Y_LOC*3)-1, SPLASH_STATUS_X_LOC, ST7735_BLACK );  	
	lcd_gfx_drawFastHLine( 0, SPLASH_STATUS_Y_LOC*3, SPLASH_STATUS_X_LOC, ST7735_BLACK );  

	// DRAW image box border
	lcd_gfx_drawFastVLine( SPLASH_IMAGE_BOX_X_LOC, 0, SPLASH_IMAGE_BOX_Y_LOC, ST7735_BLACK );
	lcd_gfx_drawFastHLine( SPLASH_IMAGE_BOX_X_LOC, SPLASH_IMAGE_BOX_Y_LOC, 41, ST7735_BLACK );
	*/
}

void init_gui( void )
{
	g_messageCount = 0;
	g_messageCur = 0;
	g_messageLast = 0;
	g_guiState = GUI_STATE_SPLASH;
	g_guiLastState = GUI_STATE_NONE;
	g_guiRadioState = GUI_RADIOSTATE_NOCON;
	g_guiDelayCounter = 0;

	g_renderImageType = RENDER_IMAGE_TYPE_NONE;
	g_renderImageXSize = 28;
	g_renderImageYSize = 28;


	lcd_init_screen( INITR_BLACKTAB );
        
	lcd_fillScreen( COLOR_LEGIT_PURPLE );

        
	lcd_init_gfx( 160, 128 );
        lcd_gfx_setRotation( 0 );

	gui_draw_status_border();

	X11Flush();

	g_updateMenu = 1;

	// Setup screen refresh timer (32.5ms)
	/* PORT CODE -- NO LONGER NEEDED
	TACCR0 = 65000;
	TACTL = ID_3 | TASSEL_2 | MC_1;
	TACCTL0 |= CCIE;	// Enable Timer A0 interrupt
	*/
}

void clear_all_buttons( void )
{
	button_next = 0;
	button_back = 0;
	button_down = 0;
	button_up = 0;
}

void run_gui( void )
{
	if ( g_guiState == GUI_STATE_COMPOSE_QUEUE )
	{
		g_guiDelayCounter++;

		if ( g_guiDelayCounter == 3 )
			g_guiState = GUI_STATE_SPLASH;
	}
	else if ( g_guiState == GUI_STATE_COMPOSE_TEXT )
	{
		// Special button mode (allow holding down of buttons)
		if ( button_up )
		{
			button_down = 0; // Clear other button
			button_next = 0; // Clear other button
			button_back = 0; // Clear other button

			// Make buttons work (PORT)
    			//if ( (BUTTON_PORT & BUTTON_UP_BIT) )
			if ( g_buttonBit & BUTTON_UP_BIT )
				gui_text_button_depress( 1, 0 );
			else
				button_up = 0;	// Release it
		}
		else if ( button_down )
		{
			button_up = 0;	// Clear other button
			button_next = 0; // Clear other button
			button_back = 0; // Clear other button

			// Make buttons work (PORT)
    			//if ( (BUTTON_PORT & BUTTON_DOWN_BIT) )
			if ( g_buttonBit & BUTTON_DOWN_BIT )
				gui_text_button_depress( 0, 0 );
			else
				button_down = 0;	// Release it
		}
		else if ( button_next )
		{
			uint8_t pos_last;
			clear_all_buttons();

			if ( g_composeCharSelected == 64 )	
			{
				// BACK
				g_guiState = GUI_STATE_COMPOSE_IMAGE;
			}
			else if ( g_composeCharSelected == 65 )
			{
				// SEND
				gui_send_compose();

				// SEND an image
				g_guiState = GUI_STATE_COMPOSE_QUEUE;
				g_guiDelayCounter = 0;
			}
			else
			{
				// Save the text at the buffer position	
				g_composeTextBuffer[g_composeCharPos] = g_composeCharSelected;
	
				pos_last = g_composeCharPos;	
				if ( g_composeCharPos < g_composeCharTotal )
					g_composeCharPos++;

				g_composeCharSelected = g_composeTextBuffer[g_composeCharPos];
			
				gui_text_button_depress( 2, pos_last );
				gui_text_update_char_remaining();
			}
		}
		else if ( button_back )
		{
			uint8_t pos_last;
			clear_all_buttons();
			
			// Save the text at the buffer position	
			g_composeTextBuffer[g_composeCharPos] = g_composeCharSelected;
	
			pos_last = g_composeCharPos;	
			if ( g_composeCharPos > 0 )	
				g_composeCharPos--;

			g_composeCharSelected = g_composeTextBuffer[g_composeCharPos];
			
			gui_text_button_depress( 2, pos_last );
			gui_text_update_char_remaining();
		}
	}
	else
	{
		// Normal button mode (single depress mode)
	if ( button_next )
	{
		clear_all_buttons();
		
		if ( g_guiState == GUI_STATE_SPLASH )
			g_guiState = GUI_STATE_COMPOSE_WHO;
		else if ( g_guiState == GUI_STATE_COMPOSE_WHO && g_composeTeamPos < TEAM_COUNT ) // Only advance if the team position selected is valid!
			g_guiState = GUI_STATE_COMPOSE_IMAGE;
		else if ( g_guiState == GUI_STATE_COMPOSE_IMAGE )
			g_guiState = GUI_STATE_COMPOSE_TEXT;
	}
	else if ( button_back )
	{
		clear_all_buttons();

		if ( g_guiState == GUI_STATE_COMPOSE_WHO )
			g_guiState = GUI_STATE_SPLASH;	// CANCEL compose
		else if ( g_guiState == GUI_STATE_COMPOSE_IMAGE )
			g_guiState = GUI_STATE_COMPOSE_WHO;
		else if ( g_guiState == GUI_STATE_COMPOSE_TEXT )
			g_guiState = GUI_STATE_COMPOSE_TEXT;
		else if ( g_guiState == GUI_STATE_VIEW_MESSAGE )
			g_guiState = GUI_STATE_SPLASH;	// Back to splash
	}
	else if ( button_down )
	{
		clear_all_buttons();
		
		if ( g_guiState == GUI_STATE_SPLASH )
			g_guiState = GUI_STATE_VIEW_MESSAGE;
		else if ( g_guiState == GUI_STATE_COMPOSE_WHO )
			gui_compose_button_depress( 0 );
		else if ( g_guiState == GUI_STATE_COMPOSE_IMAGE )
			gui_image_select( 2 );
		else if ( g_guiState == GUI_STATE_VIEW_MESSAGE )
			gui_view_message_select( 2 );			
	}
	else if ( button_up )
	{
		clear_all_buttons();

		if ( g_guiState == GUI_STATE_COMPOSE_WHO )
			gui_compose_button_depress( 1 );
		else if ( g_guiState == GUI_STATE_COMPOSE_IMAGE )
			gui_image_select( 1 );			
		else if ( g_guiState == GUI_STATE_VIEW_MESSAGE )
			gui_view_message_select( 1 );			
	}
	}

	if ( g_guiState != g_guiLastState )
	{
		// Update last state
		g_guiLastState = g_guiState;

		// GUI state change (render UPDATE!)
		switch( g_guiState )
		{
		case GUI_STATE_NONE:
			// DO NOTHING
			break;

		case GUI_STATE_SPLASH:
			gui_render_splash();
			break;

		case GUI_STATE_VIEW_MESSAGE:
			gui_render_view_message();
			break;

		case GUI_STATE_COMPOSE_WHO:
			gui_render_compose_who();
			break;

		case GUI_STATE_COMPOSE_IMAGE:
			gui_render_compose_image();
			break;

		case GUI_STATE_COMPOSE_TEXT:
			gui_render_compose_text();
			break;

		case GUI_STATE_COMPOSE_QUEUE:
			break;

		default:
			g_guiState = GUI_STATE_NONE;
			break;
		}
	}	
	else if ( g_updateMenu )
	{
		g_updateMenu = 0;

		gui_render_menu();
	}

	if ( g_ledFlashTimer > 0 )
	{
		// Flash informing a message has arrived for around a second
		g_ledFlashTimer++;
		if ( g_ledFlashTimer%2 == 0 )
		{
			// PORT
			LEDOn( LED1_BIT | LED2_BIT );
			// LED_PORT |= (LED1_BIT | LED2_BIT);
		}
		else
		{
			// PORT
			LEDOff( LED1_BIT | LED2_BIT );
			// LED_PORT &= ~(LED1_BIT | LED2_BIT);	
			if ( g_ledFlashTimer > 8 )
				g_ledFlashTimer = 0;
		}
	}
}

void gui_flash_leds( void )
{
	g_ledFlashTimer = 1;
}

void gui_render_menu( void )
{
	uint8_t c[2];
	// NOW DRAW menu info
	
	// DRAW TEAM ID
	c[0] = '0' + g_myTeamID / 10;
	c[1] = '0' + g_myTeamID % 10;	

	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)-10, (SPLASH_STATUS_Y_LOC/2)-8, c[0], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );
	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)+2, (SPLASH_STATUS_Y_LOC/2)-8, c[1], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );
	

	// DRAW MESSAGE NUMBER
	c[0] = '0' + g_messageCur / 10;
	c[1] = '0' + g_messageCur % 10;	
	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)-10, SPLASH_STATUS_Y_LOC + (SPLASH_STATUS_Y_LOC/2)-8, c[0], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );
	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)+2, SPLASH_STATUS_Y_LOC + (SPLASH_STATUS_Y_LOC/2)-8, c[1], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );

	// DRAW MESSAGE COUNT
	c[0] = '0' + g_messageCount / 10;
	c[1] = '0' + g_messageCount % 10;	
	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)-10, (SPLASH_STATUS_Y_LOC*2) + (SPLASH_STATUS_Y_LOC/2)-8, c[0], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );
	lcd_gfx_drawChar( (SPLASH_STATUS_X_LOC/2)+2, (SPLASH_STATUS_Y_LOC*2) + (SPLASH_STATUS_Y_LOC/2)-8, c[1], COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 2 );

	// Draw radio status signal	
	// ERASE
	lcd_fillRect( 0, (SPLASH_STATUS_Y_LOC*3)+2, SPLASH_STATUS_X_LOC-1, SPLASH_STATUS_Y_LOC-1, COLOR_LEGIT_PURPLE );

	lcd_gfx_fillTriangle( (SPLASH_STATUS_X_LOC/2)-3, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5, (SPLASH_STATUS_X_LOC/2)+3, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5, (SPLASH_STATUS_X_LOC/2), (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2), ST7735_WHITE );

	lcd_gfx_drawFastVLine( 	(SPLASH_STATUS_X_LOC/2), (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2), 5, ST7735_WHITE ); 
	
	if ( g_guiRadioState == GUI_RADIOSTATE_CON )
	{
		// DO NOTHING!
	}
	else
	{
		const static tLinePoint x_line_array[4] = {
		{ (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5 },
		{ (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-4, (SPLASH_STATUS_X_LOC/2)+4, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5 },
		{ (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5 },
		{ (SPLASH_STATUS_X_LOC/2)-4, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-4 }
		};
		// NO CONNECTION
		// PUT AN X in RED through the radio symbol
		gui_draw_lines( x_line_array, 4, ST7735_RED );
		
		/*
		lcd_gfx_drawLine( (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, ST7735_RED ); 
		lcd_gfx_drawLine( (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-4, (SPLASH_STATUS_X_LOC/2)+4, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, ST7735_RED ); 
		
		lcd_gfx_drawLine( (SPLASH_STATUS_X_LOC/2)-5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-5, ST7735_RED ); 
		lcd_gfx_drawLine( (SPLASH_STATUS_X_LOC/2)-4, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)+5, (SPLASH_STATUS_X_LOC/2)+5, (SPLASH_STATUS_Y_LOC*3) + (SPLASH_STATUS_Y_LOC/2)-4, ST7735_RED ); 
		*/
	}
}

void gui_render_clear_work_area( void )
{
	lcd_fillRect( SPLASH_STATUS_X_LOC+1, 0, 89, 42, COLOR_LEGIT_PURPLE );
	lcd_fillRect( SPLASH_STATUS_X_LOC+1, 42, 130, 86, COLOR_LEGIT_PURPLE );
}

void gui_render_splash( void )
{
	gui_render_clear_work_area();

	// Render buttons
	lcd_gfx_drawRect( 39, 50, 50, 27, COLOR_LEGIT_WHITE );	// Draw border
	lcd_fillRect( 40, 51, 48, 25, COLOR_LEGIT_ORANGE );	// Fill inside
	
	lcd_gfx_drawRect( 90, 50, 50, 27, COLOR_LEGIT_WHITE );	// Draw border
	lcd_fillRect( 91, 51, 48, 25, COLOR_LEGIT_ORANGE );	// Fill inside
	
	// Render text
	lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_ORANGE );
	lcd_gfx_setCursor( 43, 52 );
	lcd_gfx_print( "COMPOSE" );
	lcd_gfx_setCursor( 49, 62 );
	lcd_gfx_print( "NEXT" );
	
	lcd_gfx_setCursor( 99, 52 );
	lcd_gfx_print( "VIEW" );
	lcd_gfx_setCursor( 99, 62 );
	lcd_gfx_print( "DOWN" );
}

void gui_view_message_select( uint8_t button_action )
{
	uint8_t view_message_number;
	uint16_t sram_address;
__attribute__((aligned(2)))	tRenderedMessage oRenderedMessage;

	gui_render_clear_work_area();

	if ( g_messageCount == 0 )
	{
		lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE );		
		lcd_gfx_setCursor( 32, 4 );
		lcd_gfx_print( "No messages" );
		
		return;
	}

	if ( button_action == 1 )
	{
		// UP button
		if ( g_messageCur > 1 )
			g_messageCur--;
	}
	else if ( button_action == 2 )
	{
		// DOWN button -- go to older messages
		if ( g_messageCur < MAX_SAVED_MESSAGES && g_messageCur < g_messageCount )
			g_messageCur++;
	}

	gui_clear_image_box();
	gui_render_menu();

	// Lookup message by number
	view_message_number = g_messageCur;

	// Account for rollover
	if ( view_message_number > g_messageLast )
		view_message_number = (g_messageLast+MAX_SAVED_MESSAGES) - view_message_number;
	else
		view_message_number = g_messageLast - view_message_number;	

	
	sram_address = ((sizeof(tRenderedMessage)/2) * view_message_number);

	sram_read_u16( sram_address, (uint16_t*)&oRenderedMessage, sizeof(tRenderedMessage)/2 );

	// Get type
	g_renderImageType = oRenderedMessage.image_type;
	g_renderImageXSize = oRenderedMessage.image_x_size;
	g_renderImageYSize = oRenderedMessage.image_y_size;

	memcpy( (void *)g_renderImageBuffer, (void *)oRenderedMessage.image_data, MAX_RENDERED_IMAGE_SIZE );

	gui_render_image();

	// Display text
	lcd_gfx_setCursor( 32, 44 );
	lcd_gfx_setTextWrapStartXPos( 32 );
	lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE );
	oRenderedMessage.text_data[oRenderedMessage.text_character_count] = '\0';
	lcd_gfx_print( oRenderedMessage.text_data );
	
	lcd_gfx_setCursor( 32, 4 );
	lcd_gfx_print( "From:" );

	lcd_gfx_setCursor( 32, 12 );
	if ( oRenderedMessage.from_tid < TEAM_COUNT )
		lcd_gfx_print( g_teamNameTable[oRenderedMessage.from_tid] );
	else
		lcd_gfx_print( "Unknown" );	
		
}

void gui_render_view_message( void )
{
	gui_view_message_select( 0 );
}

void gui_render_compose_who( void )
{
	g_composeTeamPos = 0xFF;

	gui_compose_button_depress( 1 );
}

void gui_render_compose_team_list( void )
{
	uint8_t i;

	gui_render_clear_work_area();
	gui_clear_image_box();
	
	// Display team picker...
	lcd_gfx_drawRect( 30, 2, 65, 121, COLOR_LEGIT_WHITE );
	lcd_fillRect( 31, 3, 63, 119, COLOR_LEGIT_ORANGE );
	lcd_gfx_drawRect( 95, 46, 64, 76, COLOR_LEGIT_WHITE );
	lcd_fillRect( 95, 47, 64, 75, COLOR_LEGIT_ORANGE );

	lcd_gfx_setTextColor( 0x003F, COLOR_LEGIT_ORANGE );		
	lcd_gfx_setCursor( 32, 4 );
	lcd_gfx_print( "To who:" );

	for ( i = 0; i < 12; i++ )
	{
		if ( get_message_queue_available_at( i ) == FORWARD_MESSAGE_STATUS_EMPTY && i != g_myTeamID )
			lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_ORANGE );
		else
			lcd_gfx_setTextColor( 0x3333, COLOR_LEGIT_ORANGE );

		lcd_gfx_setCursor( 32, (i*9)+12 );
		lcd_gfx_print( (char *)g_teamNameTable[i] );
	}

	for ( ; i < TEAM_COUNT; i++ )
	{
		if ( get_message_queue_available_at( i ) == FORWARD_MESSAGE_STATUS_EMPTY && i != g_myTeamID )
			lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_ORANGE );
		else
			lcd_gfx_setTextColor( 0x3333, COLOR_LEGIT_ORANGE );
		
		lcd_gfx_setCursor( 98, ((i-12)*9)+48 );
		lcd_gfx_print( (char *)g_teamNameTable[i] );
	} 	
}

void gui_render_compose_image( void )
{
	gui_render_clear_work_area();

	// Set to first image
	g_composeImagePos = 0;

	// Display an image!
	gui_image_select( 0 );
}

void gui_render_compose_text( void )
{
	uint8_t charTotalCount[12] = "of ";
	gui_render_clear_work_area();

	if ( g_composeTeamPos >= TEAM_COUNT )
		return;	// ERROR!

	// Calculate the number of characters total remaining
	if ( g_composeImagePos >= IMAGE_LOOKUP_TABLE_COUNT )
		return; // ERROR!

	if ( g_composeImagePos == 0 )
		g_composeCharTotal = 200;
	else
	{
		// TEXT HEADER = 2 bytes
		// IMAGE HEADER = 2 bytes
		uint16_t bytesForText = FORWARD_DATA_SIZE - ((g_imageLookupTable[g_composeImagePos].image_size+2)+2);
		
		bytesForText = ((bytesForText * 8) / 6); // ROUND down in this case
		
		if ( bytesForText > 200 )
			g_composeCharTotal = 200;
		else
			g_composeCharTotal = (uint8_t)bytesForText;	
	}
	
	// Reset these
	g_composeCharSelected = 0;
	g_composeCharPos = 0;

	// Clear the compose buffer
	memset( (void *)g_composeTextBuffer, 0, 200 );

	// Now render the message headers
	lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE );		
	lcd_gfx_setCursor( 32, 4 );
	lcd_gfx_print( "To:" );
	lcd_gfx_setCursor( 32, 13 );
	lcd_gfx_print( g_teamNameTable[g_composeTeamPos] );
	lcd_gfx_setCursor( 56, 22 );

	// PORT: itoa with sprintf
	// itoa( g_composeCharTotal, charTotalCount+3, 10 );
	sprintf( charTotalCount+3, "%d", g_composeCharTotal );

	charTotalCount[7] = '\0';

	lcd_gfx_print( charTotalCount  );

	gui_text_button_depress( 2, 0 );
}

void gui_update_radio_state( uint8_t radio_state )
{
	if ( radio_state != g_guiRadioState )
	{
		// Only update state when it changes!
		g_guiRadioState = radio_state;

		// Inform GUI to update menu
		g_updateMenu = 1;
	}
}

void gui_image_select( uint8_t button_depress )
{
	// Run buttons
	if ( button_depress == 1 )
	{
		g_composeImagePos++;
		if ( g_composeImagePos >= IMAGE_LOOKUP_TABLE_COUNT )
			g_composeImagePos = 0;
	}
	else if ( button_depress == 2 )
	{
		if ( g_composeImagePos > 0 )
			g_composeImagePos--;
		else
			g_composeImagePos = IMAGE_LOOKUP_TABLE_COUNT-1; 
		
	}

	// Render image!!!
	if ( g_imageLookupTable[g_composeImagePos].image_compression )
	{
		int16_t decompress_size;
		decompress_size = lzssvw_decompress( (uint8_t*)g_imageLookupTable[g_composeImagePos].image_data, (uint8_t*)g_renderImageBuffer, g_imageLookupTable[g_composeImagePos].image_size, 800 );

		if ( decompress_size == -1 )
			return;
		
	}

	// Get type
	g_renderImageType = g_imageLookupTable[g_composeImagePos].image_type;
	g_renderImageXSize = g_imageLookupTable[g_composeImagePos].image_x_size;
	g_renderImageYSize = g_imageLookupTable[g_composeImagePos].image_y_size;

	// Run the image rendered
	gui_render_image();
}

void gui_render_image( void )
{
	uint16_t image_pos_x;
	uint16_t image_pos_y;
	uint16_t image_max_x;
	uint16_t image_max_y;
	uint16_t x, y;

	gui_clear_image_box();
	
	if ( g_renderImageType == RENDER_IMAGE_TYPE_NONE )
		return;

	// Generate the image positions based off the size of the image (try to center it)
	image_pos_x = SPLASH_IMAGE_BOX_X_LOC+1 + (20 - (g_renderImageXSize/2));
	image_pos_y = 0 + (20 - (g_renderImageYSize/2));
	image_max_x = g_renderImageXSize;
	image_max_y = g_renderImageYSize;	


	if ( g_renderImageType == RENDER_IMAGE_TYPE_BW )
	{
		for ( y = 0; y < image_max_y; y++ )
			for ( x = 0; x < image_max_x; x++ )
			{
				uint16_t bytePos = ((y*image_max_y)+x)/8;
				uint8_t bitPos = ((y*image_max_y)+x) % 8;

				if ( g_renderImageBuffer[bytePos] & (1<<(7-bitPos)) )
					lcd_drawPixel( x+image_pos_x, y+image_pos_y, 0x526a );
			}
	}
	else
	{
		// Render COLOR
		for ( y = 0; y < image_max_y; y++ )
			for ( x = 0; x < image_max_x; x++ )
			{
				uint16_t bytePos = ((y*image_max_y)+x)/2;
				uint8_t bitPos = ((y*image_max_y)+x) % 2;

				uint8_t palette_value = (g_renderImageBuffer[bytePos] >> (4*(1-bitPos))) & 0xF;

				lcd_drawPixel( x+image_pos_x, y+image_pos_y, g_colorPalette[palette_value] );
			}
	}
}

void gui_clear_image_box( void )
{
	lcd_fillRect( SPLASH_IMAGE_BOX_X_LOC+1, 0, 40, 40, COLOR_LEGIT_PURPLE );
}

void gui_compose_button_depress( uint8_t button_up )
{
	uint8_t count;
	uint8_t new_pos, last_pos;
	// IF button_up == 1 it is an up depress
	// IF button_up == 0 it is a down depress

	// Rerender the list (incase it has changed)
	gui_render_compose_team_list();

	last_pos = g_composeTeamPos;

	if ( last_pos == 0xFF )
		new_pos = 1;	// Start over (the list might have changed)
	else
		new_pos = last_pos;	

	if ( button_up == 1 )
	{
		// GO BACKWARDS
		for ( count = 0; count < TEAM_COUNT; count++ )
		{
			if ( new_pos == 0 )
				new_pos = TEAM_COUNT-1;
			else
				new_pos--;

			if ( get_message_queue_available_at( new_pos ) == FORWARD_MESSAGE_STATUS_EMPTY && new_pos != g_myTeamID )
				break;
		}

	}
	else
	{
		// GO FORWARD
		for ( count = 0; count < TEAM_COUNT; count++ )
		{
			if ( new_pos >= (TEAM_COUNT-1) )
				new_pos = 0;
			else
				new_pos++;

			if ( get_message_queue_available_at( new_pos ) == FORWARD_MESSAGE_STATUS_EMPTY && new_pos != g_myTeamID )
				break;
		}
	}
	
	if ( count == TEAM_COUNT )
		new_pos = 0xFF;

	// OK highlight selected (if it was changed previously)
	if ( new_pos < TEAM_COUNT )
	{
		uint8_t x_pos, y_pos;
		if ( new_pos < 12 )
		{
			x_pos = 32;
			y_pos = (new_pos*9)+12;
		}
		else
		{
			x_pos = 98;
			y_pos = ((new_pos-12)*9)+48;
		}

		lcd_gfx_setTextColor( 0x003F, COLOR_LEGIT_ORANGE );
		lcd_gfx_setCursor( x_pos, y_pos );
		lcd_gfx_print( (char*)g_teamNameTable[new_pos] ); 
	}

	g_composeTeamPos = new_pos;	
}
			
void gui_text_button_depress( uint8_t button_up, uint8_t pos_last )
{
	uint8_t cur_char;
	uint8_t prev_char;
	uint8_t x_pos, y_pos;
	// 1 for button up
	// 0 for button down

	cur_char = g_composeCharSelected;
	prev_char = g_composeCharSelected;

	if ( button_up == 1 )
	{
		if ( cur_char == 0 )
			cur_char = 65;
		else
			cur_char--;
	}
	else if ( button_up == 0 )
	{
		if ( cur_char == 65 )
			cur_char = 0;
		else
			cur_char++;
	}
	// DO NOTHING if button_up == 2 (just display the current)

	// Update
	g_composeCharSelected = cur_char;

	// ERASE old line	
	x_pos = 32 + ((pos_last%21)*6);
	y_pos = 44 + ((pos_last/21)*8);
	lcd_gfx_drawFastHLine( x_pos, y_pos+8, 6, COLOR_LEGIT_PURPLE );

	// Set wrap start X position
	lcd_gfx_setTextWrapStartXPos( 32 );

	// ERASE previous character (if they moved backwards)
	if ( g_composeCharPos < pos_last )
	{
		lcd_gfx_setCursor( x_pos, y_pos );
		lcd_gfx_print( "    " );
	}

	// Calculate render position
	x_pos = 32 + ((g_composeCharPos%21)*6);
	y_pos = 44 + ((g_composeCharPos/21)*8);

	if ( (prev_char == 64 || prev_char == 65) && cur_char != 64 && cur_char != 65 )
	{	
		lcd_gfx_setCursor( x_pos, y_pos );
		lcd_gfx_print( "    " );
	}

	// OK render the appropriate text
	if ( cur_char < 26 )
		lcd_gfx_drawChar( x_pos, y_pos, (cur_char+'a'), COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 1 );
	else if ( cur_char < 52 )
		lcd_gfx_drawChar( x_pos, y_pos, ((cur_char-26)+'A'), COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 1 );
	else if ( cur_char < 62 )
		lcd_gfx_drawChar( x_pos, y_pos, ((cur_char-52)+'0'), COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 1 );
	else if ( cur_char == 62 )
		lcd_gfx_drawChar( x_pos, y_pos, ' ', COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 1 );
	else if ( cur_char == 63 )
		lcd_gfx_drawChar( x_pos, y_pos, ',', COLOR_LEGIT_WHITE, COLOR_LEGIT_PURPLE, 1 );
			
	else if ( cur_char == 64 )
	{
		lcd_gfx_setCursor( x_pos, y_pos );
		lcd_gfx_print( "BACK" );
	}
	else if ( cur_char == 65 )
	{
		lcd_gfx_setCursor( x_pos, y_pos );
		lcd_gfx_print( "SEND" );
	}

	// Draw a line under the chosen character
	lcd_gfx_drawFastHLine( x_pos, y_pos+8, 6, COLOR_LEGIT_WHITE );
}

void gui_text_update_char_remaining( void )
{
	char charPosCount[8];

	// PORT: itoa with sprintf
	// itoa( g_composeCharPos, charPosCount, 10 );
	sprintf( charPosCount, "%d", g_composeCharPos );
	
	charPosCount[3] = '\0';

	lcd_gfx_setCursor( 32, 22 );
	lcd_gfx_print( charPosCount );
}

void gui_send_compose( void )
{
	uint8_t payload[86*3];
	uint16_t pos;
	uint8_t size_remaining;
	uint8_t i;
	tBitState bit_state;

	// Send text section
	payload[0] = 0x80;	// Text, charset=6bit, and 0 for color (WHITE)
	
	pos = 0;	
	// Convert text to 6-bit encoding
	text_init_state( &bit_state, payload+2 );
	for ( i = 0; i < g_composeCharPos; i++ )
		text_writebit( g_composeTextBuffer[i], 6, &bit_state );
	text_writeflush( &bit_state );

	// Get size of stream
	pos = bit_state.cur_pos;

	// Length is the number of characters
	payload[1] = g_composeCharPos;
	pos+=2;

	size_remaining = (FORWARD_DATA_SIZE-pos);
		
	// NOW add image (if it fits -- it should always fit due to previous restricts)
	if ( g_imageLookupTable[g_composeImagePos].image_size <= (size_remaining-2) && g_imageLookupTable[g_composeImagePos].image_type != RENDER_IMAGE_TYPE_NONE )
	{
		payload[pos] = 0x40;
		if ( g_imageLookupTable[g_composeImagePos].image_compression )
			payload[pos] |= IMAGE_HEADER_COMPRESSION_MASK;
		if ( g_imageLookupTable[g_composeImagePos].image_type == RENDER_IMAGE_TYPE_COLOR )
			payload[pos] |= IMAGE_HEADER_TYPE_MASK;
	
	
		// Setup the image header with the x/y sizes	
		payload[pos] |= ((g_imageLookupTable[g_composeImagePos].image_x_size - 28) / 4) << IMAGE_HEADER_X_SIZE_SHIFT;
		payload[pos] |= ((g_imageLookupTable[g_composeImagePos].image_y_size - 28) / 4) << IMAGE_HEADER_Y_SIZE_SHIFT;

		// Record the size
		payload[pos+1] = g_imageLookupTable[g_composeImagePos].image_size;
		
		// Lastly copy in the image data
		memcpy( payload+pos+2, g_imageLookupTable[g_composeImagePos].image_data, g_imageLookupTable[g_composeImagePos].image_size );	

		pos += g_imageLookupTable[g_composeImagePos].image_size+2;	
	}

	// message queue!!!!
	radio_add_forward_message( payload, pos, g_myTeamID, g_composeTeamPos );

	
	// Render	
	gui_render_clear_work_area();

	// Render buttons
	lcd_gfx_drawRect( 49, 55, 70, 27, COLOR_LEGIT_WHITE );	// Draw border
	lcd_fillRect( 50, 56, 68, 25, COLOR_LEGIT_ORANGE );	// Fill inside
	
	// Render text
	lcd_gfx_setTextColor( COLOR_LEGIT_WHITE, COLOR_LEGIT_ORANGE );
	lcd_gfx_setCursor( 53, 57 );
	lcd_gfx_print( "QUEUED" );
}

void write_message_to_sram( tRenderedMessage *pRenderedMessage )
{
	uint16_t sram_address;

	sram_address = ((sizeof(tRenderedMessage) / 2) * g_messageLast);

	sram_write_u16( sram_address, (uint16_t*)pRenderedMessage, sizeof(tRenderedMessage)/2 );

	// Increment the last message
	g_messageLast++;
	if ( g_messageLast >= MAX_SAVED_MESSAGES )
		g_messageLast = 0;

	if ( g_messageCount == 0 )
		g_messageCur = 1;	// Stay on the first message
	else
	{
		// Increment the current message (that we are viewing)
		// only if there are messages available
		if ( g_messageCur < MAX_SAVED_MESSAGES )
			g_messageCur++;
	}

	if ( g_messageCount < MAX_SAVED_MESSAGES )
		g_messageCount++;

	// Flash leds
	gui_flash_leds();

	// Rerender the current message
	g_updateMenu = 1;
}
