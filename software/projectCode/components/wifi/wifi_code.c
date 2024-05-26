/**
 * @file wifi_code.c
 * @author Seahi-Mo (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include "blog.h"
#include "wifi_code.h"

static wifi_conf_t conf =
{
    .country_code = "CN",
};
