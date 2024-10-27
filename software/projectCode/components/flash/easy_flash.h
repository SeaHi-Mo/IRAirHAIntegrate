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
    FLASH_MQTT_HOST,
    FLASH_MQTT_PORT,
    FLASH_MQTT_CLIENT_ID,
    FLASH_MQTT_USERNAME,
    FLASH_MQTT_PASSWORD,
    FLASH_HA_DEV_NAME,
    FLASH_HA_MANUFACTURER,
    FLASH_HA_AC_TYPE,
    FLASH_HA_AC_GCODE,
}flash_key_t;

bool flash_save_wifi_info(void* value);
int flash_get_wifi_info(void* value);
bool flash_save_mqtt_info(void* value);
int flash_get_mqtt_info(void* value);
bool flash_save_ha_device_msg(void* value);
int flash_get_ha_device_msg(void* value);
bool ef_del_key(const char* key);
bool flash_set_ir_code(const char* data_name, char* ir_data, unsigned short int data_len);
int  flash_get_ir_code(const char* data_name, char* ir_data, unsigned short int data_len);
bool flash_save_new_temp(float temperature);
float flash_get_temperature(void);
bool flash_save_new_ac_mode(uint8_t modes);
int flash_get_ac_mode(void);

bool flash_save_new_ac_type(int ac_type);
int flash_get_ac_type(void);

bool flash_save_new_ac_gcode(unsigned char* g_codec, int g_code_len);
int flash_get_ac_gcode(unsigned char* g_codec);
#endif