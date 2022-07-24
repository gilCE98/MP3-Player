#include "ssp2_lab.h"
#include <stdint.h>
#include <stdio.h>
void ssp2__init(uint32_t max_clock_mhz) {
  // a) Power on Peripheral
  LPC_SC->PCONP |= (1 << 20); // Table 16: Configure SSP2

  // b) Setup control registers CR0 and CR1
  const uint32_t scr_value = 0;
  LPC_SSP2->CR0 = (0b111 << 0) | (scr_value << 8);

  const uint32_t ssp2_enable = (1 << 1);
  LPC_SSP2->CR1 = ssp2_enable;
  // c) Setup prescalar register to be <= max_clock_mhz
  // PCLK must be less then PCLK / 2
  uint8_t divider = 2;
  uint8_t prescalar_register = clock__get_core_clock_hz() / 1000UL / 1000UL;
  while (max_clock_mhz <= (prescalar_register / divider)) {
    divider += 2;
  }
  LPC_SSP2->CPSR = divider;
}

uint8_t ssp2__exchange_byte_lab(uint8_t data_out) {
  // Configure the Data register(DR) to send and receive data by checking the SPI peripheral status register
  LPC_SSP2->DR = data_out;

  while (LPC_SSP2->SR & (1 << 4)) {
    ; // Waiting
  }

  return (uint8_t)(LPC_SSP2->DR & 0xFF);
}