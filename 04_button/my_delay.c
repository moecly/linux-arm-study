#include "my_delay.h"

void delay(unsigned int t) {
  for (; t > 0; t--) {
  }
}

void delay_ms(unsigned int t) {
  for (; t > 0; t--)
    delay(1000);
}
