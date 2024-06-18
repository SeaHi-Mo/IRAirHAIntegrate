/**
 * @file homeAssistantPort.c
 * @author SeaHi (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-06
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "homeAssistantPort.h"


homeAssisatnt_device_t* ha_device;
extern  void update_all_entity_to_homeassistant(void);
extern  ha_event_t homeAssistant_get_command(const char* topic, unsigned short topic_len, const char* data, unsigned short data_len);


#ifdef CONFIG_Ai_M6x
#include "aiio_mqtt_client.h"
#include "aiio_wifi.h"

aiio_mqtt_client_handle_t client;

static void HA_LOG_Error_if_nonzero(const char* message, int error_code)
{
    if (error_code != 0) {
        HA_LOG_E("Last error %s: 0x%x\r\n", message, error_code);
    }
}
//MQTT 事件回调
static aiio_err_t mqtt_event_cb(aiio_mqtt_event_handle_t event)
{
    int32_t event_id;
    aiio_mqtt_client_handle_t client = event->client;
    event_id = event->event_id;
    HA_LOG_D("Event dispatched, event_id=%d\r\n", event_id);
    int msg_id;
    switch ((aiio_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ha_device->mqtt_info.mqtt_connect_status = true;
            ha_device->homeassistant_online = true;
            ha_device->event_cb(HA_EVENT_MQTT_CONNECED, ha_device);
            aiio_mqtt_client_subscribe(client, CONFIG_HA_STATUS_TOPIC, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ha_device->mqtt_info.mqtt_connect_status = false;
            ha_device->event_cb(HA_EVENT_MQTT_DISCONNECT, ha_device);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            HA_LOG_D("MQTT_EVENT_SUBSCRIBED\r\n");

            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            HA_LOG_D("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\r\n", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            HA_LOG_D("MQTT_EVENT_PUBLISHED, msg_id=%d\r\n", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            HA_LOG_D("MQTT_EVENT_DATA\r\n");
            HA_LOG_D("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            HA_LOG_D("DATA=%.*s\r\n", event->data_len, event->data);
            ha_event_t ha_event = homeAssistant_get_command(event->topic, event->topic_len, event->data, event->data_len);
            if (ha_event==HA_EVENT_HOMEASSISTANT_STATUS_ONLINE) {
                HA_LOG_I("HomeAssistant is online,send device status\r\n");
                update_all_entity_to_homeassistant();
                homeAssistant_device_send_status(HOMEASSISTANT_STATUS_ONLINE);
            }

            if (ha_device->homeassistant_online)
                ha_device->event_cb(ha_event, ha_device);
            else
                HA_LOG_E("HomeAssistant is offline\r\n");
            break;
        case MQTT_EVENT_ERROR:
            // HA_LOG_D("MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                HA_LOG_Error_if_nonzero("reported from esp-tls", event->error_handle->aiio_tls_last_aiio_err);
                HA_LOG_Error_if_nonzero("reported from tls stack", event->error_handle->aiio_tls_stack_err);
                HA_LOG_Error_if_nonzero("captured as transport's socket errno", event->error_handle->aiio_transport_sock_errno);
                HA_LOG_D("Last errno string (%s)\r\n", strerror(event->error_handle->aiio_transport_sock_errno));
            }
            ha_device->event_cb(HA_EVENT_MQTT_ERROR, ha_device);
            break;
        default:
            HA_LOG_D("Other event id:%d\r\n", event->event_id);
            break;
    }
    event->event_id = MQTT_EVENT_ANY;
    return AIIO_OK;
}
#endif

#ifdef CONFIG_Ai_WB2
#include <mqtt_client.h>
#include <wifi_mgmr_ext.h>
#include "blog.h"
axk_mqtt_client_handle_t client;

//错误打印的log
static void log_error_if_nonzero(const char* message, int error_code)
{
    if (error_code != 0) {
        HA_LOG_E("Last error %s: 0x%x\r\n", message, error_code);
    }
}
//Ai-WB2 MQTT事件回调函数
static axk_err_t event_cb(axk_mqtt_event_handle_t event)
{
    int32_t event_id;
    axk_mqtt_client_handle_t client = event->client;

    event_id = event->event_id;
    HA_LOG_D("Event dispatched, event_id=%d\r\n", (int)event_id);
    int msg_id;
    switch ((axk_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            HA_LOG_I("MQTT_EVENT_CONNECTED\r\n");
            ha_device->mqtt_info.mqtt_connect_status = true;
            ha_device->homeassistant_online = true;
            if (ha_device->event_cb==NULL) HA_LOG_E("event cb is NULL\r\n");
            ha_device->event_cb(HA_EVENT_MQTT_CONNECED, ha_device);
            axk_mqtt_client_subscribe(client, CONFIG_HA_STATUS_TOPIC, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            HA_LOG_I("MQTT_EVENT_DISCONNECTED\r\n");
            ha_device->mqtt_info.mqtt_connect_status = false;
            ha_device->event_cb(HA_EVENT_MQTT_DISCONNECT, ha_device);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            HA_LOG_I("MQTT_EVENT_SUBSCRIBED, msg_id=%d\r\n", event->msg_id);

            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            HA_LOG_I("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\r\n", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            HA_LOG_I("MQTT_EVENT_PUBLISHED, msg_id=%d\r\n", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            HA_LOG_I("MQTT_EVENT_DATA\r\n");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            ha_event_t ha_event = homeAssistant_get_command(event->topic, event->topic_len, event->data, event->data_len);
            if (ha_event==HA_EVENT_HOMEASSISTANT_STATUS_ONLINE) {
                HA_LOG_I("HomeAssistant is online,send device status\r\n");
                update_all_entity_to_homeassistant();
                homeAssistant_device_send_status(HOMEASSISTANT_STATUS_ONLINE);
            }

            if (ha_device->homeassistant_online)
                ha_device->event_cb(ha_event, ha_device);
            else
                HA_LOG_E("HomeAssistant is offline\r\n");
            break;
        case MQTT_EVENT_ERROR:
            HA_LOG_I("MQTT_EVENT_ERROR\r\n");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from axk-tls", event->error_handle->axk_tls_last_axk_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->axk_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->axk_transport_sock_errno);
                HA_LOG_I("Last errno string (%s)\r\n", strerror(event->error_handle->axk_transport_sock_errno));
            }
            ha_device->event_cb(HA_EVENT_MQTT_ERROR, ha_device);
            break;
        default:
            HA_LOG_I("Other event id:%d\r\n", event->event_id);
            break;
    }
    return AXK_OK;
}
#endif
/**
 * @brief 获取STA 的MAC地址
 *
 * @param mac
 * @return * void
*/
void homeAssistant_get_sta_mac(char* mac)
{
#ifdef CONFIG_Ai_M6x  
    aiio_wifi_sta_mac_get(mac);
#endif

#ifdef CONFIG_Ai_WB2
    wifi_mgmr_sta_mac_get((uint8_t*)mac);
#endif
}

