/* Sollution to 05 - Queue Challenge
 * One tasks performs basic echo on serial. If it sees a delay followed by 
 * a number, it sends the number (in a queue) to the second task. If it recieves
 * a message in the secound queue, it prints it to the console. The 2nd task 
 * blinks an LED. When it gets a message from the 1st queue (number) , it updates
 * the blink delay to that number. Whenever the LED blinks 100 times, the 2nd task 
 * sends a message to the first task to be printed.
 
*/
#include<Arduino.h>
// Use only one core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 255;       // Size of the buffer to look for command
static const char command[] = "delay ";   // Note the space!
static const int delay_queue_len = 5;     // Size of the delay_queue
static const int msg_queue_len = 5;       // Size of msg_queue
static const uint8_t blink_max = 100;     // Num times to blink before message

// Pins 
static const int led_pin = 2;

// Message struct: used to wrap strings
typedef struct message {
  char body[20];
  int count;

}message;

// Globals
static QueueHandle_t delay_queue;
static QueueHandle_t msg_queue;

//**********************************************************************
// Tasks

// Task: command line interface (CLI)
/* This task will do the following
 *    - Listen on the serial
 *    - If there is a message on the message_queue - > print it on the serial
 *    - Check for 'delay' cmd and put the delay on the delay_queue
 *    - echo all from serial to the screen
 */
void doCLI (void *parameters){
  
  message rcv_msg;
  char c;
  char buff[buf_len];
  uint8_t idx = 0;
  uint8_t cmd_len = strlen(command);
  int led_delay;

  // Clear whole buffer
  memset(buff, 0, buf_len);

  // Loop forever
  while (1) {
    
    // Print msg recieved in the queue (do not block)
    if (xQueueReceive(msg_queue, (void*)&rcv_msg, 0) == pdTRUE)
    {
      Serial.print(rcv_msg.body);
      Serial.print(rcv_msg.count);
      Serial.println();
    }

    // Read char from serial
    if (Serial.available() > 0)
    {
      c = Serial.read();
    
      // Store rcv char to buffer id not over buffer limit
      if (idx < buf_len - 1){
        buff[idx] = c;
        idx++;
      }

      // Print newline and check input on 'enter'
      if ((c == '\n') || (c == '\r')){

        // Print newline to termial
        Serial.print("\r\n");

        // Check if the first chars are "delay"
        if (memcmp(buff, command, cmd_len) == 0){
          
          // Convert last part to positive int (negative int crashes)
          char* tail = buff + cmd_len; // pointer arithmetics -> it shifts the pointer to the delay val
          led_delay = atoi(tail);
          led_delay = abs(led_delay);

          // Send int to other task via queue
          if (xQueueSend(delay_queue, (void*)&led_delay, 10) != pdTRUE){
            Serial.println("ERROR: could not put item on the queue.");
          }
        }
        
        // Reset recieve buffer & index counter
        memset(buff, 0, buf_len);
        idx = 0;

      }
        // Otherwise, echo char back to serial terminal
      else{
        Serial.print(c);
      }
    }
  }
}

// Task: flash LED based on delay provided, notify other task every 100 blinks
/* This task will do the following
 *    - Check for delay val on the queue & print Message recieved on the screen
 *    - Update blink val
 *    - Print Blinked after 100 msg
 */
void blinkLED(void *parameters){

  message msg;
  // Initial led delay
  int led_delay = 500;
  uint8_t counter = 0;

  // Set up pin
  pinMode(led_pin, OUTPUT);

  // Loop forever
  while (1)
  {
    // See if there is a message in the queue (do not block)
    if (xQueueReceive(delay_queue, (void*)&led_delay, 0) == pdTRUE)
    {
      // Best practice: use only one task to manage serial comms
      strcpy(msg.body, "Message Recieved ");
      msg.count = 1;
      xQueueSend(msg_queue, (void*)&msg, 10);
    }

    // Blink
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);

    // If led blinked 100 times, send a msg to other task
    counter++;
    if(counter >= blink_max){
      // Construct message & send
      strcpy(msg.body, "Blinked");
      msg.count = counter;
      xQueueSend(msg_queue, (void*)&msg, 10);

      // Reset counter
      counter = 0;
    }
  }
}

//*********************************************************************
// Main (runs as its own task with priority 1 on core 1)
void setup()
{
  Serial.begin(115200);

  // Wait for a moment to start
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("------FreeRTOS Queue Solution------");
  Serial.println("Enter the command 'delay xxx' where xxx is the desired");
  Serial.println("LED blink time in sec");

  // Create queues
  delay_queue = xQueueCreate(delay_queue_len, sizeof(int));
  msg_queue = xQueueCreate(msg_queue_len, sizeof(message));

  // Start CLI Task
  xTaskCreatePinnedToCore(doCLI,
                          "CLI",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Start blink task
  xTaskCreatePinnedToCore(blinkLED,
                          "blinkLED",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  // Delete setup & loop tasks
  vTaskDelete(NULL);
}

void loop()
{

}
