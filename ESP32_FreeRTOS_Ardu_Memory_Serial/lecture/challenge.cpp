/*
    This program is part of the Digikey Course on FreeRTOS

    In this program we read from the serial a number and update 
    the delay of the blinking LED

    As the priority value increases, the task will execute first

*/

#include <Arduino.h>
#include <stdlib.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings 
static const uint8_t buf_len = 255;

// Globals
static char *msg_ptr = NULL;
static volatile uint8_t msg_flag = 0;

//***********************************************************
// Tasks

// Task: read message from Serial Buffer
void read_serial(void *parameters)
{
  char c;
  char buff[buf_len];
  uint8_t idx = 0;

  // Clean buffer
  memset(buff, 0, buf_len);

  // Loop forever
  while (1)
  {
    // Read chars from serial
    if (Serial.available() > 0)
    {
      c = Serial.read();

      // Store recieved char to buffer if not over buffer limit
      if (idx < buf_len - 1)
      {
        buff[idx] = c;
        idx++;
      }

      // Create a message buffer for print task
      if (c == '\n')
      {
            // Replace '/n' with '/0'
            buff[idx-1] = '\0';

            // Try to allocate and copy over message. If buffer is still in 
            // use, ignore the entire message
            if (msg_flag == 0)
            {
            // Allocate a pointer of size idx*char for the message
            msg_ptr = (char*)pvPortMalloc(idx*sizeof(char));

            // If malloc returns 0 (out of mem), throw an error and reset
            configASSERT(msg_ptr);

            // Copy message to pointer
            memcpy(msg_ptr, buff, idx);

            // Notify other task that message is ready
            msg_flag = 1;
            }

            // Reset recieve buffer and index counter
            memset(buff, 0, buf_len);
            idx = 0;
      }
    }
  }
}

// Task: print message when flag is set and free buffer
void print_message(void *parameters)
{
  while(1)
  {
    // Wait for flag to be set and print message
    if (msg_flag == 1)
    {
      Serial.println(msg_ptr);

      // Print amount of free heap memory
      Serial.print("Free heap (bytes): ");
      Serial.println(xPortGetFreeHeapSize());

      //Free buffer, set pointer to null, and clear flag
      vPortFree(msg_ptr);
      msg_ptr = NULL;
      msg_flag = 0;
    }
  }
}

//******************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup()
{
  Serial.begin(115200);

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Heap Demo---");
  Serial.println("Enter string");

  // Start Serial recieve task
  xTaskCreatePinnedToCore(
                    read_serial,
                    "Read Serial",
                    1024,
                    NULL,
                    1,
                    NULL,
                    app_cpu
  );

  // Start Serial print task
  xTaskCreatePinnedToCore(
                    print_message,
                    "Print Message",
                    1024,
                    NULL,
                    1,
                    NULL,
                    app_cpu
  );
  // Delete setup
  vTaskDelete(NULL);
}


void loop()
{

}