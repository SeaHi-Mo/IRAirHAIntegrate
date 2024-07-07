/**
 * @file wifi_code.h
 * @author seahi-Mo (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef WIFI_CODE_H
#define WIFI_CODE_H

#include <wifi_mgmr_ext.h>
#include <hal_wifi.h>
#include "axk_ble.h"
typedef struct wifi_code_info {
    char ssid[64];
    char password[64];
    char mac[6];
    char pmk[64];
    uint8_t band;
    uint8_t chan_id;
    char ipv4_addr[16];
    uint32_t addr_ip;
}wifi_info_t;

void wifi_device_init(blufi_wifi_conn_event_cb_t cb);
void quick_connect_wifi(wifi_info_t* wifi_info);
bool wifi_device_connect_status(void);
#endif