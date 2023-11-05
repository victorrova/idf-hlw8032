#include <stdio.h>

//FreeRTOS Essentials
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//ESP system Essentials
#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>

#include "components/hlw8032/include/hlw8032.h"

#define HLW8032_GPIO  GPIO_NUM_5

static hlw8032_t hlw8032;

void task_hlw8032_rcv(void* arg)
{
    esp_err_t err;

    while (1)
    {
        err = hlw8032_read(&hlw8032);

        if (err == ESP_OK && hlw8032.VoltageData != 0 && hlw8032.CurrentData != 0)
        {
            ESP_LOGI(__func__, "Voltage: %.3f", hlw8032_get_V(&hlw8032));
            ESP_LOGI(__func__, "Current: %.3f", hlw8032_get_I(&hlw8032));
            ESP_LOGI(__func__, "Power: %.3f", hlw8032_get_P_active(&hlw8032));
        }
        else if (err != ESP_OK)
        {
            ESP_LOGW(__func__, "Reading HLW8032 failed");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

esp_err_t hlw8032_init(gpio_num_t gpio_num)
{
    esp_err_t err = hlw8032_serial_begin(&hlw8032, UART_NUM_1, gpio_num, 256);
    hlw8032_set_I_coef_from_R(&hlw8032, 0.001);
    hlw8032_set_V_coef_from_R(&hlw8032, 1000000, 1000);

    return err;
}

void app_main(void) 
{
    ESP_LOGI("ESP", "Free heap: %d bytes", (int)esp_get_free_heap_size());
    ESP_LOGI("ESP", "IDF version: %s", esp_get_idf_version());

    hlw8032_init(HLW8032_GPIO);

    xTaskCreate(task_hlw8032_rcv, "hlw8032_rcv", 3*1024, NULL, 5, NULL);
}