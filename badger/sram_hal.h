#ifndef __SRAM_HAL_H__
#define __SRAM_HAL_H__

#include <stdint.h>

#define SRAM_CSN_PORT	(P2OUT)
#define SRAM_CSN_BIT	(0x08)		// PIN 8 for non-server FPGA load

//============================================================================
// SRAM SPI Controller
//============================================================================
#define SRAMSPI_CTRL            (*(volatile unsigned int *)0x0098)      // CTRL register address 
#define SRAMSPI_DATA            (*(volatile unsigned int *)0x009A)      // Data register (8-bit or 16-bits)

#define SRAMSPI_CTRL_SMCLK      (0x0200)        // SMCLK select
#define SRAMSPI_CTRL_SENDCNT    (0x0100)        // Send count (1 = 16-bits, 0 = 8 bits)
#define SRAMSPI_CTRL_CLOCKDIV   (0x00E0)        // Clock divider (power of 2)
#define SRAMSPI_CTRL_IEN        (0x0010)        // Interrupt enable
#define SRAAMSPI_CTRL_IFLG      (0x0008)        // Interrupt flag
#define SRAMSPI_CTRL_CKPH       (0x0004)        // Clock Phase
#define SRAMSPI_CTRL_CKPOL      (0x0002)        // Clock Polarity
#define SRAMSPI_CTRL_EN         (0x0001)        // Enable



void sram_init( void );
// uint8_t sram_read_mode( void );
void sram_read_u16( uint16_t address, uint16_t *dest, uint16_t length );
void sram_write_u16( uint16_t address, uint16_t *dest, uint16_t length );


#endif // SRAM_HAL_H__
