#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#define TICK_RATE_HZ configTICK_RATE_HZ
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/timers.h>
#include <esp_log.h>
#include <esp_system.h>

#define tag "TIMER"
#define LED_Pin GPIO_NUM_5
#define Btn_GPIO GPIO_NUM_19

TimerHandle_t led_timer;
TimerHandle_t timeout_timer;

bool led_state = false;
static bool timer_active = false;

//! periodic timer cb: toggle gpio
void led_timer_cb(TimerHandle_t xTimer)
{
    led_state = !led_state;
    gpio_set_level(LED_Pin, led_state);
    ESP_LOGI(tag, "Led switched: %s", led_state ? "ON" : "OFF");
}

//! one-shot timer cb: turn off gpio

void timeout_timer_cb(TimerHandle_t xTimer)
{
    led_state = false;
    gpio_set_level(LED_Pin, led_state);
    ESP_LOGW(tag, "Led turned off by timeout");
    timer_active = false;
}

void btn_task(void *args)
{
    bool last_btn_state = true;

    while (1)
    {
        bool curr_btn_state = gpio_get_level(Btn_GPIO);

        if (!curr_btn_state && last_btn_state) // Falling edge
        {
            if (!timer_active)
            {
                ESP_LOGI(tag, "Button Pressed -> Starting One-shot Timer");
                xTimerStart(timeout_timer, 0);
                timer_active = true;
            }
        }

        last_btn_state = curr_btn_state;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{

    gpio_config_t io_config = {
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << Btn_GPIO),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,

    };

    gpio_config(&io_config);

    gpio_set_direction(LED_Pin, GPIO_MODE_DEF_OUTPUT);
    led_timer = xTimerCreate("Led Timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, led_timer_cb);

    if (led_timer == NULL)
    {
        ESP_LOGE(tag, "Failed to create LED timer");
        return;
    }

    timeout_timer = xTimerCreate("Timeout timer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timeout_timer_cb);

    if (timeout_timer == NULL)
    {
        ESP_LOGE(tag, "Failed to create Timeout timer");
        return;
    }

    xTaskCreate(btn_task, "Btn Task", 2048, NULL, 5, NULL);

    xTimerStart(led_timer, 0);
    // xTimerStart(timeout_timer, 0);
}