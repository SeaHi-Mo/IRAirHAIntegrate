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
#include <aos/yloop.h>
#include "lwip/dns.h"
#include <aos/kernel.h>
#include "device_state.h"
#include <arpa/inet.h>

#define PMK_LEN 64

static dev_msg_t dev_msg = { 0 };
static bool wifi_status = false;

extern blufi_wifi_conn_event_cb_t sg_blufi_conn_cb;

static void blufi_wifi_evt_export(int evt)
{
    if (sg_blufi_conn_cb)
    {
        sg_blufi_conn_cb(evt, NULL);
    }
}

static wifi_conf_t conf =
{
    .country_code = "CN",
};

void quick_connect_wifi(wifi_info_t* wifi_info)
{
    wifi_interface_t wifi_interface;
    uint8_t channel = 0;
    size_t len = 0;
    int quick_connect = 1;
    bool open_bss_flag = 1;
    uint32_t flags = 0;
    memset(&dev_msg.wifi_info, 0, sizeof(wifi_info_t));

    memset(wifi_info->pmk, 0, PMK_LEN);

    if (quick_connect > 0) {
        flags |= WIFI_CONNECT_STOP_SCAN_CURRENT_CHANNEL_IF_TARGET_AP_FOUND;
    }
    wifi_mgmr_psk_cal(
             wifi_info->password,
              wifi_info->ssid,
              strlen(wifi_info->ssid),
             wifi_info->pmk
    );
    memcpy(&dev_msg.wifi_info, wifi_info, sizeof(wifi_info_t));
    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_mac_get((uint8_t*)wifi_info->mac);
    wifi_mgmr_sta_connect_mid(wifi_interface, wifi_info->ssid, NULL, wifi_info->pmk, NULL, 0, wifi_info->chan_id, 1, flags);
}
/**
 * @brief wifi 回调函数
 *
 * @param event
 * @param private_data
*/
static void event_cb_wifi_event(input_event_t* event, void* private_data)
{
    static char* ssid;
    static char* password;

    wifi_mgmr_ap_item_t item = { 0 };
    switch (event->code)
    {
        case CODE_WIFI_ON_INIT_DONE:
        {
            blog_info("[APP] [EVT] INIT DONE %lld", aos_now_ms());
            wifi_mgmr_start_background(&conf);
        }
        break;
        case CODE_WIFI_ON_MGMR_DONE:
        {
            blog_info("[APP] [EVT] MGMR DONE %lld", aos_now_ms());
            // //_connect_wifi();

            wifi_mgmr_scan(NULL, NULL);
        }
        break;
        case CODE_WIFI_ON_SCAN_DONE:
        {
            blog_info("[APP] [EVT] SCAN Done %lld", aos_now_ms());
            // wifi_mgmr_cli_scanlist();

            dev_msg.device_state = DEVICE_SATE_SYSYTEM_INIT;
            device_state_update(true, &dev_msg); //WiFi 准备OK,等待连接
        }
        break;
        case CODE_WIFI_ON_DISCONNECT:
        {
            blog_info("[APP] [EVT] disconnect %lld", aos_now_ms());
            if (ble_is_connected)
                blufi_wifi_evt_export(BLUFI_STATION_DISCONNECTED);
        }
        break;
        case CODE_WIFI_ON_CONNECTING:
        {
            blog_info("[APP] [EVT] Connecting %lld", aos_now_ms());
        }
        break;
        case CODE_WIFI_CMD_RECONNECT:
        {
            blog_info("[APP] [EVT] Reconnect %lld", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_CONNECTED:
        {
            blog_info("[APP] [EVT] connected %lld", aos_now_ms());
            if (ble_is_connected)
                blufi_wifi_evt_export(BLUFI_STATION_CONNECTED);
        }
        break;
        case CODE_WIFI_ON_PRE_GOT_IP:
        {
            blog_info("[APP] [EVT] connected %lld", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_GOT_IP:
        {
            blog_info("[APP] [EVT] GOT IP %lld", aos_now_ms());
            blog_info("[SYS] Memory left is %d Bytes", xPortGetFreeHeapSize());
            if (ble_is_connected)
                blufi_wifi_evt_export(BLUFI_STATION_GOT_IP);
            wifi_status = true;
            dev_msg.device_state = DEVICE_STATE_WIFI_CONNECTED;
            memset(dev_msg.wifi_info.ipv4_addr, 0, 16);
            uint32_t  gw, mask;
            wifi_mgmr_sta_ip_get(&dev_msg.wifi_info.addr_ip, &gw, &mask);
            wifi_mgmr_sta_connect_ind_stat_info_t wifi_info;
            wifi_mgmr_sta_connect_ind_stat_get(&wifi_info);
            dev_msg.wifi_info.band = wifi_info.chan_band;
            dev_msg.wifi_info.chan_id = wifi_info.chan_id;
            strcpy(dev_msg.wifi_info.ssid, wifi_info.ssid);

            if (dev_msg.wifi_info.addr_ip!=0) {
                strcpy(dev_msg.wifi_info.ipv4_addr, ip4addr_ntoa(&dev_msg.wifi_info.addr_ip));
            }
            device_state_update(true, &dev_msg); //WiFi 准备OK,等待连接
        }
        break;
        case CODE_WIFI_ON_PROV_SSID:
        {
            blog_info("[APP] [EVT] [PROV] [SSID] %lld: %s",
                   aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (ssid)
            {
                vPortFree(ssid);
                ssid = NULL;
            }
            ssid = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_BSSID:
        {
            blog_info("[APP] [EVT] [PROV] [BSSID] %lld: %s",
                   aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (event->value)
            {
                vPortFree((void*)event->value);
            }
        }
        break;
        case CODE_WIFI_ON_PROV_PASSWD:
        {
            blog_info("[APP] [EVT] [PROV] [PASSWD] %lld: %s", aos_now_ms(),
                   event->value ? (const char*)event->value : "UNKNOWN");
            if (password)
            {
                vPortFree(password);
                password = NULL;
            }
            password = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_CONNECT:
        {
            blog_info("[APP] [EVT] [PROV] [CONNECT] %lld", aos_now_ms());
            blog_info("connecting to %s:%s...", ssid, password);
            // wifi_sta_connect(ssid, password);
        }
        break;
        case CODE_WIFI_ON_PROV_DISCONNECT:
        {
            blog_info("[APP] [EVT] [PROV] [DISCONNECT] %lld", aos_now_ms());
        }
        break;
        default:
        {
            blog_info("[APP] [EVT] Unknown code %u, %lld", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static void proc_main_entry(void* pvParameters)
{

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);
    hal_wifi_start_firmware_task();
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
    dns_init();
    ip_addr_t dns_addr;
    ip4addr_aton("223.5.5.5", &dns_addr);
    dns_setserver(1, &dns_addr);
    vTaskDelete(NULL);
}

void wifi_device_init(blufi_wifi_conn_event_cb_t cb)
{
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
    sg_blufi_conn_cb = cb;
}

bool wifi_device_connect_status(void)
{
    return wifi_status;
}