/**
 * @brief homeAssistant_port_init
 *         homeAssistant 所用MQTT 库的初始化接口，所有的MQTT 库的初始化应该在此进行
 * @param ha_device
 * @return int
*/
int homeAssistant_mqtt_port_init(homeAssisatnt_device_t* ha_dev)
{
    if (ha_dev==NULL) {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
    }
    ha_device = ha_dev;
    //安信可Ai-M6x 系列模组例子
#ifdef CONFIG_Ai_M6x    
    aiio_mqtt_client_config_t mqtt_cfg = {
         .host = ha_device->mqtt_info.mqtt_host,
         .port = ha_device->mqtt_info.port,
         .username = ha_device->mqtt_info.mqtt_username,
         .password = ha_device->mqtt_info.mqtt_password,
         .keepalive = ha_device->mqtt_info.mqtt_keeplive,
         .client_id = ha_device->mqtt_info.mqtt_clientID,
         .lwt_qos = ha_device->mqtt_info.will.will_qos,
         .lwt_retain = ha_device->mqtt_info.will.will_retain,
         .lwt_msg = ha_device->mqtt_info.will.will_msg,
         .lwt_msg_len = ha_device->mqtt_info.will.will_msg_len,
         .lwt_topic = ha_device->mqtt_info.will.will_topic,
         .event_handle = mqtt_event_cb,
    };

    memset(&client, 0, sizeof(aiio_mqtt_client_handle_t));
    client = aiio_mqtt_client_init(&mqtt_cfg);
    if (client!= NULL) HA_LOG_D("MQTT client init suceess!\r\n");
    else {
        HA_LOG_E("MQTT client init fail\r\n");
        return -1;
    }
#endif
    //安信可Ai-WB2 系列模组例子
#ifdef CONFIG_Ai_WB2
    axk_mqtt_client_config_t mqtt_cfg = {
        .host = ha_device->mqtt_info.mqtt_host,
        .port = ha_device->mqtt_info.port,
        .username = ha_device->mqtt_info.mqtt_username,
        .password = ha_device->mqtt_info.mqtt_password,
        .keepalive = ha_device->mqtt_info.mqtt_keeplive,
        .client_id = ha_device->mqtt_info.mqtt_clientID,
        .lwt_qos = ha_device->mqtt_info.will.will_qos,
        .lwt_retain = ha_device->mqtt_info.will.will_retain,
        .lwt_msg = ha_device->mqtt_info.will.will_msg,
        .lwt_msg_len = ha_device->mqtt_info.will.will_msg_len,
        .lwt_topic = ha_device->mqtt_info.will.will_topic,
        .event_handle = event_cb,
    };
    memset(&client, 0, sizeof(axk_mqtt_client_handle_t));
    client = axk_mqtt_client_init(&mqtt_cfg);
    if (client!= NULL) HA_LOG_D("MQTT client init suceess!\r\n");
    else {
        HA_LOG_E("MQTT client init fail\r\n");
        return -1;
    }
#endif
    //其他程序
    return 0;
}
/**
 * @brief homeAssistant MQTT 启动程序
 *
 * @return int 成功返回0 失败返回 -1
*/
int homeAssistant_mqtt_port_start(void)
{
    //安信可Ai-M6x 系列模组例子
#ifdef CONFIG_Ai_M6x
    if (client!=NULL)
        return aiio_mqtt_client_start(client);
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif
    //安信可Ai-WB2 系列模组例子
#ifdef CONFIG_Ai_WB2
    if (client!=NULL)
        return axk_mqtt_client_start(client);
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif

    return 0;
}
/**
 * @brief mqtt 停止函数，
 *
 * @return int
*/
int homeAssistant_mqtt_port_stop(void)
{
    //安信可Ai-M6x 系列模组例子
    int ret = 0;
#ifdef CONFIG_Ai_M6x
    if (client!=NULL) {
        ret = aiio_mqtt_client_disconnect(client);
        if (ret<0) HA_LOG_E("[%s:%d]mqtt disconnect error", __func__, __LINE__);
        ret = aiio_mqtt_client_stop(client);
        if (ret<0) HA_LOG_E("[%s:%d]mqtt stop error", __func__, __LINE__);
    }
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif
#ifdef CONFIG_Ai_WB2
    //安信可Ai-WB2 系列模组例子
    if (client!=NULL) {
        ret = axk_mqtt_client_disconnect(client);
        if (ret<0) HA_LOG_E("[%s:%d]mqtt disconnect error", __func__, __LINE__);
        ret = axk_mqtt_client_stop(client);
        if (ret<0) HA_LOG_E("[%s:%d]mqtt stop error", __func__, __LINE__);
    }
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif
    return ret;
}

