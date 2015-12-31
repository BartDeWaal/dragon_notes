#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

// define the pin a LED is attached to
// B is because this one is in the B block
#define LED PB5

double delaytime = 300;

int main(void) {
  DDRB |= (1<<LED);  

  while (1) {
    PORTB ^= (1<<LED);
    _delay_ms(delaytime);
  }
}
