#include "mp3_controls.h"
#include "LiquidCrystal_I2C.h"
#include "mp3_decoder.h"
extern TaskHandle_t player_handle;
extern volatile uint8_t volume;
extern QueueHandle_t songName_Queue;
static size_t item_number = -1;
static uint16_t true_volume;
static int height = 2;
bool pause = false;
bool bass_treble_increase = true;

void control_buttons_pin_configurations(void) {
  volume_up = gpio__construct_with_function(GPIO__PORT_1, 20, GPIO__FUNCITON_0_IO_PIN);
  volume_down = gpio__construct_with_function(GPIO__PORT_1, 28, GPIO__FUNCITON_0_IO_PIN);
  play_pause_button = gpio__construct_with_function(GPIO__PORT_1, 23, GPIO__FUNCITON_0_IO_PIN);
  next_song = gpio__construct_with_function(GPIO__PORT_1, 29, GPIO__FUNCITON_0_IO_PIN);
  previous_song = gpio__construct_with_function(GPIO__PORT_0, 6, GPIO__FUNCITON_0_IO_PIN);
  bass = gpio__construct_with_function(GPIO__PORT_0, 8, GPIO__FUNCITON_0_IO_PIN);
  treble = gpio__construct_with_function(GPIO__PORT_0, 26, GPIO__FUNCITON_0_IO_PIN);
  bass_treble_direction = gpio__construct_with_function(GPIO__PORT_1, 31, GPIO__FUNCITON_0_IO_PIN);

  gpio__set_as_input(volume_up);
  gpio__set_as_input(volume_down);
  gpio__set_as_input(play_pause_button);
  gpio__set_as_input(next_song);
  gpio__set_as_input(previous_song);
  gpio__set_as_input(bass);
  gpio__set_as_input(treble);
  gpio__set_as_input(bass_treble_direction);
}

void volume_handler() {

  if (!gpio__get(volume_down)) {
    if (volume < 0xEB) {
      volume = volume + 0x13;
      true_volume = (volume << 8) | volume;
      write_16bits_on_decoder(0x0B, true_volume);
    }
    vTaskDelay(100);
  }

  if (!gpio__get(volume_up)) {
    if (volume > 0x13) {
      volume = volume - 0x13;
      true_volume = (volume << 8) | volume;
      write_16bits_on_decoder(0x0B, true_volume);
    }
    vTaskDelay(100);
  }
}

void base_treble_handler() {
  if (!gpio__get(bass_treble_direction)) {
    if (bass_treble_increase) {
      bass_treble_increase = false;
    } else {
      bass_treble_increase = true;
    }
    vTaskDelay(100);
  }

  if (!gpio__get(bass) && (!bass_treble_increase)) {
    mp3_decrease_bass();
    vTaskDelay(100);
  }

  if (!gpio__get(bass) && (bass_treble_increase)) {
    mp3_increase_bass();
    vTaskDelay(100);
  }

  if (!gpio__get(treble) && (!bass_treble_increase)) {
    mp3_decrease_treble();
    vTaskDelay(100);
  }

  if (!gpio__get(treble) && (bass_treble_increase)) {
    mp3_increase_treble();
    vTaskDelay(100);
  }
}

void play_pause_handler() {
  if (!pause) {
    if (!gpio__get(play_pause_button)) {
      vTaskSuspend(player_handle);
      pause = true;
      vTaskDelay(400);
    }
  }

  else {
    if (!gpio__get(play_pause_button)) {
      vTaskResume(player_handle);
      pause = false;
      vTaskDelay(400);
    }
  }
}

void play_next_song() {
  if (!gpio__get(next_song)) {
    if (item_number >= song_list__get_item_count() - 1) {
      item_number = -1;
    }
    xQueueSend(songName_Queue, song_list__get_name_for_item(++item_number), 0);

    vTaskDelay(400);
  }
}

void play_previous_song() {
  if (!gpio__get(previous_song)) {
    if (item_number <= 0) {
      item_number = song_list__get_item_count();
    }
    xQueueSend(songName_Queue, song_list__get_name_for_item(--item_number), 0);

    vTaskDelay(200);
  }
}