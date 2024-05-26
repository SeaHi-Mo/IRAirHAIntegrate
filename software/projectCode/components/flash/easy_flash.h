/**
 * @file easy_flash.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef EASY_FLASH_H
#define EASY_FLASH_H

typedef enum {
    FLASH_WIFI_SSID,
    FLASH_WIFI_PASSWORD,
    FLASH_WIFI_PMK,
    FLASH_WIFI_BAND,
    FLASH_WIFI_CHAN_ID,
}flash_key_t;

bool flash_save_wifi_info(void* value);
int flash_get_wifi_info(void* value);
#endif