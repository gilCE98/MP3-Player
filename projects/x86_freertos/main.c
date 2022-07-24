#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cpu_utilization_task.h"

/**
 * This POSIX based FreeRTOS simulator is based on:
 * https://github.com/linvis/FreeRTOS-Sim
 *
 * Do not use for production!
 * There may be issues that need full validation of this project to make it production intent.
 * This is a great teaching tool though :)
 */
void print_hello_function(void) { puts("hello"); }
void print_world_function(void) { puts("world"); }

typedef void (*void_function_pointer_t)(void);

void call_function(int which_one) {
  void_function_pointer_t array_of_function_pointers[] = {print_hello_function, print_world_function};

  array_of_function_pointers[which_one];
  void_function_pointer_t function = array_of_function_pointers[which_one];
  // unction(123);
}

int main() {
  call_function(1);
  call_function(2);
  return 0;
}
