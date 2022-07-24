#include "gpio_lab.h"
#include "lpc40xx.h"

void gpio0set_as_input(uint8_t pin_port, uint8_t pin_num) {
  if (pin_port == 0) {
    LPC_GPIO0->DIR &= ~(1 << pin_num);
  } else if (pin_port == 1) {
    LPC_GPIO1->DIR &= ~(1 << pin_num);
  } else if (pin_port == 2) {
    LPC_GPIO2->DIR &= ~(1 << pin_num);
  }
}

void gpio0set_as_output(uint8_t pin_port, uint8_t pin_num) {
  switch (pin_port) {
  case 0:
    LPC_GPIO0->DIR |= (1 << pin_num);
    break;
  case 1:
    LPC_GPIO1->DIR |= (1 << pin_num);
    break;
  case 2:
    LPC_GPIO2->DIR |= (1 << pin_num);
    break;
  default:
    break;
  }
}

void gpio0set_high(uint8_t pin_port, uint8_t pin_num) {
  switch (pin_port) {
  case 0:
    LPC_GPIO0->PIN |= (1 << pin_num);
    break;
  case 1:
    LPC_GPIO1->PIN |= (1 << pin_num);
    break;
  case 2:
    LPC_GPIO2->PIN |= (1 << pin_num);
    break;
  default:
    break;
  }
}

void gpio0set_low(uint8_t pin_port, uint8_t pin_num) {
  switch (pin_port) {
  case 0:
    LPC_GPIO0->PIN &= ~(1 << pin_num);
    break;
  case 1:
    LPC_GPIO1->PIN &= ~(1 << pin_num);
    break;
  case 2:
    LPC_GPIO2->PIN &= ~(1 << pin_num);
    break;
  default:
    break;
  }
}

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpio0__set(uint8_t pin_port, uint8_t pin_num, bool high) {
  if (high)
    gpio0set_high(pin_port, pin_num);
  else
    gpio0set_low(pin_port, pin_num);
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpio0__get_level(uint8_t pin_port, uint8_t pin_num) {
  if (pin_port == 0) {
    if (LPC_GPIO0->PIN & (1 << pin_num))
      return true;
    else
      return false;
  } else if (pin_port == 1) {
    if (LPC_GPIO1->PIN & (1 << pin_num))
      return true;
    else
      return false;
  } else if (pin_port == 2) {
    if (LPC_GPIO2->PIN & (1 << pin_num))
      return true;
    else
      return false;
  }
}