#include "LiquidCrystal_I2C.h"
#include "delay.h"
#include "i2c.h"
#include <inttypes.h>

uint8_t _Addr;
uint8_t _displaycontrol;
uint8_t _displayfunction = 0x00 | LCD_2LINE | LCD_5x8DOTS;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _cols;
uint8_t _rows;
uint8_t _backlightval;

// init function
void begin(uint8_t cols, uint8_t lines) {
  _numlines = lines;
  delay__ms(50); // DONT DIVIDE *

  expanderWrite(_backlightval); // turn the backlight off
  delay__ms(1000);
  write4bits(0x03 << 4);
  delay__ms(5);

  write4bits(0x03 << 4);
  delay__ms(5);

  write4bits(0x03 << 4);
  delay__ms(1);

  write4bits(0x02 << 4);

  command(LCD_FUNCTIONSET | _displayfunction); // config LCD

  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();
  clear();
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
  backlight();
  home();
}

void clear() {
  command(LCD_CLEARDISPLAY);
  delay__ms(2);
}

void home() {
  command(LCD_RETURNHOME); // CURSOR HOME
  delay__ms(2);            // LONG DELAY
}

void setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > (_numlines - 1)) {
    row = _numlines - 1;
  }
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void scrollDisplayLeft(void) { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT); }
void scrollDisplayRight(void) { command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT); }

void leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    send(charmap[i], Rs);
  }
}

void noBacklight(void) {
  _backlightval = LCD_NOBACKLIGHT;
  expanderWrite(0);
}

void backlight(void) {
  _backlightval = LCD_BACKLIGHT;
  expanderWrite(0);
}

inline void command(uint8_t hex) { send(hex, 0); }

void send(uint8_t data, uint8_t mode) {
  uint8_t highnib = data & 0xf0;
  uint8_t lownib = (data << 4) & 0xf0;
  write4bits((highnib) | mode);
  write4bits((lownib) | mode);
}

void write4bits(uint8_t data) {
  expanderWrite(data);
  pulseEnable(data);
}

void expanderWrite(uint8_t _data) {
  i2c__write_LCD_data(2, 0x4E, _data | _backlightval); // I2C FUNCTION W/O THE REG NUM **
}

void pulseEnable(uint8_t _data) {
  expanderWrite(_data | En);
  delay__ms(1);

  expanderWrite(_data & ~En);
  delay__ms(1);
}

void cursor_on() { cursor(); }

void cursor_off() { noCursor(); }

void blink_on() { blink(); }

void blink_off() { noBlink(); }

void load_custom_character(uint8_t char_num, uint8_t *rows) { createChar(char_num, rows); }

void setBacklight(uint8_t backlight_val) {
  if (backlight_val) {
    backlight();
  } else {
    noBacklight();
  }
}

void animation() {
  // if mutex / song playing
  char pause_motion[] = {0x00, 0x00, 0x0A, 0x15, 0x00, 0x00, 0x00, 0x00};

  char motion_1[] = {0x00, 0x02, 0x0A, 0x15, 0x00, 0x00, 0x00, 0x00};

  char motion_2[] = {0x00, 0x0A, 0x0A, 0x15, 0x00, 0x00, 0x00, 0x00};

  char motion_3[] = {0x00, 0x08, 0x08, 0x17, 0x00, 0x00, 0x00, 0x00};

  char motion_4[] = {0x00, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00, 0x00};

  setCursor(1, 1);
  // load_custom_character(0, pause_motion); // REGISTER
  load_custom_character(2, motion_1);
  // load_custom_character(2, motion_2);
  load_custom_character(0, motion_3);
  load_custom_character(1, motion_4);
  home();
  while (true) {
    for (int i = 0; i < 2; i++) {
      setCursor(0, 1);
      if (i == 0) {
        send(2, Rs);
        send(2, Rs);
        send(2, Rs);
        delay__ms(1000);
        continue;
      }
      if (i == 1) {
        send(0, Rs);
        send(0, Rs);
        send(0, Rs);
        delay__ms(1000);
        continue;
      }
      if (i == 2) {
        send(1, Rs);
        send(1, Rs);
        send(1, Rs);
        delay__ms(1000);
        continue;
      }
      if (i == 3) {
        send(2, Rs);
        send(2, Rs);
        send(2, Rs);
        delay__ms(1000);
        continue;
      }
      if (i == 4) {
        send(0, Rs);
        send(0, Rs);
        send(0, Rs);
        delay__ms(1000);
        continue;
      }
    }
  }
}
void volume_display(int height) {
  setCursor(16, height);
  send(4, Rs);
}

