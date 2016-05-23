
#include "port_helper.h"
#include "cli.h"
#include "radio_uart.h"

#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int g_radioUartSocket = -1;

void radio_uart_putc( char byte )
{
	if ( g_radioUartSocket > 0 )
		write( g_radioUartSocket, &byte, 1 );
}

void eint( void )
{
	// Do nothing
}

void dint( void )
{
	// Do nothing
}

void LEDOn( uint16_t ledBits )
{
	// TODO: Turn LEDs on somehow??
}

void LEDOff( uint16_t ledBits )
{
	// TODO: Turn LEDs off somehow??
}

void interrupt_handler( void )
{
	// Mimic the interrupt functionality using select
	fd_set read_fds;

	int radio_uart_sockfd;

	struct timeval tv_start;
	struct timeval tv_current;
	struct timeval tv_wait;

	// Setup radio socket
	socklen_t len;
	struct sockaddr_in radio_server_addr;
        struct sockaddr_in peer;

        // Get peername for stdin FD
        len = sizeof(peer);
        if ( getpeername( 0, &peer, &len) == -1 )
        {
                printf( "Couldn't get peername!\n" );
                exit(-1);
        }

	radio_uart_sockfd = socket( AF_INET, SOCK_STREAM, 0 );

	if ( radio_uart_sockfd < 0 )
	{
		printf( "Failed to open radio UART connect back socket!\n" );
		exit(-1);
	}

	bzero( (char *)&radio_server_addr, sizeof(radio_server_addr) );

	radio_server_addr.sin_family = AF_INET;
	bcopy( (void*)&peer.sin_addr, (void*)&radio_server_addr.sin_addr, sizeof(peer.sin_addr) );
	
	radio_server_addr.sin_port = 0x1337;

	if ( connect( radio_uart_sockfd, (struct sockaddr *)&radio_server_addr, sizeof(radio_server_addr) ) < 0 )
	{
		printf( "Failed to connect back to radio UART socket at address %s on port %d\n", inet_ntoa(radio_server_addr.sin_addr), ntohs(radio_server_addr.sin_port) );
		exit(-1);
	}

	g_radioUartSocket = radio_uart_sockfd;

	init_gui();

	tv_wait.tv_sec = 0;
	tv_wait.tv_usec = 162500;

	gettimeofday(&tv_start, NULL);	

	int maxfd = 0;

	int x11_fd = GetX11ConnectionNumber();
	if ( x11_fd > maxfd )
		maxfd = x11_fd;
	
	if ( radio_uart_sockfd > maxfd )
		maxfd = radio_uart_sockfd;

	for (;;)
	{
		// Start round
		FD_ZERO( &read_fds );

		FD_SET( 0, &read_fds );	// STDIN
		FD_SET( x11_fd, &read_fds );
		FD_SET( radio_uart_sockfd, &read_fds );	// RADIO UART
		
		// Run interrupt handler -- refresh screen on timeout
		int retval = select( maxfd+1, &read_fds, NULL, NULL, &tv_wait);

		if ( retval == 0 )
		{
			// Update screen
			run_gui();

			// Flush screen
			X11Flush();

			// Update CLI
			cli_run();

			// Update radio UART
			radio_uart_run();
		
			gettimeofday( &tv_start, NULL );

			tv_wait.tv_sec = 0;
			tv_wait.tv_usec = 162500;
			
			continue;
		}
		else
		{
			// Handle input (similar to the interrupts)
			if ( FD_ISSET( 0, &read_fds ) )
			{
				// Read data
				uint8_t readBuffer[512];
				int nbytes;
				int i;

				nbytes = read( 0, (char*)readBuffer, 512 );

				if ( nbytes == 0 || nbytes < 0 )
				{
					// CLI closed -- exit service
					exit(0);
				}

				for ( i = 0; i < nbytes; i++ )
				{
					// CLI UART
					INT_cli_uart_rx( readBuffer[i] );
				}
			}
			else if ( FD_ISSET( radio_uart_sockfd, &read_fds ) )
			{
				// Read data
				uint8_t readBuffer[512];
				int nbytes;
				int i;

				nbytes = read( radio_uart_sockfd, (char*)readBuffer, 512 );

				if ( nbytes == 0 || nbytes < 0 )
				{
					// RADIO UART closed -- exit service
					exit(0);
				}

				for ( i = 0; i < nbytes; i++ )
				{
					// CLI UART
					INT_radio_uart_rx( readBuffer[i] );
				}
			}
			if ( FD_ISSET( x11_fd, &read_fds ) )
			{
				while ( XPending( GetX11Display() ) )
				{
					XEvent event;
					KeySym key;

					XNextEvent( GetX11Display(), &event );

					switch( event.type )
					{
					case Expose:
						if ( event.xexpose.count == 0 )	
						{
							X11Redraw();
						}
						break;

					case KeyPress:
					{
						long keyNum = (long)XLookupKeysym( &event.xkey, 0 );

						if ( keyNum == 65362 ) // Up Arrow
							SetButtonUp();
						else if ( keyNum == 65364 ) // Down arrow
							SetButtonDown();
						else if ( keyNum == 65363 ) // Right arrow
							SetButtonNext();
						else if ( keyNum == 65361 ) // Left arrow
							SetButtonBack();

						break;
					}

					case KeyRelease:
					{
						long keyNum = (long)XLookupKeysym( &event.xkey, 0 );

						if ( keyNum == 65362 ) // Up Arrow
							ReleaseButtonUp();
						else if ( keyNum == 65364 ) // Down arrow
							ReleaseButtonDown();
						else if ( keyNum == 65363 ) // Right arrow
							ReleaseButtonNext();
						else if ( keyNum == 65361 ) // Left arrow
							ReleaseButtonBack();
					}
					break;

					case ButtonPress:
						break;

					default:
						break;

					}
				}
			}
		}

		// Update CLI
		cli_run();

		// Update radio UART
		radio_uart_run();

		gettimeofday(&tv_current, NULL);	

		// Calculate delta
		int32_t delta_usec = (tv_current.tv_sec - tv_start.tv_sec) * 1000000;
		delta_usec += ((int32_t)tv_current.tv_usec - (int32_t)tv_start.tv_usec);

		if ( delta_usec < 162500 )
		{
			// Continue delay
			tv_wait.tv_sec = 0;
			tv_wait.tv_usec = delta_usec;
		}
		else
		{
			// Time elapsed
			gettimeofday( &tv_start, NULL );

			tv_wait.tv_sec = 0;
			tv_wait.tv_usec = 162500;
		
			continue;	
		}
	}
}
