#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#define TICK_RATE_HZ configTICK_RATE_HZ
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include <stdio.h>
#include <esp_log.h>
#include <esp_system.h>

#define TAG "Btn_ISR_Semahore"
#define delay_time 50
#define Btn_GPIO GPIO_NUM_19
#define Led_GPIO GPIO_NUM_5
#define debounce_time_ms 200

SemaphoreHandle_t xSemaphore = NULL;
volatile TickType_t last_intr_time = 0;

//* ISR Handler
static void IRAM_ATTR gpio_isr_handler(void *args)
{

    TickType_t curr_time = xTaskGetTickCountFromISR();
    if ((curr_time - last_intr_time) > pdMS_TO_TICKS(debounce_time_ms))
    {
        last_intr_time = curr_time;
        BaseType_t xHighPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xSemaphore, &xHighPriorityTaskWoken);

        if (xHighPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }
}

//* Task that waits for semaphore & toggles the LED
void led_task(void *args)
{
    gpio_set_direction(Led_GPIO, GPIO_MODE_OUTPUT);
    bool led_state = false;

    while (1)
    {
        /* code */
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY))
        {
            led_state = !led_state;
            gpio_set_level(Led_GPIO, led_state);
            ESP_LOGI(TAG, "Led is now: %s", led_state ? "ON" : "OFF");
        }
    }
}

void app_main()
{
    xSemaphore = xSemaphoreCreateBinary();
    if (xSemaphore == NULL)
    {
        ESP_LOGE(TAG, "Semaphore creation failed");
        return;
    }

    // configure the btn pin

    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_NEGEDGE, // falling edge interrupt
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << Btn_GPIO),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    gpio_config(&io_config);

    //! install GPIO ISR & handler
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(Btn_GPIO, gpio_isr_handler, NULL);

    xTaskCreate(led_task, "Led Task", 2048, NULL, 2, NULL);
}