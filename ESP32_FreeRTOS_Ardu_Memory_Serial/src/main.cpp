/*
    This program is part of the Digikey Course on FreeRTOS

    In this program we test the concepts of memory allocation using 
    tasks

    As the priority value increases, the task will execute first

*/


#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
 
// Task: Perform some mundane task
void testTask(void *parameter)
{
  while(1)
  {
    int a = 1;
    int b[100];

    // Do sth with the array so it's not optimized by the compiler
    for (int i = 0; i < 100; i++)
    {
      b[i] = a + 1;
    }
    Serial.println(b[0]);

    // Print out remaining stack memory (words)*4
    Serial.print("High water mark (bytes):");
    Serial.println(uxTaskGetStackHighWaterMark(NULL)*4);

    // Print out number of free heap memory bytes before malloc
    Serial.print("Heap size before malloc (bytes):");
    Serial.println(xPortGetFreeHeapSize());

    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

    // One way to prevent heap overflow is to check the malloc output
    if (ptr == NULL)
    {
      Serial.println("Not enough heap");
    }
    else
    {
      // Do sth with the memory so that the compiler don't optimize it
      for (int i = 0; i < 1024; i++)
      {
        ptr[i] = 3;
      }
    }

    vPortFree(ptr);

    // Print out number of free heap memory bytes after malloc
    Serial.print("Heap size before malloc (bytes):");
    Serial.println(xPortGetFreeHeapSize());

    // Wait for a while
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  
  
  }
}

void setup() {
  // Configure serial
  Serial.begin(115200);

  // Wait a moment to start
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("----FreeRTOS Memory Demo----");

  // Start the other only task
  xTaskCreatePinnedToCore(testTask,
                          "Test task",
                          1500,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  // Delete setup & loop
  vTaskDelete(NULL);
}

void loop() {
 
}