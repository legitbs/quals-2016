#ifndef __GUI_H__
#define __GUI_H__
#include "message_handler.h"

#include <stdint.h>

#define TEAM_COUNT		(20)

#define MAX_SAVED_MESSAGES	(80)

#define GUI_STATE_NONE		(0)
#define GUI_STATE_SPLASH	(1)	// The top level window (splash screen!)
#define GUI_STATE_VIEW_MESSAGE	(2)	// View a message (g_messageCur is the message being viewed)
#define GUI_STATE_COMPOSE_WHO	(3)	// Pick who to compose a message to!
#define GUI_STATE_COMPOSE_IMAGE	(4)	// Pick an image to send!
#define GUI_STATE_COMPOSE_TEXT	(5)	// Write text
#define GUI_STATE_COMPOSE_QUEUE	(6)	// Inform them the message is being queued

// Maybe add some indicators of the RSSI??? Ya that is a big TO DO haha
#define GUI_RADIOSTATE_NOCON	0
#define GUI_RADIOSTATE_CON	1

typedef struct MESSAGE_SRAM_INFO_STRUCT
{
	uint8_t message_num;
	uint16_t sram_address;
} tMessageSramInfo;

typedef struct IMAGE_LOOKUP_DATA_STRUCT
{
	uint8_t image_type;
	uint8_t image_compression;
	uint8_t image_x_size;
	uint8_t image_y_size;
	uint8_t image_size;
	uint8_t *image_data;
} tImageLookupData;

typedef struct LINE_POINT_STRUCT
{
	uint8_t start_x, start_y;
	uint8_t end_x, end_y;
} tLinePoint;

void init_gui( void );
void run_gui( void );
void add_rendered_message( tRenderedMessage *pMessage );
void gui_render_menu( void );
void gui_render_splash( void );
void gui_render_view_message( void );
void gui_view_message_select( uint8_t button_action );
void gui_render_compose_team_list( void );
void gui_render_compose_who( void );
void gui_render_compose_image( void );
void gui_render_compose_text( void );

void gui_image_select( uint8_t button_action );
void gui_render_image( void );
void gui_clear_image_box( void );

void gui_compose_button_depress( uint8_t button_up );
void gui_text_button_depress( uint8_t button_up, uint8_t pos_last );
void gui_text_update_char_remaining( void );

void gui_send_image( void );
void gui_send_compose( void );
void gui_update_radio_state( uint8_t radioState );

void write_message_to_sram( tRenderedMessage *pRenderedMessage );

void SetButtonUp( void );
void SetButtonDown( void );
void SetButtonBack( void );
void SetButtonNext( void );

void ReleaseButtonUp( void );
void ReleaseButtonDown( void );
void ReleaseButtonBack( void );
void ReleaseButtonNext( void );

#endif // __GUI_H__
