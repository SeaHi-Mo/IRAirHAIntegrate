/**
 * @file blufi_config.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blog.h>
#include "device_state.h"
#include "blufi_config.h"

#define BLUFI_NAME "HA_IRremote"

blufi_config_t g_blufi_config = { 0 };
static dev_msg_t dev_msg = { 0 };
static int scan_counter;
bool ble_is_connected = false;
static bool gl_sta_connected = false;

void blufi_wifi_event(int event, void* param)
{
    switch (event)
    {

        case BLUFI_STATION_CONNECTED:
            gl_sta_connected = true;
            break;
        case BLUFI_STATION_DISCONNECTED:
            gl_sta_connected = false;
            break;
        case BLUFI_STATION_GOT_IP:
        {
            axk_blufi_extra_info_t info;
            memset(&info, 0, sizeof(axk_blufi_extra_info_t));

            wifi_conn_ap_info_get(&g_blufi_config.wifi.sta);

            memcpy(info.sta_bssid, g_blufi_config.wifi.sta.cwjap_param.bssid, 6);
            info.sta_bssid_set = true;
            info.sta_ssid = (uint8_t*)g_blufi_config.wifi.sta.cwjap_param.ssid;
            info.sta_ssid_len = strlen(g_blufi_config.wifi.sta.cwjap_param.ssid);

            if (ble_is_connected == true)
            {
                axk_blufi_send_wifi_conn_report(g_blufi_config.wifi.cwmode, _BLUFI_STA_CONN_SUCCESS, 0, &info);
            }
            else
            {
                blog_info("BLUFI BLE is not connected yet");
            }
            //blog_info("BLUFI save ssid&&pwd ");
            g_blufi_config.wifi.cwmode = WIFIMODE_STA;
            dev_msg.device_state = DEVICE_STATE_ATCMD_WIFICFG_SET;
            device_state_update(false, &dev_msg);
        }
        break;
        default:
            break;
    }
}

static void example_event_callback(_blufi_cb_event_t event, _blufi_cb_param_t* param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as a example, we do it more simply */
    switch (event)
    {
        case AXK_BLUFI_EVENT_INIT_FINISH:
            blog_info("BLUFI init finish,ble name=%s", g_blufi_config.ble.blufi.blufiname);
            axk_blufi_adv_start();
            break;
        case AXK_BLUFI_EVENT_DEINIT_FINISH:
            blog_info("BLUFI deinit finish");
            break;
        case AXK_BLUFI_EVENT_BLE_CONNECT:
            blog_info("BLUFI ble connect");
            ble_is_connected = true;
            axk_blufi_adv_stop();
            blufi_security_init();
            break;
        case AXK_BLUFI_EVENT_BLE_DISCONNECT:
            blog_info("BLUFI ble disconnect");
            ble_is_connected = false;
            blufi_security_deinit();
            axk_blufi_profile_deinit();
            axk_hal_blufi_deinit();
            axk_blufi_adv_stop();
            axk_hal_ble_role_set(BLE_ROLE_DEINIT);
            break;
        case AXK_BLUFI_EVENT_SET_WIFI_OPMODE:
            blog_info("BLUFI Set WIFI opmode %d", param->wifi_mode.op_mode);
            // if (axk_hal_wifi_mode_set(WIFIMODE_STA, 0) != BLUFI_ERR_SUCCESS)
            // {
            //    blog_info("BLUFI axk_hal_wifi_mode_set fail\r");
            //     break;
            // }
            g_blufi_config.wifi.cwmode = WIFIMODE_STA;
            break;
        case AXK_BLUFI_EVENT_REQ_CONNECT_TO_AP:
        {
            cwjap_param_t cwjap_param = { 0 };
            //blog_info("BLUFI requset wifi connect to AP");
            cwjap_param = g_blufi_config.wifi.sta.cwjap_param;
            if (axk_hal_conn_ap_info_set(&cwjap_param) != BLUFI_ERR_SUCCESS)
            {
                blog_info("BLUFI axk_hal_conn_ap_info_set fail");
                break;
            }
            g_blufi_config.wifi.sta.state = BLUFI_WIFI_STATE_CONNECTING;
        }

        break;
        case AXK_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
            //blog_info("BLUFI requset wifi disconnect from AP");
            axk_hal_disconn_ap();
            break;
        case AXK_BLUFI_EVENT_REPORT_ERROR:
            blog_info("BLUFI report error, error code %d", param->report_error.state);
            axk_blufi_send_error_info(param->report_error.state);
            break;
        case AXK_BLUFI_EVENT_GET_WIFI_STATUS:
        {
            wifi_mode_t mode;
            mode = g_blufi_config.wifi.cwmode;

            if (gl_sta_connected)
            {
                axk_blufi_extra_info_t info;
                memset(&info, 0, sizeof(axk_blufi_extra_info_t));
                wifi_conn_ap_info_get(&g_blufi_config.wifi.sta);
                memcpy(info.sta_bssid, g_blufi_config.wifi.sta.cwjap_param.bssid, 6);
                info.sta_bssid_set = true;
                info.sta_ssid = (uint8_t*)g_blufi_config.wifi.sta.cwjap_param.ssid;
                info.sta_ssid_len = strlen(g_blufi_config.wifi.sta.cwjap_param.ssid);
                axk_blufi_send_wifi_conn_report(mode, _BLUFI_STA_CONN_SUCCESS, 0, &info);
            }
            else
            {
                axk_blufi_send_wifi_conn_report(mode, _BLUFI_STA_CONN_FAIL, 0, NULL);
            }
            blog_info("BLUFI get wifi status from AP");

            break;
        }
        case AXK_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
            //blog_info("blufi close a gatt connection");
            axk_blufi_disconnect();
            break;
        case AXK_BLUFI_EVENT_DEAUTHENTICATE_STA:
            /* TODO */
            break;
        case AXK_BLUFI_EVENT_RECV_STA_BSSID:
            memset(g_blufi_config.wifi.sta.cwjap_param.bssid, 0, 6);
            memcpy(g_blufi_config.wifi.sta.cwjap_param.bssid, param->sta_bssid.bssid, 6);
            // sta_config.sta.bssid_set = 1;
            // esp_wifi_set_config(WIFI_IF_STA, &sta_config);
            blog_info("Recv STA BSSID %s", param->sta_bssid.bssid);
            break;
        case AXK_BLUFI_EVENT_RECV_STA_SSID:
            memset(g_blufi_config.wifi.sta.cwjap_param.ssid, 0, 33);
            strncpy(g_blufi_config.wifi.sta.cwjap_param.ssid, (char*)param->sta_ssid.ssid, param->sta_ssid.ssid_len);

            blog_info("Recv STA SSID %s", (char*)g_blufi_config.wifi.sta.cwjap_param.ssid);
            strncpy(dev_msg.wifi_info.ssid, g_blufi_config.wifi.sta.cwjap_param.ssid, strlen(g_blufi_config.wifi.sta.cwjap_param.ssid));
            break;
        case AXK_BLUFI_EVENT_RECV_STA_PASSWD:
            memset(g_blufi_config.wifi.sta.cwjap_param.pwd, 0, 64);
            strncpy(g_blufi_config.wifi.sta.cwjap_param.pwd, (char*)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
            blog_info("Recv STA PASSWORD %s", (char*)g_blufi_config.wifi.sta.cwjap_param.pwd);
            strncpy(dev_msg.wifi_info.password, g_blufi_config.wifi.sta.cwjap_param.pwd, strlen(g_blufi_config.wifi.sta.cwjap_param.pwd));
            break;
        case AXK_BLUFI_EVENT_RECV_SOFTAP_SSID:
            break;
        case AXK_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
            break;
        case AXK_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM:
            break;
        case AXK_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE:
            break;
        case AXK_BLUFI_EVENT_RECV_SOFTAP_CHANNEL:
            break;
        case AXK_BLUFI_EVENT_GET_WIFI_LIST:
            // wifi_scan_start();
            break;
        case AXK_BLUFI_EVENT_RECV_CUSTOM_DATA:
            blog_info("Recv Custom Data len:%d", param->custom_data.data_len);
            blog_info("Custom Data:%.*s", param->custom_data.data_len, param->custom_data.data);
            // echo
            axk_blufi_send_custom_data(param->custom_data.data, param->custom_data.data_len);
            break;
        case AXK_BLUFI_EVENT_RECV_USERNAME:
            /* Not handle currently */
            break;
        case AXK_BLUFI_EVENT_RECV_CA_CERT:
            /* Not handle currently */
            break;
        case AXK_BLUFI_EVENT_RECV_CLIENT_CERT:
            /* Not handle currently */
            break;
        case AXK_BLUFI_EVENT_RECV_SERVER_CERT:
            /* Not handle currently */
            break;
        case AXK_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
            /* Not handle currently */
            break;
            ;
        case AXK_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
            /* Not handle currently */
            break;
        default:
            break;
    }
}

static _blufi_callbacks_t _callbacks = {
    .event_cb = example_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

void blufi_config_start(void)
{
    int ret = -1;
    memset(g_blufi_config.ble.blufi.blufiname, 0, 31);
    strcpy(g_blufi_config.ble.blufi.blufiname, BLUFI_NAME);
    axk_hal_blufi_init();
    ret = _blufi_host_and_cb_init(&_callbacks);
    if (ret)
    {
        blog_error("%s initialise failed: %d", __func__, ret);
    }
}

