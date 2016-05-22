#include "delay.h"

// Dumb delay code (doesn't use timers)
/**
Delay function.
*/
void delay(unsigned int c, unsigned int d)
{
  volatile int i, j;
  for (i = 0; i<c; i++) {
    for (j = 0; j<d; j++) {
      //nop();
      //nop();
    }
  }
}

void delay_ms( unsigned int ms )
{
        delay( ms, 0x7ff );
}
