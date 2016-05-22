// #include "omsp_system.h"
#include "port_helper.h"
#include "hardware.h"
#include "sram_hal.h"
#include "lcd_hal.h"
#include "lcd_gfx.h"
#include "radio_uart.h"
#include "cli.h"
#include "gui.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


#define FLAG_FILE_NAME	"/home/badger/flag"
#define MAX_IDLE_SECS	(60)

volatile uint8_t g_myTeamID;

void sig_alarm_handler( int signum )
{
	printf( "Timeout! Connection closing.\n" );

	exit( 1 );
}

/**
* Run the application code (display messages from the radio and send responses)
*/
int main(void) 
{
	char pFlagData[32];

	/* Porting code -- Don't need to do this
	WDTCTL = WDTPW | WDTHOLD;	// Disable watchdog timer

	P1OUT  = 0x00;                     // Port data output
	P2OUT  = 0x00;
 
	P1DIR  = 0x00;                     // Port direction register
	P2DIR  = 0xff;
 
	P1IES  = 0x00;                     // Port interrupt enable (0=dis 1=enabled)
	P2IES  = 0x00;
	P1IE   = 0x0F;                     // Port interrupt Edge Select (0=pos 1=neg)
	P2IE   = 0x00;
	*/

	// Setup no buffering for XINETD service
        setvbuf( stdout, NULL, _IONBF, 0 );

	FILE *pFlagFile = fopen( FLAG_FILE_NAME, "r" );

	if ( !pFlagFile )
	{
		printf( "Failed to read flag data!\n" );
		exit(1);
	}

	memset( pFlagData, 0, 32 );	
	fgets( pFlagData, 32, pFlagFile );

	fclose( pFlagFile );
       
	// Setup sig alarm handler
        signal( SIGALRM, sig_alarm_handler );
        alarm( MAX_IDLE_SECS );
	
	sram_init();
	cli_init_uart();
	radio_init_uart();
	//init_gui();
	
	// PORT: OLD: g_myTeamID = TEAM_ID_ADDR; 
	g_myTeamID = 0; // PPP now 



	puts( "This is an infamous challenge from DEF CON CTF Finals in 2014, in which a custom hardware badge was made by the LegitBS team, I've ported it to work here, enjoy.\n" );	
	puts( "The buttons are mapped to the arrow keys.\n" );
	puts( ".........................................\n" );

	puts( "Application Core v1.0" );
	puts( "openMSP430 core by Oliver Girard" );
	puts( "p.s. I modded the core to make data executable -sirgoon" );

	// Enable interrupts
	eint();

	// Run process loop

	/*
	while ( 1 )
	{
		LPM0;
		cli_run();
		radio_uart_run();
	}
	*/
	interrupt_handler();
}
