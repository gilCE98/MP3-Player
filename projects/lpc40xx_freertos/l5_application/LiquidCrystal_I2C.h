#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include "gpio.h"
#include <inttypes.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100 // Enable bit
#define Rw 0b00000010 // Read/Write bit
#define Rs 0b00000001 // Register select bit

void begin(uint8_t cols, uint8_t rows);
void clear();
void home();
void noDisplay();
void display();
void noBlink();
void blink();
void noCursor();
void cursor();
void scrollDisplayLeft();
void scrollDisplayRight();
void printLeft();
void printRight();
void leftToRight();
void rightToLeft();
void shiftIncrement();
void shiftDecrement();
void noBacklight();
void backlight();
void autoscroll();
void noAutoscroll();
void createChar(uint8_t, uint8_t[]);
void setCursor(uint8_t, uint8_t);

void command(uint8_t); //*
void init();

void blink_on();
void blink_off();
void cursor_on();
void cursor_off();
void setBacklight(uint8_t);                     //*
void load_custom_character(uint8_t, uint8_t *); //*

void send(uint8_t, uint8_t);          //*
void write4bits(uint8_t);             //*
void expanderWrite(uint8_t);          //*
void pulseEnable(uint8_t);            //*
void display_song_name(char *);       //*
void display_song_list(char[20][20]); //*
void button_icons();                  //*
void animation();                     //*
void pin_configuration_for_LCD();     //*
void volume_display(int);

#endif