void button_icons() {
  char play[] = {0x00, 0x08, 0x0C, 0x0A, 0x0C, 0x08, 0x00, 0x00};
  char pause[] = {0x00, 0x04, 0x0A, 0x11, 0x0A, 0x04, 0x00, 0x00};
  char skip[] = {0x00, 0x04, 0x02, 0x1D, 0x02, 0x04, 0x00, 0x00};

  char back[] = {0x00, 0x04, 0x08, 0x17, 0x08, 0x04, 0x00, 0x00};
  char vol[] = {0x00, 0x04, 0x0E, 0x04, 0x00, 0x0E, 0x00, 0x00};

  load_custom_character(5, play); // REGISTER
  load_custom_character(6, pause);
  load_custom_character(7, skip);
  load_custom_character(3, back);
  load_custom_character(4, vol);
  home();

  // load_custom_character(1, s);
  // home();

  setCursor(13, 3);
  send(3, Rs);

  setCursor(14, 3);
  send(5, Rs);

  setCursor(15, 3);
  send(6, Rs);

  setCursor(16, 3);
  send(4, Rs);

  setCursor(17, 3);
  send(7, Rs);
}

void display_song_name(char *text) {
  clear();
  setCursor(0, 0);
  int max_chars = 13; // limit song name to 13 char
  for (int i = 0; i < max_chars; i++) {
    if (strlen(text) == i) // if less than max char limit, stop printing
      break;
    if (text[i] == '.') // if .mp3 is part of name
      break;
    if (i == max_chars - 1 &&
        strlen(text) > max_chars) { // if greater than max char limit, trim end by 3 chars and add "..."
      send('-', Rs);
      break;
    }
    send(text[i], Rs);
  }
}

void display_song_list(char song_list[20][20]) {
  setCursor(0, 0);
  send('Y', Rs);
  send('O', Rs);
  send('U', Rs);
  send('R', Rs);
  send(' ', Rs);
  send('S', Rs);
  send('0', Rs);
  send('N', Rs);
  send('G', Rs);
  send('S', Rs);
  delay__ms(500);
  send('.', Rs);
  delay__ms(500);
  send('.', Rs);
  delay__ms(500);
  send('.', Rs);
  delay__ms(500);
  clear();

  int temp = 0;            // used for resetting screen on 4th song
  int max_chars = 20;      // limit song name to 20 char
  bool flag = true;        // reset screen if true (upon 4th song)
  bool break_flag = false; // if no more songs in list, set break flag & break

  for (int i = 0; i < 20; i++) {
    int j = 0;
    if (flag) {
      setCursor(0, 0);
      flag = false;
    }

    for (j = 0; j < max_chars; j++) {
      if (strlen(song_list[i]) == 0) {
        break_flag = true;
        break;
      }

      if (strlen(song_list[i]) == j) // if too short
        break;

      if (song_list[i][j] == '.') // if .mp3 is part of name
        break;

      if (j == max_chars - 1) {
        send('-', Rs);
        break;
      }

      send(song_list[i][j], Rs);
    }

    if (break_flag) {
      delay__ms(1000);
      setCursor(0, 0);
      clear();
      break;
    }

    delay__ms(1000);
    setCursor(0, ++temp);

    if (temp == 4) {
      flag = true;
      temp = 0;
      delay__ms(500);
      clear();
    }
  }
}

void pin_configuration_for_LCD() {
  gpio__construct_with_function(GPIO__PORT_0, 11, GPIO__FUNCTION_2); // SCL
  gpio__construct_with_function(GPIO__PORT_0, 10, GPIO__FUNCTION_2); // SDA
}