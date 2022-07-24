#include <clock.h>
#include <gpio.h>
#include <lpc40xx.h>
#include <stdint.h>
#include <stdio.h>

void ssp2__init(uint32_t max_clock_mhz);

uint8_t ssp2__exchange_byte2(uint8_t data_out);