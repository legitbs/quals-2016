#ifndef MAIN_H
#define MAIN_H
 
// #include "omsp_system.h"

// Scoring TOKEN is memory mapped at these addresses
/* PORT: COMMENT THESE OUT SO WE DON'T ACCIDENTALLY USE THEM
#define TOKEN_0			(*(volatile uint16_t *) 0x01A0)
#define TOKEN_1			(*(volatile uint16_t *) 0x01A2)
#define TOKEN_2			(*(volatile uint16_t *) 0x01A4)
#define TOKEN_3			(*(volatile uint16_t *) 0x01A6)

// SLA STATUS polling (to counter teams trying to move OFF device) 
#define POLL_STATUS_0		(*(volatile uint16_t *) 0x01A8)
#define POLL_STATUS_1		(*(volatile uint16_t *) 0x01AA)
#define POLL_STATUS_2		(*(volatile uint16_t *) 0x01AC)
#define POLL_STATUS_3		(*(volatile uint16_t *) 0x01AE)

// Team ID (set by the radio processor)
#define TEAM_ID_ADDR		(*(volatile uint16_t *) 0x01B0)

//--------------------------------------------------
// Hardware UART register address mapping
//--------------------------------------------------

#define CLI_UART_CTL          (*(volatile unsigned char *) 0x0080)  // UART Control register (8bit)
#define CLI_UART_STAT         (*(volatile unsigned char *) 0x0081)  // UART Status register (8bit)
#define CLI_UART_BAUD         (*(volatile unsigned int  *) 0x0082)  // UART Baud rate configuration (16bit)
#define CLI_UART_TXD          (*(volatile unsigned char *) 0x0084)  // UART Transmit data register (8bit)
#define CLI_UART_RXD          (*(volatile unsigned char *) 0x0085)  // UART Receive data register (8bit)

//--------------------------------------------------
// Hardware UART register address mapping
//--------------------------------------------------

#define APP_UART_CTL          (*(volatile unsigned char *) 0x0088)  // UART Control register (8bit)
#define APP_UART_STAT         (*(volatile unsigned char *) 0x0089)  // UART Status register (8bit)
#define APP_UART_BAUD         (*(volatile unsigned int  *) 0x008A)  // UART Baud rate configuration (16bit)
#define APP_UART_TXD          (*(volatile unsigned char *) 0x008C)  // UART Transmit data register (8bit)
#define APP_UART_RXD          (*(volatile unsigned char *) 0x008D)  // UART Receive data register (8bit)



//--------------------------------------------------
// Hardware UART register field mapping
//--------------------------------------------------

// UART Control register fields
#define  UART_IEN_TX_EMPTY  0x80
#define  UART_IEN_TX        0x40
#define  UART_IEN_RX_OVFLW  0x20
#define  UART_IEN_RX        0x10
#define  UART_SMCLK_SEL     0x02
#define  UART_EN            0x01

// UART Status register fields
#define  UART_TX_EMPTY_PND  0x80
#define  UART_TX_PND        0x40
#define  UART_RX_OVFLW_PND  0x20
#define  UART_RX_PND        0x10
#define  UART_TX_FULL       0x08
#define  UART_TX_BUSY       0x04
#define  UART_RX_BUSY       0x01
*/

//--------------------------------------------------
// Diverse
//--------------------------------------------------

// BAUD = (mclk_freq/baudrate)-1

//#define BAUD           2083            //   9600  @20.0MHz
//#define BAUD           1042            //  19200  @20.0MHz
//#define BAUD            521            //  38400  @20.0MHz
//#define BAUD            347            //  57600  @20.0MHz
//#define BAUD            174            // 115200  @20.0MHz
//#define BAUD             87            // 230400  @20.0MHz
#define CLI_BAUD         138             // 115200 @ 16.0MHz 
#define APP_BAUD         34             // 460800 @ 16.0MHz 

// LEDs
#define LED1_BIT    0x1
#define LED2_BIT    0x2
#define LED_PORT    (P2OUT)

#define BUTTON_NEXT_BIT 0x1
#define BUTTON_UP_BIT   0x2 
#define BUTTON_DOWN_BIT 0x4 
#define BUTTON_BACK_BIT 0x8 
//PORT COMMENT THESE OUT SO WE DON'T ACCIDENTALLY USE THEM
//#define BUTTON_PORT (P1IN)
//#define BUTTON_FLAG     (P1IFG)
 
 
#endif // MAIN_H
