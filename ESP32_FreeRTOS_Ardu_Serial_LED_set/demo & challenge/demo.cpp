/*
    This program is part of the Digikey Course on FreeRTOS

    In this program we blink the LED in 2 tasks with different
    delay values. We set the priority of one of them to be higher
    to observe a overlay after some time

    As the priority value increases, the task will execute first

*/

#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const int led_pin = 2;

// LED rates
static const int rate_1 = 500; // ms
static const int rate_2 = 323; // ms

// Our task: blink an LED
void toggleLED_1(void *parameter)
{
    while(1)
    {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(rate_1 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    } 
}

// Our task: blink an LED
void toggleLED_2(void *parameter)
{
    while(1)
    {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(rate_2 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    } 
}


void setup() 
{    
    // Configure pin
    pinMode(led_pin, OUTPUT);

    // Pin a task 1 to run forever
    xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
        toggleLED_1,       // Function to be called
        "Toggle LED",    // Name of task
        1024,            // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,           // Parameter to pass to function
        2,              // Task priority (0 to  configMAX_PRIORITIES-1)
        NULL,           // Task handle
        app_cpu);       // Run on one core (ESP32 only)

    // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler()
    // in main after setting up your tasks
 

    xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
        toggleLED_2,       // Function to be called
        "Toggle LED",    // Name of task
        1024,            // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,           // Parameter to pass to function
        0,              // Task priority (0 to  configMAX_PRIORITIES-1)
        NULL,           // Task handle
        app_cpu);       // Run on one core (ESP32 only)

    // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler()
    // in main after setting up your tasks
}

void loop()
{

}
