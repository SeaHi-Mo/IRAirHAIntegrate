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
static dev_msg_t dev_msg = { 0 };
extern bool __hxd039b_busy;
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
            // ir_codec_start_learn();

            dev_msg.device_state = DEVICE_STATE_START_IR_LEARN;
            device_state_update(false, &dev_msg);
        }

        if (!bl_gpio_input_get_value(IR_DEVICE_NET_COONFIG_BUTTON)) {
            uint8_t timer_out = 0;
            while (!bl_gpio_input_get_value(IR_DEVICE_NET_COONFIG_BUTTON))
            {
                timer_out++;
                vTaskDelay(pdMS_TO_TICKS(30));
                if (timer_out>=67) {
                    break;
                }
            }
            //启动配网
            // 
            if (timer_out>=67) {
                timer_out = 0;
                blog_debug("start netconfig ........");
                dev_msg.device_state = DEVICE_STATE_BLUFI_CONFIG;
                device_state_update(false, &dev_msg); //WiFi 准备OK,等待连接
            }
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