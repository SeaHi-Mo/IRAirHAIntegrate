/**
 * @file blufi_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef BLUFI_CONFIG_H
#define BLUFI_CONFIG_H

#include "blufi.h"
#include "blufi_api.h"
#include "blufi_hal.h"
#include "blufi_init.h"
#include "axk_blufi.h"
#include "ble_interface.h"
#include "blufi_security.h"
#include "wifi_interface.h"

extern bool ble_is_connected;

void blufi_config_start(void);
void blufi_wifi_init(void);
void blufi_wifi_event(int event, void* param);
#endif