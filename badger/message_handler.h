#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

#include <stdint.h>

// TURN ON FOR DEBUGGING ONLY
// #define DEBUG_APP_MESSAGES	1

#define RECV_MESSAGE_PAYLOAD_SIZE	(86*3)-4

#define MAX_TEXT_CHARACTERS	200

#define MAX_IMAGE_DATA_LEN	240

#define MAX_RENDERED_IMAGE_SIZE	((40*40)/2)

#define MAX_COMMAND_LENGTH	64

#define IMAGE_X_MAXSIZE		40
#define IMAGE_Y_MAXSIZE		40

#define MESSAGE_TYPE_MASK	0xC0
#define MESSAGE_TYPE_UNKNOWN	0x00
#define MESSAGE_TYPE_IMAGE	0x40
#define MESSAGE_TYPE_TEXT	0x80
#define MESSAGE_TYPE_COMMAND	0xC0

// This struct contains the receive message (received from device)
typedef struct RECV_MESSAGE_STRUCT	// KNOWN as a FORWARD DATA STRUCT
{
	uint16_t session_id;
	uint8_t from_tid;
	uint8_t data_len;
	uint8_t payload_data[RECV_MESSAGE_PAYLOAD_SIZE];
} tReceiveForwardData;

// Status of the forward messages
#define FORWARD_MESSAGE_STATUS_EMPTY	0	// No forward message in this slot
#define FORWARD_MESSAGE_STATUS_FULL	1	// Forward message ready to be sent

#define FORWARD_MESSAGE_STATE_NONE	0	// Ready
#define FORWARD_MESSAGE_STATE_RETRY	1	// Retry to send
#define FORWARD_MESSAGE_STATE_PENDING	2	// Pending send

// This struct contains the forward message (to be sent)
typedef struct SEND_MESSAGE_STRUCT
{
	uint8_t status;
	uint8_t state;
	uint8_t from_tid;
	uint8_t data_len;
	uint8_t payload_data[RECV_MESSAGE_PAYLOAD_SIZE];
} tSendForwardData;

#define TEXT_HEADER_COLOR_MASK		0x0F
#define TEXT_HEADER_CHARSET_MASK	0x10	// If set = 7bit character set, 6bit character set if not set

typedef struct TEXT_INFO_STRUCT
{
	uint8_t text_header;
	uint8_t character_count;
	uint8_t character_data[MAX_TEXT_CHARACTERS];
} tTextInfo;

#define IMAGE_HEADER_TYPE_MASK		0x20
#define IMAGE_HEADER_COMPRESSION_MASK	0x10
#define IMAGE_HEADER_X_SIZE_MASK	0x0C
#define IMAGE_HEADER_Y_SIZE_MASK	0x03


#define IMAGE_HEADER_X_SIZE_SHIFT	2
#define IMAGE_HEADER_Y_SIZE_SHIFT	0

typedef struct IMAGE_INFO_STRUCT
{
	uint8_t image_header;
	uint8_t	data_len;
	uint8_t image_data[MAX_IMAGE_DATA_LEN];
} tImageInfo;

#define COMMAND_HEADER_LENGTH_MASK	0x3F

typedef struct COMMAND_INFO_STRUCT
{
	uint8_t command_header;
	uint8_t command_data[MAX_COMMAND_LENGTH];
} tCommandInfo;

#define RENDER_TYPE_NORMAL	0	// Full render and display
#define RENDER_TYPE_SILENT	1	// Render image internally but do not display (silent)

#define RENDER_IMAGE_TYPE_NONE	0
#define RENDER_IMAGE_TYPE_BW	1
#define RENDER_IMAGE_TYPE_COLOR	2

typedef struct RENDERED_MESSAGE_STRUCT
{
	uint16_t session_id;		// The session ID for this message
	uint8_t from_tid;		// From Team ID
	uint8_t render_type;		// render type info
	uint8_t text_color;		// 4-bit palette lookup for text color		
	uint8_t text_character_count;	// How many characters is the text
	uint8_t text_data[MAX_TEXT_CHARACTERS+1];		// The text data (up to max characters)
	uint8_t image_type;		// The type of image (B/W or 4-bit color)
	uint8_t image_x_size;		// The image size in X dimension
	uint8_t image_y_size;		// The image size in Y dimension
	uint8_t image_data[MAX_RENDERED_IMAGE_SIZE];	// The image data (up to maximum image data size)
} tRenderedMessage;

typedef struct COMMAND_RESPONSE_STATE
{
        uint8_t command_response_buffer[86-4];
        uint8_t pos;
} tCommandResponseState;

void recv_data_message( uint8_t *pMessage, uint16_t dataLen );
void recv_response_message( uint8_t *pMessage, uint16_t dataLen );

uint8_t get_message_queue_available_at( uint8_t team_id );

uint8_t message_process_image( tRenderedMessage *pRenderedMessage, uint8_t *payload, uint8_t payload_length );
uint8_t message_process_text( tRenderedMessage *pRenderedMessage, uint8_t *payload, uint8_t payload_length );
uint8_t message_process_command( tRenderedMessage *pRenderedMessage, tCommandResponseState *state, uint8_t *payload, uint8_t payload_length, uint8_t *command_num, uint8_t from_tid );

#endif // __MESSAGE_HANDLER_H__
