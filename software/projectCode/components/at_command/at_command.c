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

static char cmd_list[16][64] = { 0 };

static dev_msg_t dev_msg = {
      .device_state = DEVICE_STATE_ATCMD_WIFICFG_SET,
};
static int containsNChar(const char* cmd, char c)
{
    int i = 1;
    char* str = cmd;
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

    char* p = NULL;
    char* delim = ",";
    char* p_str = strtok_r(cmd, delim, &p);
    if (p_str!=NULL) {
        memset(cmd_list[0], 0, 64);
        memcpy(cmd_list[0], p_str, strlen(p_str));
    }
    for (size_t i = 1; i < command_cnt; i++)
    {
        p_str = strtok_r(NULL, delim, &p);
        if (p_str!=NULL) {
            memset(cmd_list[i], 0, 64);
            memcpy(cmd_list[i], p_str, strlen(p_str));
        }
    }
    blog_debug("%d command=%s", 2, cmd_list[1]);
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
    char* str = pvPortMalloc(128);
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
/**
 * @brief AT指令配置MQTT服务器地址
 *          AT+HAMQTTCFG="host",port,[clientID],[username],[password]
 * @param cmd
 * @param cmd_len
*/
static void at_ha_mqtt_config_set(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    //识别参数数量
    int command_cnt = containsNChar(cmd, ',');
    //判断参数是否低于2个
    if (command_cnt<2) {
        AT_RESPONSE(AT_ERR);
        return;
    }
    //解析指令
    char* p = NULL;
    char* delim = ",";
    char* p_str = strtok_r(cmd, delim, &p);

    if (p_str!=NULL) {
        memset(cmd_list[0], 0, 64);
        memcpy(cmd_list[0], p_str, strlen(p_str));
    }
    for (size_t i = 1; i < command_cnt; i++)
    {
        p_str = strtok_r(NULL, delim, &p);
        if (p_str!=NULL) {
            memset(cmd_list[i], 0, 64);
            memcpy(cmd_list[i], p_str, strlen(p_str));
        }
    }

    if (dev_msg.ha_dev.mqtt_info.mqtt_host==NULL)dev_msg.ha_dev.mqtt_info.mqtt_host = malloc(64);

    memset(dev_msg.ha_dev.mqtt_info.mqtt_host, 0, sizeof(dev_msg.ha_dev.mqtt_info.mqtt_host));
    strcpy(dev_msg.ha_dev.mqtt_info.mqtt_host, cmd_list[0]);
    dev_msg.ha_dev.mqtt_info.port = atoi(cmd_list[1]);
    if (command_cnt==2) {
        if (dev_msg.ha_dev.mqtt_info.mqtt_clientID!=NULL) {
            vPortFree(dev_msg.ha_dev.mqtt_info.mqtt_clientID);
            dev_msg.ha_dev.mqtt_info.mqtt_clientID = NULL;
        }
        if (dev_msg.ha_dev.mqtt_info.mqtt_username!=NULL) {
            vPortFree(dev_msg.ha_dev.mqtt_info.mqtt_username);
            dev_msg.ha_dev.mqtt_info.mqtt_username = NULL;
        }
        if (dev_msg.ha_dev.mqtt_info.mqtt_password!=NULL) {
            vPortFree(dev_msg.ha_dev.mqtt_info.mqtt_password);
            dev_msg.ha_dev.mqtt_info.mqtt_password = NULL;
        }
    }
    //获取ClientID
    if (command_cnt==3 || command_cnt==4 ||command_cnt==5) {

        if (dev_msg.ha_dev.mqtt_info.mqtt_clientID==NULL)dev_msg.ha_dev.mqtt_info.mqtt_clientID = pvPortMalloc(128);
        memset(dev_msg.ha_dev.mqtt_info.mqtt_clientID, 0, sizeof dev_msg.ha_dev.mqtt_info.mqtt_clientID);
        strcpy(dev_msg.ha_dev.mqtt_info.mqtt_clientID, cmd_list[2]);
    }
    //userName
    if (command_cnt==4 ||command_cnt==5) {

        if (dev_msg.ha_dev.mqtt_info.mqtt_username==NULL)dev_msg.ha_dev.mqtt_info.mqtt_username = pvPortMalloc(64);
        memset(dev_msg.ha_dev.mqtt_info.mqtt_username, 0, strlen(cmd_list[3]));
        strcpy(dev_msg.ha_dev.mqtt_info.mqtt_username, cmd_list[3]);
    }

    //password
    if (command_cnt==5) {

        if (dev_msg.ha_dev.mqtt_info.mqtt_password==NULL)dev_msg.ha_dev.mqtt_info.mqtt_password = pvPortMalloc(64);
        memset(dev_msg.ha_dev.mqtt_info.mqtt_password, 0, strlen(cmd_list[4]));
        strcpy(dev_msg.ha_dev.mqtt_info.mqtt_password, cmd_list[4]);
    }

    bool ret = flash_save_mqtt_info(&dev_msg.ha_dev.mqtt_info);
    if (!ret) {
        AT_RESPONSE(AT_ERR);
        return;
    }
    AT_RESPONSE(AT_OK);
}

static void at_ha_mqtt_config_check(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    flash_get_mqtt_info(&dev_msg.ha_dev.mqtt_info);
    if (dev_msg.ha_dev.mqtt_info.mqtt_host!=NULL) {

        char* str = pvPortMalloc(256);
        memset(str, 0, 256);
        sprintf(str, "+HAMQTTCFG:%s,%d,<%s>,<%s>,<%s>\r\n", dev_msg.ha_dev.mqtt_info.mqtt_host, dev_msg.ha_dev.mqtt_info.port,
        dev_msg.ha_dev.mqtt_info.mqtt_clientID==NULL?"null":dev_msg.ha_dev.mqtt_info.mqtt_clientID,
        dev_msg.ha_dev.mqtt_info.mqtt_username==NULL?"null":dev_msg.ha_dev.mqtt_info.mqtt_username,
        dev_msg.ha_dev.mqtt_info.mqtt_password==NULL?"null":dev_msg.ha_dev.mqtt_info.mqtt_password
        );
        AT_RESPONSE(str);
        vPortFree(str);
    }
    else {
        AT_RESPONSE(AT_ERR);
        return;
    }
    AT_RESPONSE(AT_OK);
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
/**
 * @brief HA 设备信息配置
 *      AT+HADEVICEMSGCFG=<name>,<manufacturer>
 * @param cmd
 * @param cmd_len
*/
static void at_ha_device_msg_set(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    int command_cnt = containsNChar(cmd, ',');
    //判断参数是否低于2个
    if (command_cnt<2) {
        AT_RESPONSE(AT_ERR);
        return;
    }
    //解析指令
    char* p = NULL;
    char* delim = ",";
    char* p_str = strtok_r(cmd, delim, &p);

    if (p_str!=NULL) {
        memset(cmd_list[0], 0, 64);
        memcpy(cmd_list[0], p_str, strlen(p_str));
    }
    for (size_t i = 1; i < command_cnt; i++)
    {
        p_str = strtok_r(NULL, delim, &p);
        if (p_str!=NULL) {
            memset(cmd_list[i], 0, 64);
            memcpy(cmd_list[i], p_str, strlen(p_str));
        }
    }
    //解析设备名
    if (dev_msg.ha_dev.name==NULL) dev_msg.ha_dev.name = pvPortMalloc(32);
    memset(dev_msg.ha_dev.name, 0, 32);
    strcpy(dev_msg.ha_dev.name, cmd_list[0]);

    //解析产商名
    if (dev_msg.ha_dev.manufacturer==NULL)dev_msg.ha_dev.manufacturer = pvPortMalloc(64);
    memset(dev_msg.ha_dev.manufacturer, 0, 64);
    strcpy(dev_msg.ha_dev.manufacturer, cmd_list[1]);
    bool ret = flash_save_ha_device_msg(&dev_msg.ha_dev);
    blog_debug("AT ha device name =%s ,manufacturer=%s ", dev_msg.ha_dev.name, dev_msg.ha_dev.manufacturer);
    if (!ret) {
        AT_RESPONSE(AT_ERR);
        return;
    }
    AT_RESPONSE(AT_OK);
}

static void at_ha_device_msg_check(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    int ret = flash_get_ha_device_msg(&dev_msg.ha_dev);
    if (ret!=-1)
    {
        char* str = pvPortMalloc(256);
        memset(str, 0, 256);
        sprintf(str, "+HADEVICEMSGCFG:%s,%s\r\n", dev_msg.ha_dev.name, dev_msg.ha_dev.manufacturer);
        AT_RESPONSE(str);
        vPortFree(str);
    }
    else {
        AT_RESPONSE(AT_ERR);
        return;
    }

    AT_RESPONSE(AT_OK);
}

static void at_ha_device_msg_test(char* cmd, uint16_t cmd_len)
{
    if (cmd==NULL) {
        blog_error("parameter err \r\n");
        return;
    }
    AT_RESPONSE(AT_OK);
}

at_cmd_info_t at_cmd_list[] = {
    {"AT+WIFIJAP",at_wifi_set,at_wifi_check,at_wifi_test},
    {"AT+HAMQTTCFG",at_ha_mqtt_config_set,at_ha_mqtt_config_check,at_ha_mqtt_config_test},
    {"AT+HADEVICEMSGCFG",at_ha_device_msg_set,at_ha_device_msg_check,at_ha_device_msg_test},
};