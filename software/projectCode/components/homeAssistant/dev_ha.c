/**
 * @file dev_ha.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blog.h"
#include "dev_ha.h"

void ha_event_cb(ha_event_t event, homeAssisatnt_device_t* dev)
{
    switch (event)
    {
        /*  连接服务器事件  */
        case HA_EVENT_MQTT_CONNECED:
            HA_LOG_I("<<<<<<<<<<  HA_EVENT_MQTT_CONNECED\r\n");
            static ha_sw_entity_t sw1 = {
            .name = "开关",
            .unique_id = "switch1"
            };
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_SWITCH, &sw1);

            static ha_climateHVAC_t AC1 = {
                .name = "美的空调",
                .unique_id = "AC_1",
            };
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_CLIMATE_HVAC, &AC1);
            homeAssistant_device_send_status(HOMEASSISTANT_STATUS_ONLINE);
            break;
            /*  服务器断开事件  */
        case HA_EVENT_MQTT_DISCONNECT:
            HA_LOG_I("<<<<<<<<<<  HA_EVENT_MQTT_DISCONNECT\r\n");
        default:
            break;
    }
}

void device_homeAssistant_init(homeAssisatnt_device_t* dev_ha)
{
    if (dev_ha==NULL)
    {
        blog_error("dev_ha is NULL");
        return;
    }
    uint8_t MAC[6] = { 0 };

    wifi_mgmr_sta_mac_get(MAC);
    if (dev_ha->name==NULL) dev_ha->name = "IRAirHAIntegrate";
    dev_ha->hw_version = DEVICE_HW_SERSION;
    if (dev_ha->manufacturer==NULL) dev_ha->manufacturer = "SeaHi";
    if (dev_ha->model==NULL) dev_ha->model = "Ai-WB2";
    if (dev_ha->identifiers==NULL) {
        dev_ha->identifiers = pvPortMalloc(64);
        memset(dev_ha->identifiers, 0, 64);
        sprintf(dev_ha->identifiers, "%s-%02X%02X", dev_ha->name, MAC[4], MAC[5]);
    }

    if (dev_ha->mqtt_info.mqtt_host==NULL) dev_ha->mqtt_info.mqtt_host = MQTT_SERVER_DEFAULT_HOST;
    if (dev_ha->mqtt_info.port==0)dev_ha->mqtt_info.port = MQTT_SERVER_DEFAULT_PORT;

    if (dev_ha->mqtt_info.mqtt_clientID==NULL) {
        dev_ha->mqtt_info.mqtt_clientID = pvPortMalloc(64);
        memset(dev_ha->mqtt_info.mqtt_clientID, 0, 64);
        sprintf(dev_ha->mqtt_info.mqtt_clientID, "%s-%02X%02X%02X%02X%02X%02X", dev_ha->name, MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
    }
    if (dev_ha->mqtt_info.mqtt_username==NULL) dev_ha->mqtt_info.mqtt_username = dev_ha->name;
    if (dev_ha->mqtt_info.mqtt_password==NULL)dev_ha->mqtt_info.mqtt_password = "12345678";

    blog_info("------------------mqtt msg----------------------");
    blog_info("mqtt host:%s", dev_ha->mqtt_info.mqtt_host);
    blog_info("mqtt port:%d", dev_ha->mqtt_info.port);
    blog_info("mqtt client ID:%s", dev_ha->mqtt_info.mqtt_clientID==NULL?"null":dev_ha->mqtt_info.mqtt_clientID);
    blog_info("--------HomeAssistant device massege------------");
    blog_info("device name:%s", dev_ha->name);
    blog_info("device manufacturer:%s", dev_ha->manufacturer);
    blog_info("------------------------------------------------");

    homeAssistant_device_init(dev_ha, ha_event_cb);
}
