/**
 * @file button.c
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
#include "button.h"

static void ir_buttom_task(void* asr)
{
    while (1)
    {
        if (!bl_gpio_input_get_value(IR_DEVICE_LEARN_BUTTON)) {
            while (!bl_gpio_input_get_value(IR_DEVICE_LEARN_BUTTON))
            {
                vTaskDelay(pdMS_TO_TICKS(30));
            }
            //启动学习
            blog_debug("start learning ........");
            ir_codec_start_learn();
        }

        if (!bl_gpio_input_get_value(IR_DEVICE_NET_COONFIG_BUTTON)) {
            while (!bl_gpio_input_get_value(IR_DEVICE_NET_COONFIG_BUTTON))
            {
                vTaskDelay(pdMS_TO_TICKS(30));
            }
            //启动配网
            blog_debug("start netconfig ........");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}

void device_button_init(void)
{
    bl_gpio_enable_input(IR_DEVICE_LEARN_BUTTON, 0, 0);
    bl_gpio_enable_input(IR_DEVICE_NET_COONFIG_BUTTON, 0, 0);
    xTaskCreate(ir_buttom_task, "learn task", 256, NULL, 9, NULL);
}