/**
 * @brief mqtt 发布数据接口
 *
 * @param topic 数据Topic
 * @param payload 数据本体
 * @param qos 服务质量
 * @param retain 是否要求服务器保留信息
 * @return int 成功返回0 失败返回 -1
*/
int homeAssistant_mqtt_port_public(const char* topic, const char* payload, int qos, bool retain)
{
    //安信可Ai-M6x 系列模组例子
#ifdef CONFIG_Ai_M6x
    if (client!=NULL && topic!=NULL &&  payload!=NULL) {

        return aiio_mqtt_client_publish(client, topic, payload, strlen(payload), qos, retain);
    }
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }

#endif
    //安信可Ai-WB2 系列模组例子
#ifdef CONFIG_Ai_WB2
    if (client!=NULL && topic!=NULL &&  payload!=NULL)
        return axk_mqtt_client_publish(client, topic, payload, strlen(payload), qos, retain);
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif

    return 0;
}

/**
 * @brief mqtt 订阅接口
 *
 * @param topic 需要订阅的topic
 * @param qos 服务质量
 * @return int 成功返回0 失败返回-1
*/
int homeAssistant_mqtt_port_subscribe(const char* topic, int qos)
{
#ifdef CONFIG_Ai_M6x
    if (client!=NULL && topic!=NULL)
        return aiio_mqtt_client_subscribe(client, topic, qos);
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif

#ifdef CONFIG_Ai_WB2
    if (client!=NULL && topic!=NULL)
        return axk_mqtt_client_subscribe(client, topic, qos);
    else {
        HA_LOG_E("[%s:%d]param is null\r\n", __func__, __LINE__);
        return -1;
    }
#endif
    return 0;
}

