#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t xMutex; //Manejo del recurso compartido mutex
int classroomTemp = 0;  // variable compartida

void taskSensor(void* pvParameters) {
  (void) pvParameters;
  for (;;) {
    int val = analogRead(A1);
    xSemaphoreTake(xMutex, portMAX_DELAY);
    classroomTemp = val;  
    xSemaphoreGive(xMutex);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void taskLogger(void* pvParameters) {
  for (;;) {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    int temp_copy = classroomTemp;  
    xSemaphoreGive(xMutex);
    Serial.print("Temperatura: ");
    Serial.println(temp_copy);  // puede leer mientras se escribe
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setup() {
  Serial.begin(9600);
  xMutex = xSemaphoreCreateMutex(); //Cree un semáforo mutex
  xTaskCreate(taskSensor, "Sensor", 128, NULL, 1, NULL);
  xTaskCreate(taskLogger, "Logger", 128, NULL, 1, NULL);
  vTaskStartScheduler();
}

void loop() {}