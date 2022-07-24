#include "LiquidCrystal_I2C.h"
#include "adc.h"
#include "board_io.h"
#include "cli_handlers.h"
#include "common_macros.h"
#include "delay.h"
#include "ff.h"
#include "gpio.h"
#include "i2c.h"
// #include "i2c_slave.h"
#include "FreeRTOS.h"
#include "cli_handlers.h"
#include "ff.h"
#include "gpio.h"
#include "mp3_controls.h"
#include "mp3_decoder.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "sj2_cli.h"
#include "song_list.h"
#include "ssp2.h"
#include "task.h"
#include <acceleration.h>
#include <adc.h>
#include <assert.h>
#include <clock.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

volatile uint8_t volume = 0x4F;

TaskHandle_t player_handle;
QueueHandle_t songData_Queue;
QueueHandle_t songName_Queue;
QueueHandle_t LCD_songName_Queue;

typedef struct {
  char song_name[64];
} songName_s;

typedef struct {
  uint8_t song_data[512];
} songData_s;

typedef struct {
  char songnames[20][20];
} songlist_s;

songlist_s list;

songlist_s print_song_list(void) {
  int song_names = 0;
  for (size_t song_number = 0; song_number < song_list__get_item_count(); song_number++) {
    char *text = song_list__get_name_for_item(song_number);
    for (int j = 0; j < 20; j++) {
      list.songnames[song_number][j] = text[j];
    }
  }
  return list;
}

void mp3_reader_task(void *p) {
  songName_s song_to_play;
  FIL file; // object file
  FRESULT is_file_open;
  songData_s bytes;
  UINT br;

  while (1) {
    xQueueReceive(songName_Queue, song_to_play.song_name, portMAX_DELAY);
    xQueueSend(LCD_songName_Queue, song_to_play.song_name, 0);
    vTaskDelay(1);

    is_file_open = f_open(&file, song_to_play.song_name, FA_READ);
    if (FR_OK == is_file_open) {
      while (!(f_eof(&file))) {
        if (uxQueueMessagesWaiting(songName_Queue) > 0) {
          f_close(&file);
          break;
        }
        if (FR_OK == f_read(&file, bytes.song_data, sizeof(bytes.song_data), &br)) {
          xQueueSend(songData_Queue, &bytes, portMAX_DELAY);
        }
      }
      f_close(&file);
    } else {
      printf("Error: Failed to open.\n");
    }
  }
}

static void transfer_data_block(songData_s *mp3_playback_buffer) {
  for (int index = 0; index < 512; index++) {

    while (!decoder_is_requesting_data()) {
      ; // vTaskDelay(1);
    }
    mp3_decoder_SDI_cs();
    ssp2__exchange_byte(mp3_playback_buffer->song_data[index]);
    mp3_decoder_SDI_ds();
  }
}

static void mp3_player_task(void *parameter) {
  songData_s mp3_playback_buffer;
  while (1) {
    xQueueReceive(songData_Queue, &mp3_playback_buffer, portMAX_DELAY);
    transfer_data_block(&mp3_playback_buffer);
  }
}

void mp3_control(void *parameter) {

  while (1) {
    volume_handler();
    play_pause_handler();
    play_next_song();
    play_previous_song();
    base_treble_handler();
    vTaskDelay(100);
  }
}
static void LCD_screen() {
  songName_s song_to_display;
  while (1) {
    if (xQueueReceive(LCD_songName_Queue, song_to_display.song_name, portMAX_DELAY)) {
      display_song_name(song_to_display.song_name);
      button_icons();
    }
  }
}

