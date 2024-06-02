/**
 * @file easy_flash.c
 * @author SeaHi-Mo(you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#include<stdio.h>
#include <stdbool.h>
#include<string.h>
#include "blog.h"
#include <easyflash.h>
#include <easy_flash.h>
#include "wifi_code.h"

static char* flash_key[] = { "ssid","pass","pmk","band","chan_id" };

static bool ef_set_bytes(const char* key, char* value, int len) {
    int result = ef_set_env_blob(key, value, len);
    return result == EF_NO_ERR ? true : false;
}

static int ef_get_bytes(const char* key, char* value, int len) {
    size_t read_len = 0;
    int result = ef_get_env_blob(key, value, len, &read_len);
    return read_len;
}

bool ef_del_key(const char* key) {
    return ef_del_env(key);
}

bool flash_save_wifi_info(void* value)
{
    wifi_info_t* wifi_info = (wifi_info_t*)value;
    int result = 0;

    if (strlen(wifi_info->ssid)!=0)
        result = ef_set_bytes(flash_key[FLASH_WIFI_SSID], wifi_info->ssid, strlen(wifi_info->ssid));
    if (strlen(wifi_info->password)!=0)
        result = ef_set_bytes(flash_key[FLASH_WIFI_PASSWORD], wifi_info->password, strlen(wifi_info->password));
    if (strlen(wifi_info->pmk)!=0)
        result = ef_set_bytes(flash_key[FLASH_WIFI_PMK], wifi_info->pmk, strlen(wifi_info->pmk));
    if (wifi_info->band)
        result = ef_set_bytes(flash_key[FLASH_WIFI_BAND], (char*)&wifi_info->band, sizeof(wifi_info->band));
    if (wifi_info->chan_id)
        result = ef_set_bytes(flash_key[FLASH_WIFI_CHAN_ID], (char*)&wifi_info->chan_id, sizeof(wifi_info->chan_id));

    return result == EF_NO_ERR ? true : false;
}

int flash_get_wifi_info(void* value)
{

    if (value==NULL) {
        return -1;
    }
    int result = 0;
    size_t read_len = 0;
    wifi_info_t* wifi_info = (wifi_info_t*)value;
    memset(wifi_info->ssid, 0, 64);
    memset(wifi_info->password, 0, 64);
    memset(wifi_info->pmk, 0, 64);
    result = ef_get_bytes(flash_key[FLASH_WIFI_SSID], wifi_info->ssid, sizeof(wifi_info->ssid));
    wifi_info->ssid[result] = '\0';
    result = ef_get_bytes(flash_key[FLASH_WIFI_PASSWORD], wifi_info->password, 64);
    wifi_info->password[result] = '\0';
    result = ef_get_bytes(flash_key[FLASH_WIFI_PMK], wifi_info->pmk, 64);
    return result;

}