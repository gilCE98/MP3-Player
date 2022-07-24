#pragma once
#include "FreeRTOS.h"
#include "gpio.h"
#include "queue.h"
#include "song_list.h"
#include "task.h"
#include <adc.h>
#include <lpc40xx.h>
#include <stdint.h>

gpio_s play_pause_button;
gpio_s volume_up;
gpio_s volume_down;
gpio_s next_song;
gpio_s previous_song;
gpio_s bass;
gpio_s treble;
gpio_s bass_treble_direction;

void control_buttons_pin_configurations(void);
void play_pause_handler(void);
void play_next_song(void);
void volume_handler(void);
void play_previous_song(void);
void base_treble_handler(void);