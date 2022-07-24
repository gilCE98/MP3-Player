#include "mp3_decoder.h"
static uint8_t treble;
static uint8_t bass;

void mp3_decoder_SCI_cs(void) { // chip select
  gpio__reset(CS);
}

void mp3_decoder_SCI_ds(void) { // deselect chip
  gpio__set(CS);
}
void mp3_decoder_SDI_cs(void) { // chip select
  gpio__reset(DCS);
}

void mp3_decoder_SDI_ds(void) { // deselect chip
  gpio__set(DCS);
}
void reset_decoder() {
  gpio__reset(RESET); // XRST low
}
void restart_decoder() {
  gpio__set(RESET); // XRST high
}

void pin_configuration_for_decoder() {
  CS = gpio__construct_with_function(GPIO__PORT_2, 2, GPIO__FUNCITON_0_IO_PIN);    // chip select for sci
  DCS = gpio__construct_with_function(GPIO__PORT_2, 5, GPIO__FUNCITON_0_IO_PIN);   // chip select for sdi
  DREQ = gpio__construct_with_function(GPIO__PORT_2, 7, GPIO__FUNCITON_0_IO_PIN);  // DREQ
  RESET = gpio__construct_with_function(GPIO__PORT_2, 9, GPIO__FUNCITON_0_IO_PIN); // RESET
  gpio__set_as_input(DREQ);                              // set DREQ pin from decoder as input for sjtwo
  gpio__set_as_output(CS);                               // Set chip select as output for sci
  gpio__set_as_output(DCS);                              // Set chip select as output for sdi
  gpio__set_as_output(RESET);                            // Set chip select as output for sdi
  gpio__construct_with_function(1, 0, GPIO__FUNCTION_4); // sck2
  gpio__construct_with_function(1, 1, GPIO__FUNCTION_4); // MOSI
  gpio__construct_with_function(1, 4, GPIO__FUNCTION_4); // MISO
}

bool decoder_is_requesting_data() { return gpio__get(DREQ); }

void write_16bits_on_decoder(uint8_t address, uint16_t data) {
  while (!decoder_is_requesting_data()) {
    ; // wait
  }
  mp3_decoder_SCI_cs();
  ssp2__exchange_byte(0x2); // Opcode for write
  ssp2__exchange_byte(address);
  ssp2__exchange_byte(data >> 8);
  ssp2__exchange_byte(data);
  while (!decoder_is_requesting_data()) {
    ; // wait
  }

  mp3_decoder_SCI_ds();
}
void decoder_init() {
  mp3_decoder_SCI_ds();
  mp3_decoder_SDI_ds();
  reset_decoder();
  delay__ms(100);
  restart_decoder();
  delay__ms(10);
  write_16bits_on_decoder(0x00, 0x4800); // SM_SDINEW =1
  delay__ms(10);
  write_16bits_on_decoder(0x0B, 0x4F4F); // setting volume to middle
  delay__ms(10);
  write_16bits_on_decoder(0x03, 0x6000); // clock multiplier
  delay__ms(10);
}

static void write_base_and_treble_value() {
  uint16_t reg_data = 0;
  reg_data |= (treble << 8);
  reg_data |= (bass << 0);
  write_16bits_on_decoder(SCI_BASS, reg_data);
}

void mp3_increase_treble() {
  uint8_t new_treble = (treble >> 4);
  if (new_treble == 0x07) {
    return;
  } else if (new_treble == 0x0F) {
    new_treble = 0x00;
  } else {
    new_treble += 0x01;
  }
  treble &= ~(0xF << 4);
  treble |= (new_treble << 4);
  write_base_and_treble_value();
}

void mp3_decrease_treble() {
  uint8_t new_treble = (treble >> 4);
  if (new_treble == 0x00) {
    new_treble = 0x0F;
  } else if (new_treble == 0x08) {
    return;
  } else {
    new_treble -= 0x01;
  }
  treble &= ~(0xF << 4);
  treble |= (new_treble << 4);
  write_base_and_treble_value();
}

void mp3_increase_bass() {
  uint8_t new_bass = (bass >> 4);
  if (new_bass == 0x0F) {
    return;
  } else {
    new_bass += 0x01;
    bass &= ~(0xF << 4);
    bass |= (new_bass << 4);
  }
  write_base_and_treble_value();
}

void mp3_decrease_bass() {
  uint8_t new_bass = (bass >> 4);
  if (new_bass == 0x00) {
    return;
  } else {
    new_bass -= 0x01;
    bass &= ~(0xF << 4);
    bass |= (new_bass << 4);
  }
  write_base_and_treble_value();
}