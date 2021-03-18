/* 
  This programs shows the basic concepts of a 
  queue - From the lecture

  We can see how a queue can get full if we write to it more frequently
  than we read
*/
#include <Arduino.h>

// Use only one core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
// Set the length of the queue to 5
static const uint8_t msg_quque_len = 5;

// Globals
static QueueHandle_t msg_quque;

//****************************************************************************************
// Tasks

// Tasks: wait for item on queue and print it
void printMessages(void *parameters)
{
  int item;

  // Loop forever
  while (1)
  {
    // See if there is a message in the queue (do not block)
    // Read from the queue - last param is the tick timeout
    if (xQueueReceive(msg_quque, (void *)&item, 0) == pdTRUE)
    {
      //Serial.println(item);
    }
    Serial.println(item);

    // Wait before trying again
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // We can read faster then we write and then we will get the same values
  }
  
}
//********************************************************************************************
// Main ( runs as a task with priority one )
void setup() {
  // Configure serial
  Serial.begin(115200);

  // Wait a moment to start 
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---------Free RTOS Queue Demo----------");

  // Create quque
  msg_quque = xQueueCreate(msg_quque_len, sizeof(int));

  // Start print task
  xTaskCreatePinnedToCore(printMessages, 
                          "Print messages",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  

}

void loop() {

  static int num = 0;

  // Try to add item to queue for 10 ticks, fail if quque is full
  if (xQueueSend(msg_quque, (void *)&num, 0) != pdTRUE)
  {
    Serial.println("Queue full!");
  }

  num++;

  // Wait before trying again
  vTaskDelay(500 / portTICK_PERIOD_MS);
}
