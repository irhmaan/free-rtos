
// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <sdkconfig.h>

#define LED1 GPIO_NUM_5
#define LED2 GPIO_NUM_10

static uint8_t l1_s = 0;
static uint8_t l2_s = 1;

static void async_blink(void *param)
{
    // ESP_LOGI("CONFIG: ", "%s", param);
    gpio_num_t pin = (gpio_num_t)param;
    esp_rom_gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_DEF_OUTPUT);

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = (pin == LED1) ? pdMS_TO_TICKS(500) : pdMS_TO_TICKS(300);
    bool level = 0;

    while (1)
    {
        TickType_t now = xTaskGetTickCount();

        if ((now - lastWakeTime) >= interval)
        {
            ESP_LOGI("INFO", "Switching GPIO %d: %s", pin, level ? "ON" : "OFF");
            level = !level;
            gpio_set_level(pin, level);
            lastWakeTime = now;
        }
        // switch to other task briefly
        vTaskDelay(pdMS_TO_TICKS(10));
        // vTaskDelayUntil(&lastWakeTime, interval);
    }
}

void app_main(void)
{

    xTaskCreate(&async_blink, "Blink Task 1", 2048, (void *)LED1, 1, NULL);

    xTaskCreatePinnedToCore(&async_blink, "Blink task 2", 2048, (void *)LED2, 1, NULL, 1);
}