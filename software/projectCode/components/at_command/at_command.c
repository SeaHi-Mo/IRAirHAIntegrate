/**
 * @file at_command.c
 * @author seahi-mo (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <string.h>
#include <at_command.h>
#include <blog.h>
#include "easy_flash.h"

#include "device_state.h"
static dev_msg_t dev_msg = {
      .device_state = DEVICE_STATE_ATCMD_WIFICFG_SET,
};
static int containsNChar(const char* str, char c)
{
    int i = 1;
    while (*str!='\0') {  // 遍历字符串直到遇到结束符'\0'  
        if (*str == c) {
            i++; // 如果找到匹配的字符，返回true  
        }
        str++;  // 移动到下一个字符  
    }
    return i;  // 如果没有找到匹配的字符，返回false  
}
/**
 * @brief WiFi设置指令 AT+AT+WIFIJAP=ssid,password,<pmk>,<band>,<chan_id>
 *
 * @param cmd
 * @param cmd_len
*/
static void at_wifi_set(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_debug("%s is ok", cmd);
    dev_msg.device_state = DEVICE_STATE_ATCMD_WIFICFG_SET;
    //识别参数数量
    int command_cnt = containsNChar(cmd, ',');
    if (command_cnt<2) {
        blog_error("AT numble <2");
        AT_RESPONSE(AT_ERR);
        return;
    }
    //解析指令
    char* cmd_list[32] = { 0 };
    cmd_list[0] = strtok(cmd, ",");
    blog_debug("%d command=%s", 1, cmd_list[0]);
    for (size_t i = 1; i <command_cnt; i++)
    {
        cmd_list[i] = strtok(NULL, ",");
        // blog_debug("%d command=%s", i+1, cmd_list[i]);
    }
    memset(dev_msg.wifi_info.ssid, 0, sizeof(dev_msg.wifi_info.ssid));
    memset(dev_msg.wifi_info.password, 0, sizeof(dev_msg.wifi_info.password));
    memset(dev_msg.wifi_info.pmk, 0, sizeof(dev_msg.wifi_info.pmk));

    strncpy(dev_msg.wifi_info.ssid, cmd_list[0], strlen(cmd_list[0]));
    strncpy(dev_msg.wifi_info.password, cmd_list[1], strlen(cmd_list[1]));
    switch (command_cnt) {
        case 3:

            strncpy(dev_msg.wifi_info.pmk, cmd_list[2], strlen(cmd_list[2]));

            break;
        case 4:
            if (cmd_list[2]!=NULL) strncpy(dev_msg.wifi_info.pmk, cmd_list[2], strlen(cmd_list[2]));
            dev_msg.wifi_info.band = atoi(cmd_list[3]);
            break;
        case 5:
            if (cmd_list[2]!=NULL) strncpy(dev_msg.wifi_info.pmk, cmd_list[2], strlen(cmd_list[2]));
            if (cmd_list[3]!=NULL) dev_msg.wifi_info.band = atoi(cmd_list[3]);
            dev_msg.wifi_info.chan_id = atoi(cmd_list[4]);
            break;
    }
    // flash_save_wifi_info(&dev_msg.wifi_info);
    //检查扫描列表是否有该SSID 存在
    for (size_t i = WIFI_MGMR_SCAN_ITEMS_MAX-1; i >0; i--)
    {
        //识别到该设备之后，发起连接
        if (!memcmp(wifiMgmr.scan_items[i].ssid, dev_msg.wifi_info.ssid, strlen(dev_msg.wifi_info.ssid))) {
            dev_msg.wifi_info.band = wifiMgmr.wifi_mgmr_stat_info.chan_band;
            dev_msg.wifi_info.chan_id = wifiMgmr.scan_items[i].channel;
            device_state_update(false, &dev_msg);
            AT_RESPONSE(AT_OK);
            return;
        }
    }

    // vPortFree(dev_msg.data);
    AT_RESPONSE(AT_ERR);
}

static void at_wifi_check(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_info("%s is ok", cmd);
    wifi_info_t wifi_info;
    flash_get_wifi_info(&wifi_info);
    if (wifi_info.ssid==NULL) {
        blog_error("flash_get_wifi_info error");
        AT_RESPONSE(AT_ERR);
        return;
    }
    char* str = malloc(128);
    memset(str, 0, 128);
    sprintf(str, "+WIFIJAP:%s,%s\r\n", wifi_info.ssid, wifi_info.password);

    blog_info("%s is ok", str);
    AT_RESPONSE(str);
    AT_RESPONSE("OK\r\n");
    free(str);
}

static void at_wifi_test(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_info("%s is ok", cmd);

    AT_RESPONSE("OK\r\n");
}

static void at_ha_mqtt_config_set(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_info("%s is ok", cmd);
    AT_RESPONSE("OK\r\n");
}

static void at_ha_mqtt_config_check(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_info("%s is ok", cmd);
    AT_RESPONSE("OK\r\n");
}

static void at_ha_mqtt_config_test(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    blog_info("%s is ok", cmd);
    AT_RESPONSE("OK\r\n");
}

static void at_ha_device_msg_set(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
}

at_cmd_info_t at_cmd_list[] = {
    {"AT+WIFIJAP",at_wifi_set,at_wifi_check,at_wifi_test},
    {"AT+HAMQTTCFG",at_ha_mqtt_config_set,at_ha_mqtt_config_check,at_ha_mqtt_config_test},
};