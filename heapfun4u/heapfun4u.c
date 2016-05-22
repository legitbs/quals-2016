#include <stdio.h>
#include <string.h>
#include "dc_malloc.h"

void *array[100];
int array_sz[100];
int c;

void print_array()
{
	int i =0;

	for ( i = 0; i < 100; i++) {
		if ( array[i] == NULL ) {
			continue;
		}

		printf("%d) %p -- %d\n", i+1, array[i], array_sz[i]);
	}

	return;
}

void name( void )
{
	int hello;

	printf("Here you go: %p\n", &hello);

	return;
}

void wb( )
{
	char stuff[16];
	int sel = 0;

	print_array();

	printf("Write where: ");
	if ( read( 0, stuff, 15) <= 0 ) {
		exit(0);
	}

	sel = atoi(stuff) - 1;

	if ( sel < 0 || sel >= 100 ) {
		exit(0);
	}

	if ( !array[sel] ) {
		exit(0);
	}

	printf("Write what: ");

	if ( read( 0, array[sel], array_sz[sel]) <= 0 ) {
		exit(0);
	}


	return;
}

int main()
{
	memset( array, 0, sizeof(void*) * 100);
	c = 0;
	int sel = 0;
	char yo[256];

	setvbuf( stdin, NULL, _IONBF, 0 );
	setvbuf( stdout, NULL, _IONBF, 0 );

	while ( 1 ) {
		printf("[A]llocate Buffer\n");
		printf("[F]ree Buffer\n");
		printf("[W]rite Buffer\n");
		printf("[N]ice guy\n");
		printf("[E]xit\n");
		printf("| ");
		
		if ( read( 0,  yo, 255) <= 0 ) {
			exit(0);
		}

		switch( yo[0] ) {
			case 'A':
				if ( c == 100) {
					exit(0);
				}

				printf("Size: ");

				if ( read(0,yo, 255) <= 0 ) {
					exit(0);
				}

				sel = atoi( yo );
				array[c] = dc_malloc(sel);
				array_sz[c] = sel;

				if ( !array[c] ) {
					exit(0);
				}

				c++;

				break;
			case 'F':
				print_array();
				printf("Index: ");
				
				if ( read( 0, yo, 256) <= 0 ) {
					exit(0);
				}

				sel = atoi(yo) - 1;

				if ( sel < 0 || sel >=100) {
					exit(0);
				}

				if ( array[sel] ) {
					dc_free( array[sel] );
				} else {
					exit(0);
				}

				break;
			case 'W':
				wb();
				break;
			case 'N':
				name();
				break;
			case 'E':
				printf("Leave\n");
				return 0;
			default:
				exit(0);
				break;
		};		
	}

	return 0;
}
