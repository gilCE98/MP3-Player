#pragma once
#include "gpio.h"
#include <stdint.h>

/* VS1053B V4  Registers */
#define SCI_MODE 0x00
#define SCI_STATUS 0x01
#define SCI_BASS 0x02
#define SCI_CLOCKF 0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA 0x05
#define SCI_WRAM 0x06
#define SCI_WRAMADDR 0x07
#define SCI_HDAT0 0x08
#define SCI_HDAT1 0x09
#define SCI_AIADDR 0x0A
#define SCI_VOL 0x0B
#define SCI_AICTRL0 0x0C
#define SCI_AICTRL1 0x0D
#define SCI_AICTRL2 0x0E
#define SCI_AICTRL3 0x0F
#define MAX_VOLUME 0x0000
#define MIN_VOLUME 0xFAFA

gpio_s CS;
gpio_s DCS;
gpio_s DREQ;
gpio_s RESET;

void mp3_decoder_SCI_cs(void);
void mp3_decoder_SCI_ds(void);
void mp3_decoder_SDI_cs(void);
void mp3_decoder_SDI_ds(void);
void reset_decoder();
void restart_decoder();
void pin_configuration_for_decoder();
bool decoder_is_requesting_data();
void write_16bits_on_decoder(uint8_t address, uint16_t data);
void decoder_init();
void mp3_increase_treble();
void mp3_decrease_treble();
void mp3_increase_bass();
void mp3_decrease_bass();
