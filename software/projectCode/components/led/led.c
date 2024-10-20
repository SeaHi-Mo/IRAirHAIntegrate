/**
 * @file led.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <bl_gpio.h>
#include <blog.h>
#include <device_state.h>
#include "led.h"

static TaskHandle_t led_task;
static led_state_t led_state;

static void led_status_task(void* arg)
{
    uint16_t timer_ms = 0;
    while (1)
    {
        switch (led_state)
        {
            case LED_STATE_BLINK_0_5:
                timer_ms = 500;
                break;
            case LED_STATE_BLINK_1_0:
                timer_ms = 1000;
                break;
            case LED_STATE_BLINK_1_5:
                timer_ms = 1500;
                break;
            case LED_STATE_BLINK_2_0:
                timer_ms = 2000;
                break;
            case LED_STATE_BLINK_2_0_S_0_5:
                timer_ms = 80;
                bl_gpio_output_set(NET_WORKER_LED, 1);
                vTaskDelay(pdMS_TO_TICKS(2000));
                bl_gpio_output_set(NET_WORKER_LED, 0);
                vTaskDelay(pdMS_TO_TICKS(timer_ms));
                bl_gpio_output_set(NET_WORKER_LED, 1);
                vTaskDelay(pdMS_TO_TICKS(timer_ms));
                break;
            default:
                continue;
        }
        bl_gpio_output_set(NET_WORKER_LED, 0);
        vTaskDelay(pdMS_TO_TICKS(timer_ms));
        bl_gpio_output_set(NET_WORKER_LED, 1);
        vTaskDelay(pdMS_TO_TICKS(timer_ms));
    }
}

void device_led_init(void)
{
    bl_gpio_enable_output(NET_WORKER_LED, true, false);

    bl_gpio_output_set(NET_WORKER_LED, 1);
    xTaskCreate(led_status_task, "net led task", 1024, NULL, 11, &led_task);
}

void device_led_update_state(led_state_t led_ste)
{
    led_state = led_ste;
}