/*
    This program is part of the Digikey Course on FreeRTOS

    In this program we read from the serial a number and update 
    the delay of the blinking LED

    As the priority value increases, the task will execute first

*/

#include <Arduino.h>
#include <stdlib.h> //for atoi()

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// buffer length
const uint8_t buf_len = 20;
// Pins
static const int led_pin = 2;
// Globals
static int led_delay = 500;

//*****************************************************
// Tasks

// Task: blink at rate set by Global var
void toggleLED(void *parameter){
    while(1){
        digitalWrite(led_pin, HIGH);
        vTaskDelay(led_delay / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(led_delay / portTICK_PERIOD_MS);
    } 
}

// Task: Read from serial terminal
// Feel free to use Serial.readString() or Serial.parseInt(). I'm going to show
// it with atoi() in case you're doing this in a non-Arduino environment. You'd
// also need to replace Serial with your own UART code for non-Arduino.
void readSerial(void *parameters) 
{
  // Declare a char c to read one char at a time
  char c;
  // Create a buffer of length buf_len
  char buf[buf_len];
  uint8_t idx = 0;

  // Clear whole buffer
  memset(buf, 0, buf_len);

  // Loop forever
  while (1)
  {
    // Read char from serial
    if (Serial.available() > 0)
    { 
      // Read one char at a time
      c = Serial.read();

      // Update delay variable and reset buffer if we get a newline char
      if (c == '\n')
      { 
        // Set delay to buffer and convert to int 
        led_delay = atoi(buf);
        Serial.print("Update LED delay to: ");
        Serial.println(led_delay);

        // Clear buffer
        memset(buf, 0, buf_len);
        // Set index to zero
        idx = 0;
      }
      else
      {
        // Only append if index is not over message limit 
        if (idx < buf_len - 1)
        {
          buf[idx] = c;
          //increase index in buffer
          idx++;
        }
      }   
    }
  }
}

//*****************************************************
//Main

void setup()
{
  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Configure serial and wait for 2 seconds
  Serial.begin(115200);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  Serial.println("Multi-task LED Demo");
  Serial.println("Enter a number in ms to change the LED delay: ");

  // Start blinking task
  xTaskCreatePinnedToCore(    // Use xTaskCreate in vanilla FreeRTOS
                toggleLED,
                "Toggle LED",
                1024,
                NULL,
                1,
                NULL,
                app_cpu);
  
  // Start serial read task
    xTaskCreatePinnedToCore(    
                readSerial,
                "Read serial",
                1024,
                NULL,
                2,              // priority must be the same to prevent lockup
                NULL,
                app_cpu);

  // Delete "setup & loop" task
  vTaskDelete(NULL);
}

void loop()
{
  // Execution should never get here
}