int main(void) {
  sj2_cli__init();
  pin_configuration_for_decoder();
  control_buttons_pin_configurations();
  uint32_t clock_for_ssp_in_KHz = 1000;
  ssp2__initialize(clock_for_ssp_in_KHz);
  decoder_init();

  songName_Queue = xQueueCreate(1, sizeof(songName_s));
  songData_Queue = xQueueCreate(5, sizeof(songData_s));
  LCD_songName_Queue = xQueueCreate(1, sizeof(songName_s));

  songlist_s my_list;
  song_list__populate();
  my_list = print_song_list();

  pin_configuration_for_LCD();
  begin(20, 4);
  char songs[20][20];
  assert(sizeof(songs) == sizeof(my_list));
  memcpy(songs, &my_list, sizeof(songs));

  display_song_list(songs);
  // button_icons();

  // printf("Hello");
  xTaskCreate(mp3_control, "Control", (2 * 1024) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(mp3_reader_task, "Reader", (2 * 1024) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(mp3_player_task, "Player", (2 * 1024) / sizeof(void *), NULL, PRIORITY_HIGH, &player_handle);
  xTaskCreate(LCD_screen, "LCD", (5 * 1024) / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  vTaskStartScheduler();
  return 0;
}

// #include "LiquidCrystal_I2C.h"
// #include "adc.h"
// #include "board_io.h"
// #include "cli_handlers.h"
// #include "common_macros.h"
// #include "delay.h"
// #include "ff.h"
// #include "gpio.h"
// #include "i2c.h"
// // #include "i2c_slave.h"
// #include "FreeRTOS.h"
// #include "cli_handlers.h"
// #include "ff.h"
// #include "gpio.h"
// #include "mp3_controls.h"
// #include "mp3_decoder.h"
// #include "periodic_scheduler.h"
// #include "queue.h"
// #include "sj2_cli.h"
// #include "song_list.h"
// #include "ssp2.h"
// #include "task.h"
// #include <acceleration.h>
// #include <adc.h>
// #include <assert.h>
// #include <clock.h>
// #include <math.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <string.h>

// QueueHandle_t item;

// int x, y;

// static void producer() {
//   while (1) {
//     printf("Sending!\n");
//     xQueueSend(item, &x, 0); // Sending if queue is empty. If full stops
//     printf("Sent!\n");
//     // vTaskDelay(1000);
//   }
// }

// static void consumer() {
//   while (1) {
//     printf("Receiving!\n");
//     xQueueReceive(item, &y,
//                   portMAX_DELAY); // PORTMAXDELAY = Sleep until it receives something and other tasks are asleep
//     printf("Received!\n");

//   }
// }

// int main(void) {
//   item = xQueueCreate(1, sizeof(int));
//   xTaskCreate(producer, "Producer", (1 * 1024) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
//   xTaskCreate(consumer, "Consumer", (1 * 1024) / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

//   vTaskStartScheduler();
//   return 0;
// }

// static void create_blinky_tasks(void) {
//   /**
//    * Use '#if (1)' if you wish to observe how two tasks can blink LEDs
//    * Use '#if (0)' if you wish to use the 'periodic_scheduler.h' that will spawn 4 periodic tasks, one for each
//    LED
//    */
// #if (1)
//   // These variables should not go out of scope because the 'blink_task' will reference this memory
//   static gpio_s led0, led1;

//   // If you wish to avoid malloc, use xTaskCreateStatic() in place of xTaskCreate()
//   static StackType_t led0_task_stack[512 / sizeof(StackType_t)];
//   static StackType_t led1_task_stack[512 / sizeof(StackType_t)];
//   static StaticTask_t led0_task_struct;
//   static StaticTask_t led1_task_struct;

//   led0 = board_io__get_led0();
//   led1 = board_io__get_led1();

//   xTaskCreateStatic(blink_task, "led0", ARRAY_SIZE(led0_task_stack), (void *)&led0, PRIORITY_LOW,
//   led0_task_stack,
//                     &led0_task_struct);
//   xTaskCreateStatic(blink_task, "led1", ARRAY_SIZE(led1_task_stack), (void *)&led1, PRIORITY_LOW,
//   led1_task_stack,
//                     &led1_task_struct);
// #else
//   periodic_scheduler__initialize();
//   UNUSED(blink_task);
// #endif
// }

// static void create_uart_task(void) {
//   // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
//   // Change '#if (0)' to '#if (1)' and vice versa to try it out
// #if (0)
//   // printf() takes more stack space, size this tasks' stack higher
//   xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
// #else
//   sj2_cli__init();
//   UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
// #endif
// }

// static void blink_task(void *params) {
//   const gpio_s led = *((gpio_s *)params); // Parameter was input while calling xTaskCreate()

//   // Warning: This task starts with very minimal stack, so do not use printf() API here to avoid stack overflow
//   while (true) {
//     gpio__toggle(led);
//     vTaskDelay(500);
//   }
// }

// // This sends periodic messages over printf() which uses system_calls.c to send them to UART0
// static void uart_task(void *params) {
//   TickType_t previous_tick = 0;
//   TickType_t ticks = 0;

//   while (true) {
//     // This loop will repeat at precise task delay, even if the logic below takes variable amount of ticks
//     vTaskDelayUntil(&previous_tick, 2000);

//     /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire output will be fully
//      * sent out before this function returns. See system_calls.c for actual implementation.
//      *
//      * Use this style print for:
//      *  - Interrupts because you cannot use printf() inside an ISR
//      *    This is because regular printf() leads down to xQueueSend() that might block
//      *    but you cannot block inside an ISR hence the system might crash
//      *  - During debugging in case system crashes before all output of printf() is sent
//      */
//     ticks = xTaskGetTickCount();
//     fprintf(stderr, "%u: This is a polled version of printf used for debugging ... finished in",
//     (unsigned)ticks); fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

//     /* This deposits data to an outgoing queue and doesn't block the CPU
//      * Data will be sent later, but this function would return earlier
//      */
//     ticks = xTaskGetTickCount();
//     printf("This is a more efficient printf ... finished in");
//     printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
//   }
// }