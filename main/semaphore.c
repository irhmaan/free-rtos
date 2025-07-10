#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#define TICK_RATE_HZ configTICK_RATE_HZ
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include <stdio.h>
#include <esp_log.h>
#include <esp_system.h>

#define TAG "Button_Led"
#define delay_time 50
#define Btn_GPIO GPIO_NUM_19
#define Led_GPIO GPIO_NUM_5

SemaphoreHandle_t xSemaphore;

void btn_task(void *arg)
{
    gpio_set_direction(Btn_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(Btn_GPIO, GPIO_PULLUP_ONLY);
    bool last_state = true;

    while (1)
    {
        bool curr_state = gpio_get_level(Btn_GPIO);

        if (last_state == 1 && curr_state == false)
        {
            ESP_LOGI(TAG, "Btn Pressed");
            xSemaphoreGive(xSemaphore);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        last_state = curr_state;
        vTaskDelay(pdMS_TO_TICKS(delay_time));
    }
}

void led_task(void *args)
{
    gpio_set_direction(Led_GPIO, GPIO_MODE_OUTPUT);
    bool led_state = false;
    while (1)
    {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY))
        {
            led_state = !led_state;
            gpio_set_level(Led_GPIO, led_state);
            ESP_LOGI(TAG, "Led toggeled: %s", led_state ? " ON" : " OFF");
        }
    }
}

void app_main(void)
{
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create binary semaphore");
        return;
    }

    xTaskCreate(btn_task, "Button Task", 2048, NULL, 2, NULL);
    xTaskCreate(led_task, "Led Ta sk", 2048, NULL, 2, NULL);
}