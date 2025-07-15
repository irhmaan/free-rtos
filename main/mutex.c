#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define tag "Mutex"

SemaphoreHandle_t xMutex;

void sensor_task_1(void *param)
{
    int counter = 0;
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY))
        {
            ESP_LOGI(tag, "[Sensor 1] Temp: %d, Pressure: %d", 25 + (counter % 3), 1000 + (counter * 2));
            vTaskDelay(pdMS_TO_TICKS(500));
            xSemaphoreGive(xMutex);
        }
        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sensor_task_2(void *param)
{
    int counter = 0;
    while (1)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY))
        {
            ESP_LOGI(tag, "[Sensor 2] Temp: %d, Pressure: %d", 25 + (counter % 4), 950 + (counter * 2));
            vTaskDelay(pdMS_TO_TICKS(500));
            xSemaphoreGive(xMutex);
        }
        counter++;
        vTaskDelay(pdMS_TO_TICKS(1200));
    }
}

void app_main(void)
{
    xMutex = xSemaphoreCreateMutex();

    if (xMutex == NULL)
    {
        ESP_LOGE(tag, "Failed to create mutex");
        return;
    }

    xTaskCreatePinnedToCore(sensor_task_1, "Sensor Task 1", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(sensor_task_2, "Sensor Task 2", 2048, NULL, 2, NULL, 1);
}