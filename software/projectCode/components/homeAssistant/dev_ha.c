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

static dev_msg_t dev_msg;

void ha_event_cb(ha_event_t event, homeAssisatnt_device_t* dev)
{
    switch (event)
    {
        /*  连接服务器事件  */
        case HA_EVENT_MQTT_CONNECED:
        {
            // ir_dev_type_t ac_type = flash_get_ac_type();
            // HA_LOG_I("<<<<<<<<<<  HA_EVENT_MQTT_CONNECED AC TYPE=%d\r\n", ac_type);
            static ha_climateHVAC_t AC1 = {
                .unique_id = "AC_1",
                .name = "空调遥控器",
                .min_temp = 16.0,
                .max_temp = 31.0,
            };
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_CLIMATE_HVAC, &AC1);

            static ha_sensor_entity_t th30_t = {
                .name = "温度",
                .unique_id = "th30_t",
                .device_class = Class_temperature,
                .unit_of_measurement = "℃",
            };

            static ha_sensor_entity_t th30_h = {
                .name = "湿度",
                .unique_id = "th30_h",
                .device_class = Class_humidity,
                .unit_of_measurement = "%",
            };

            static ha_btn_entity_t leran_btn = {
                .name = "匹配空调",
                .unique_id = "btn_learn",
            };
            // homeAssistant_device_add_entity(CONFIG_HA_ENTITY_SELECT, &select);
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_SENSOR, &th30_t);
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_SENSOR, &th30_h);
            homeAssistant_device_add_entity(CONFIG_HA_ENTITY_BUTTON, &leran_btn);

            homeAssistant_device_send_status(HOMEASSISTANT_STATUS_ONLINE);
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_CONNECT;
            homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_CLIMATE_HVAC, &AC1, 0);
            device_state_update(false, &dev_msg);

        }
        break;
        /*  服务器断开事件  */
        case HA_EVENT_MQTT_DISCONNECT:
            HA_LOG_I("<<<<<<<<<<  HA_EVENT_MQTT_DISCONNECT\r\n");
            break;
        case HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_POWER:
            HA_LOG_I("<<<<<<<<<< HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_POWER=%s\r\n", dev->entity_climateHVAC->command_climateHVAC->power_state?"ON":"OFF");
            homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_CLIMATE_HVAC, dev->entity_climateHVAC->command_climateHVAC, dev->entity_climateHVAC->command_climateHVAC->power_state);
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_AC_POWER;

            device_state_update(false, &dev_msg);
            break;
        case HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_MODES:
            HA_LOG_I("<<<<<<<<<< HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_MODES\r\n");
            homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_CLIMATE_HVAC, dev->entity_climateHVAC->command_climateHVAC, 1);
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_AC_MODE;

            device_state_update(false, &dev_msg);
            break;
        case HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_TEMP:
            HA_LOG_I("<<<<<<<<<< HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_TEMP temp=%0.2f\r\n", dev->entity_climateHVAC->command_climateHVAC->temperature_value);
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_AC_TEMP;

            device_state_update(false, &dev_msg);
            break;
        case HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_FAN_MODES:
            HA_LOG_I("<<<<<<<<<< HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_FAN_MODES temp=%d\r\n", dev->entity_climateHVAC->command_climateHVAC->fan_modes_type);
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_AC_FAN_MODE;

            device_state_update(false, &dev_msg);
            break;
        case HA_EVENT_MQTT_COMMAND_BUTTON:
            HA_LOG_I("<<<<<<<<<< HA_EVENT_MQTT_COMMAND_BUTTON \r\n");
            //启动学习之后，开始
            dev_msg.device_state = DEVICE_STATE_HOMEASSISTANT_AC_LERAN_START;
            device_state_update(false, &dev_msg);
            break;
        default:
            break;
    }
    event = HA_EVENT_NONE;
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
    if (dev_ha->name==NULL) dev_ha->name = "智能红外遥控器";
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
