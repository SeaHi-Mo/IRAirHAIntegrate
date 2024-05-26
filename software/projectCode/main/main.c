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
#include "homeAssistantPort.h"
#include "at_receive.h"

void main()
{

    blog_info("[OS] Starting TCP/IP Stack...");
    tcpip_init(NULL, NULL);
    easyflash_init();
    blog_info("[OS] proc_main_entry task...");
    atUartInit(115200);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}
