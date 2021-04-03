// if working with vanilla freeRtos 
// #include semaphr.h

#include <Arduino.h>
// Use only one core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static int shared_var = 0;
static SemaphoreHandle_t mutex; 

//Incrementing task
void inc_task(void *parameters){
  int local_var;

  // loop forever
  while(1)
  {
    // Check if mutex is available -> (this is non blocking)
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
      local_var = shared_var;
      local_var++;
      vTaskDelay(random(100, 500) / portTICK_PERIOD_MS);
      shared_var = local_var;

      // Give mutex after critical section
      xSemaphoreGive(mutex);

      // Print out the new shared var
      Serial.println(shared_var);
    }
    else
    {
      // Do something else
    }
  }

}

void setup() {
  Serial.begin(115200);

  // wait a moment to start
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.println("-----FreeRTOS Mutex Demo-----");
  Serial.println();

  // Create mutex before starting tasks
  mutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(inc_task,
                          "inc_task_1",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);


  xTaskCreatePinnedToCore(inc_task,
                          "inc_task_2",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:
}