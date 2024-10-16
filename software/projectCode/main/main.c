/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "blog.h"
#include <aos/yloop.h>
#include <aos/kernel.h>
#include <easyflash.h>
#include <lwip/tcpip.h>
#include <wifi_mgmr_ext.h>
#include <hal_wifi.h>
#include "bl_gpio.h"
#include "homeAssistantPort.h"

#include "device_state.h"

void main()
{
    bl_sys_init();
    blog_set_level_log_component(BLOG_LEVEL_DEBUG, "main");
    easyflash_init();
    device_state_init(NULL);
    blog_info("[OS] Starting TCP/IP Stack...");
    tcpip_init(NULL, NULL);
    blog_info("[OS] proc_main_entry task...");

    while (1)
    {
        blog_debug("HeapSize=%d ", xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
