#include "uart_lab.h"
#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
//   uint32_t DLAB = (0b1 << 7);
//   uint32_t divider_16_bit = peripheral_clock / (16 * baud_rate);

//   switch (uart) {
//   case (UART_2): {
//     uint32_t uart2_power_bit = (0b1 << 24);
//     LPC_SC->PCONP |= uart2_power_bit;
//     LPC_UART2->LCR |= DLAB;

//     LPC_UART2->DLM = (divider_16_bit >> 8) & 0xFF;
//     LPC_UART2->DLL = (divider_16_bit)&0xFF;
//     LPC_UART2->LCR &= ~(1 << 7);

//     LPC_UART2->FCR |= 0b1;

//     LPC_IOCON->P2_8 = 0b010;
//     LPC_IOCON->P2_9 = 0b010;
//     // LPC_GPIO0->DIR |= (0b1 << 10);
//     LPC_GPIO2->DIR |= (1 << 8);
//     LPC_UART2->LCR |= (3 << 0); // set in 8 bit config mode
//   }
//   case (UART_3): {
//     uint32_t uart3_power_bit = (0b1 << 25);
//     LPC_SC->PCONP |= uart3_power_bit;
//     LPC_UART3->LCR |= DLAB;

//     LPC_UART3->DLM = (divider_16_bit >> 8) & 0xFF;
//     LPC_UART3->DLL = (divider_16_bit)&0xFF;
//     LPC_UART2->LCR &= ~(1 << 7);

//     LPC_UART3->FCR |= 0b1;

//     LPC_IOCON->P4_28 = 0b010;
//     LPC_IOCON->P4_29 = 0b010;
//     LPC_GPIO4->DIR |= (1 << 28);
//     LPC_UART3->LCR |= (3 << 0); // set in 8 bit config mode
//   }
//   }
// }

// // Read the byte from RBR and actually save it to the pointer
// bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
//   switch (uart) {
//   case UART_2:
//     while (!(LPC_UART2->LSR) & (1 << 0)) {
//       ;
//     }
//     *input_byte = LPC_UART2->RBR;

//   case UART_3:
//     while (!(LPC_UART3->LSR) & (1 << 0)) {
//       ;
//     }
//     *input_byte = LPC_UART3->RBR;
//   }
// }

// bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
//   switch (uart) {
//   case UART_2:
//     while (!(LPC_UART2->LSR) & (1 << 5)) {
//       ;
//     }
//     LPC_UART2->THR = output_byte;
//   case UART_3:
//     while (!(LPC_UART3->LSR) & (1 << 5)) {
//       ;
//     }
//     LPC_UART3->THR = output_byte;
//   }
// }
// // Private queue handle of our uart_lab.c
// static QueueHandle_t your_uart_rx_queue;

// void your_receive_interrupt(void) {
//   uint32_t intrpt_id;
//   if (LPC_UART2->IIR & (0 << 0)) {             // IF INTRPT
//     intrpt_id = LPC_UART2->IIR & (0b111 << 1); // IDENTIFY INTRPT
//     while (!(LPC_UART2->LSR) & (1 << 0)) {
//       const char byte = LPC_UART2->RBR;
//       xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
//     }
//   }
// }

// // Public function to enable UART interrupt
// // TODO Declare this at the header file
// void uart__enable_receive_interrupt(uart_number_e uart_number) {
//   lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, your_receive_interrupt, "TRACE");

//   LPC_UART2->IER |= (1 << 0);

//   your_uart_rx_queue = xQueueCreate(10, sizeof(char)); // length & item size
// }

// bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
//   return xQueueReceive(your_uart_rx_queue, input_byte, timeout);
// }

void uart_lab__init(uint32_t peripheral_clock, uint32_t baud_rate) {
  // Refer to LPC User manual and setup the register bits correctly
  // The first page of the UART chapter has good instructions
  // a) Power on Peripheral
  // b) Setup DLL, DLM, FDR, LCR registers
  LPC_SC->PCONP |= (1 << 24);
  uint8_t DLAB = 7;
  const uint16_t divider_16_bit = 96 * 1000 * 1000 / (16 * baud_rate);
  LPC_UART2->LCR |= (1 << DLAB);
  LPC_UART2->DLM = (divider_16_bit >> 8) & 0xFF;
  LPC_UART2->DLL = (divider_16_bit >> 0) & 0xFF;
  LPC_UART2->LCR &= ~(1 << DLAB);
  LPC_UART2->FCR |= 0b1;
  LPC_UART2->LCR |= (3 << 0); // set in 8 bit config mode
}

// Read the byte from RBR and actually save it to the pointer
bool uart_lab__polled_get(char *input_byte) {
  // a) Check LSR for Receive Data Ready
  while (!(LPC_UART2->LSR) & (1 << 0)) { // While is empty
    ;
  }
  // b) Copy data from RBR register to input_byte
  *input_byte = LPC_UART2->RBR;
}

bool uart_lab__polled_put(char output_byte) {
  // a) Check LSR for Transmit Hold Register Empty
  while (!(LPC_UART2->LSR) & (1 << 5)) { // While is empty
    ;
  }
  // b) Copy output_byte to THR register
  output_byte = LPC_UART2->THR;
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Private queue handle of our uart_lab.c
static QueueHandle_t your_uart_rx_queue;

// Private function of our uart_lab.c
static void your_receive_interrupt(void) {
  // TODO: Read the IIR register to figure out why you got interrupted

  if (LPC_UART2->IIR & (2 << 1)) {
    while (!(LPC_UART2->LSR) & (1 << 0)) {
      ;
    }
    const char byte = LPC_UART2->RBR;
    xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
  }
}

void uart__enable_receive_interrupt(uart_number_e uart_number) {
  // TODO: Use lpc_peripherals.h to attach your interrupt
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, your_receive_interrupt, "TRACE");

  // TODO: Enable UART receive interrupt by reading the LPC User manual
  // Hint: Read about the IER register
  LPC_UART2->IER |= (1 << 0);
  // TODO: Create your RX queue

  your_uart_rx_queue = xQueueCreate(10, sizeof(char));
}

// Public function to get a char from the queue (this function should work without modification)
// TODO: Declare this at the header file
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(your_uart_rx_queue, input_byte, timeout);
}