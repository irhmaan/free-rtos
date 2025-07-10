#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <esp_log.h>
#include <freertos/queue.h>
#include <string.h>

QueueHandle_t xQ = NULL;

typedef struct
{
    /* data */
    int temp;
    float pressure;
    char source_id[10];
} sensor_data;

void sender_task(void *param)
{
    int counter = 0;
    sensor_data data;
    while (1)
    {
        data.temp = 25 + (counter % 5);
        data.pressure = 1000 + (counter * 2);
        strcpy(data.source_id, "S1");
        // snprintf(data.source_id, sizeof(data.source_id), "S1");
        if (xQueueSend(xQ, &data, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI("Core Info: ", "ID: %d", xPortGetCoreID());
            // ESP_LOGI("xQ", "Sent-> Temp: %d, Pressure-> %f", data.temp, data.pressure);
            counter++;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sender_task2(void *param)
{
    int counter = 0;
    sensor_data data;
    while (1)
    {
        data.temp = 30 + (counter % 5);
        data.pressure = 900 + (counter * 2);
        strcpy(data.source_id, "S2");

        // snprintf(data.source_id, sizeof(data.source_id), "S2");
        if (xQueueSend(xQ, &data, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI("Core Info: ", "ID: %d", xPortGetCoreID());
            // ESP_LOGI("xQ", "Sent-> Temp: %d, Pressure-> %f", data.temp, data.pressure);
            counter++;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void receiver_task(void *param)
{
    sensor_data rData;
    while (1)
    {
        if (xQueueReceive(xQ, &rData, portMAX_DELAY))
        {
            ESP_LOGI("Data", "[Received From %s Temp: %d, Pressure-> %f", rData.source_id, rData.temp, rData.pressure);
            ESP_LOGI("Core Info: ", "ID: %d", xPortGetCoreID());
        }
    }
}

void app_main(void)
{
    xQ = xQueueCreate(5, sizeof(sensor_data));

    if (xQ == NULL)
    {
        ESP_LOGI("Err:", "Failed to create queue");
        return;
    }

    xTaskCreatePinnedToCore(sender_task, "Sender Task 1", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(sender_task2, "Sender Task 1", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(receiver_task, "Receiver Task", 2048, NULL, 2, NULL, 1);
}