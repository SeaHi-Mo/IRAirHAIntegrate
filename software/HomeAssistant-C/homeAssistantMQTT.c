
/**
 * @file homeAssistantMQTT.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-02-03
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include <task.h>
#include <assert.h>
#include <stdarg.h>
#include "time.h"
// #include "aiio_wifi.h"
#include "homeAssistantPort.h"
#include "cJSON.h"
#define DBG_TAG "homeAssistantMQTT"

static homeAssisatnt_device_t* ha_device;

static uint8_t STA_MAC[6] = { 0 };

static cJSON* homeAssistant_device_create(void)
{
    if (ha_device==NULL)
    {
        HA_LOG_E("device is NULL\r\n");
        return NULL;
    }
    cJSON* root = cJSON_CreateObject();
    if (ha_device->name!=NULL) cJSON_AddStringToObject(root, "name", ha_device->name);
    if (ha_device->hw_version!=NULL) cJSON_AddStringToObject(root, "hw_version", ha_device->hw_version);
    if (ha_device->identifiers!=NULL) {
        cJSON* identifiers = cJSON_CreateArray();
        cJSON_AddItemToObject(root, "identifiers", identifiers);
        cJSON_AddItemToArray(identifiers, cJSON_CreateString(ha_device->identifiers));
    }
    if (ha_device->manufacturer!=NULL)cJSON_AddStringToObject(root, "manufacturer", ha_device->manufacturer);
    if (ha_device->model!=NULL)cJSON_AddStringToObject(root, "model", ha_device->model);
    if (ha_device->suggest_area!=NULL)cJSON_AddStringToObject(root, "suggest_area", ha_device->suggest_area);
    if (ha_device->sw_version!=NULL)cJSON_AddStringToObject(root, "sw_version", ha_device->sw_version);
    if (ha_device->via_device!=NULL)cJSON_AddStringToObject(root, "via_device", ha_device->via_device);

    cJSON* connencts = cJSON_CreateArray();
    cJSON* MAC = cJSON_CreateArray();
    cJSON_AddItemToArray(connencts, MAC);
    char* mac_str = pvPortMalloc(32);
    memset(mac_str, 0, 32);
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5]);
    cJSON_AddItemToArray(MAC, cJSON_CreateString("mac"));
    cJSON_AddItemToArray(MAC, cJSON_CreateString(mac_str));
    cJSON_AddItemToObject(root, "connections", connencts);
    vPortFree(mac_str);
    return root;
}
/**
 * @brief homeAssistant_create_switch_data 创建开关实体配置数据
 *
 * @param switch_entity
 * @param device_json
*/
#if CONFIG_ENTITY_ENABLE_SWITCH
static void homeAssistant_create_switch_data(ha_sw_entity_t* switch_entity, cJSON* device_json)
{
    if (switch_entity==NULL) {
        HA_LOG_E("entity switch buff is NULL\r\n");
        return;
    }
    cJSON* root = cJSON_CreateObject();
    if (switch_entity->name!=NULL)cJSON_AddStringToObject(root, "name", switch_entity->name);
    if (switch_entity->device_class!=NULL)cJSON_AddStringToObject(root, "device_class", switch_entity->device_class);
    if (switch_entity->unique_id!=NULL) {
        char* unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        switch_entity->unique_id = strcat(switch_entity->unique_id, unique_id);
        cJSON_AddStringToObject(root, "unique_id", switch_entity->unique_id);
        vPortFree(unique_id);
    }
    else HA_LOG_E("unique id is null for entity:%s \r\n ", switch_entity->name);


    if (switch_entity->object_id!=NULL)cJSON_AddStringToObject(root, "object_id", switch_entity->object_id);

    if (switch_entity->command_topic==NULL) {
        switch_entity->command_topic = pvPortMalloc(256);
        memset(switch_entity->command_topic, 0, 256);
        sprintf(switch_entity->command_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/set", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], switch_entity->unique_id);
    }
    cJSON_AddStringToObject(root, "command_topic", switch_entity->command_topic);
    if (switch_entity->state_topic==NULL) {
        switch_entity->state_topic = pvPortMalloc(256);
        memset(switch_entity->state_topic, 0, 256);
        sprintf(switch_entity->state_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/state", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], switch_entity->unique_id);
    }
    cJSON_AddStringToObject(root, "state_topic", switch_entity->state_topic);
    if (switch_entity->payload_off==NULL)switch_entity->payload_off = "OFF";
    if (switch_entity->payload_on==NULL) switch_entity->payload_on = "ON";

    cJSON_AddStringToObject(root, "payload_off", switch_entity->payload_off);
    cJSON_AddStringToObject(root, "payload_on", switch_entity->payload_on);

    if (switch_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", switch_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (switch_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", switch_entity->payload_available);
    else cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (switch_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", switch_entity->payload_not_available);
    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);
    if (switch_entity->qos) cJSON_AddNumberToObject(root, "qos", switch_entity->qos);
    if (switch_entity->retain) cJSON_AddTrueToObject(root, "retain");
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);
    if (switch_entity->icon!=NULL) cJSON_AddStringToObject(root, "icon", switch_entity->icon);

    switch_entity->config_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
}

/**
 * @brief entity_swith_add_node
 *         添加一个开关实体
 * @param switch_new_node
*/
static void entity_swith_add_node(ha_sw_entity_t* switch_new_node)
{
    assert(ha_device->entity_switch->switch_list);

    //添加新的节点
    ha_sw_entity_t* switch_list_handle = ha_device->entity_switch->switch_list->prev;
    //开始创建新的实体数据
    homeAssistant_create_switch_data(switch_new_node, homeAssistant_device_create());
    if (switch_new_node->entity_config_topic==NULL) {
        switch_new_node->entity_config_topic = pvPortMalloc(128);
        memset(switch_new_node->entity_config_topic, 0, 128);
        sprintf(switch_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_SWITCH, switch_new_node->unique_id);
    }

    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(switch_new_node->entity_config_topic, switch_new_node->config_data, 1, 1);
        if (switch_new_node->command_topic!=NULL)homeAssistant_mqtt_port_subscribe(switch_new_node->command_topic, 1);
        if (switch_new_node->availability_topic!=NULL)
            homeAssistant_mqtt_port_public(switch_new_node->availability_topic, switch_new_node->payload_available, 0, 0);
    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }
    //插入节点
    switch_list_handle->next = switch_new_node;
    switch_new_node->prev = switch_list_handle;
    switch_new_node->next = ha_device->entity_switch->switch_list;
    ha_device->entity_switch->switch_list->prev = switch_new_node;

    vPortFree(switch_new_node->config_data);
}
#endif
/**
 * @brief 创建Light 的config 数据
 *
 * @param light_entity
 * @param device_json
*/
#if CONFIG_ENTITY_ENABLE_LIGHT
static void homeAssistant_create_light_data(ha_lh_entity_t* light_entity, cJSON* device_json)
{
    if (light_entity==NULL) {
        HA_LOG_E("entity light buff is NULL\r\n");
        return;
    }
    cJSON* root = cJSON_CreateObject();
    if (light_entity->name!=NULL)cJSON_AddStringToObject(root, "name", light_entity->name);
    if (light_entity->device_class!=NULL)cJSON_AddStringToObject(root, "device_class", light_entity->device_class);
    if (light_entity->unique_id!=NULL) {
        char* unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        light_entity->unique_id = strcat(light_entity->unique_id, unique_id);
        vPortFree(unique_id);
        cJSON_AddStringToObject(root, "unique_id", light_entity->unique_id);
    }
    else HA_LOG_E("unique id is null for entity:%s \r\n ", light_entity->name);
    if (light_entity->object_id!=NULL)cJSON_AddStringToObject(root, "object_id", light_entity->object_id);
    if (light_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", light_entity->icon);
    if (light_entity->availability_template!=NULL)cJSON_AddStringToObject(root, "availability_template", light_entity->availability_template);
    if (light_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", light_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (light_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", light_entity->payload_available);
    else  cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (light_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", light_entity->payload_not_available);
    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);

    if (light_entity->command_topic==NULL)
    {
        light_entity->command_topic = pvPortMalloc(128);
        memset(light_entity->command_topic, 0, 128);
        sprintf(light_entity->command_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/set", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], light_entity->unique_id);
    }
    if (light_entity->state_topic==NULL)
    {
        light_entity->state_topic = pvPortMalloc(128);
        memset(light_entity->state_topic, 0, 128);
        sprintf(light_entity->state_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/state", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], light_entity->unique_id);
    }

    cJSON_AddStringToObject(root, "command_topic", light_entity->command_topic);
    cJSON_AddStringToObject(root, "state_topic", light_entity->state_topic);
    if (light_entity->payload_off!=NULL)cJSON_AddStringToObject(root, "payload_off", light_entity->payload_off);
    if (light_entity->payload_on!=NULL)cJSON_AddStringToObject(root, "payload_on", light_entity->payload_on);
    //brightness
    if (light_entity->brightness.brightness_command_template!=NULL)cJSON_AddStringToObject(root, "brightness_command_template", light_entity->brightness.brightness_command_template);
    if (light_entity->brightness.brightness_command_topic!=NULL) cJSON_AddStringToObject(root, "brightness_command_topic", light_entity->brightness.brightness_command_topic);
    if (light_entity->brightness.brightness_state_topic!=NULL)cJSON_AddStringToObject(root, "brightness_state_topic", light_entity->brightness.brightness_state_topic);
    if (light_entity->brightness.brightness_scale!=NULL)cJSON_AddStringToObject(root, "brightness_scale", light_entity->brightness.brightness_scale);
    if (light_entity->brightness.brightness_value_template!=NULL)cJSON_AddStringToObject(root, "brightness_value_template", light_entity->brightness.brightness_value_template);
    // rbgw
    if (light_entity->rgbw.rgbw_command_template!=NULL)cJSON_AddStringToObject(root, "rgbw_command_template", light_entity->rgbw.rgbw_command_template);
    if (light_entity->rgbw.rgbw_command_topic!=NULL) cJSON_AddStringToObject(root, "rgbw_command_topic", light_entity->rgbw.rgbw_command_topic);
    if (light_entity->rgbw.rgbw_state_topic!=NULL)  cJSON_AddStringToObject(root, "rgbw_state_topic", light_entity->rgbw.rgbw_state_topic);
    if (light_entity->rgbw.rgbw_value_template!=NULL)cJSON_AddStringToObject(root, "rgbw_value_template", light_entity->rgbw.rgbw_value_template);
    //rbg
    if (light_entity->rgb.rgb_command_template!=NULL)cJSON_AddStringToObject(root, "rgb_command_template", light_entity->rgb.rgb_command_template);
    if (light_entity->rgb.rgb_command_topic!=NULL) cJSON_AddStringToObject(root, "rgb_command_topic", light_entity->rgb.rgb_command_topic);
    if (light_entity->rgb.rgb_state_topic != NULL)cJSON_AddStringToObject(root, "rgb_state_topic", light_entity->rgb.rgb_state_topic);
    if (light_entity->rgb.rgb_value_template!=NULL)cJSON_AddStringToObject(root, "rgb_value_template", light_entity->rgb.rgb_value_template);

    if (light_entity->qos) cJSON_AddNumberToObject(root, "qos", light_entity->qos);
    if (light_entity->retain) cJSON_AddTrueToObject(root, "retain");
    //添加设备信息
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);

    light_entity->config_data = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);
}

static void  entity_light_add_node(ha_lh_entity_t* light_new_node)
{
    // assert(ha_device->entity_light->light_list);
    ha_lh_entity_t* light_list_handle = ha_device->entity_light->light_list->prev;
    //开始创建新的实体数据
    homeAssistant_create_light_data(light_new_node, homeAssistant_device_create());
    if (light_new_node->entity_config_topic==NULL) {
        light_new_node->entity_config_topic = pvPortMalloc(128);
        memset(light_new_node->entity_config_topic, 0, 128);
        sprintf(light_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_LIGHT, light_new_node->unique_id);
    }
    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(light_new_node->entity_config_topic, light_new_node->config_data, 1, 1);
        if (light_new_node->command_topic!=NULL)homeAssistant_mqtt_port_subscribe(light_new_node->command_topic, 1);
        if (light_new_node->rgbw.rgbw_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(light_new_node->rgbw.rgbw_command_topic, 1);
        if (light_new_node->brightness.brightness_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(light_new_node->brightness.brightness_command_topic, 1);
        if (light_new_node->rgb.rgb_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(light_new_node->rgb.rgb_command_topic, 1);
        if (light_new_node->availability_topic!=NULL)
            homeAssistant_mqtt_port_public(light_new_node->availability_topic, light_new_node->payload_available, 0, 0);
    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }
    //插入节点
    light_list_handle->next = light_new_node;
    light_new_node->prev = light_list_handle;
    light_new_node->next = ha_device->entity_light->light_list;
    ha_device->entity_light->light_list->prev = light_new_node;
    vPortFree(light_new_node->config_data);
}
/**
 * @brief homeAssistant_get_light_rgb
 *        获取RGB 值，适用于没有使用格式要求的情况，简单解析出RGB三基色值
 * @param light_entity
 * @param rgb_data
*/
static void homeAssistant_get_light_rgb(ha_lh_entity_t* light_entity, const char* rgb_data, unsigned short data_len)
{
    if (light_entity==NULL || rgb_data==NULL) {
        HA_LOG_E("entity light buff is NULL\r\n");
        return;
    }
    char* data_str = pvPortMalloc(data_len);
    memset(data_str, 0, data_len);
    sprintf(data_str, "%.*s", data_len, rgb_data);
    HA_LOG_F("rgb_data %s\r\n", data_str);
    if (light_entity->rgb.rgb_command_template==NULL) {
        char* rgb = strtok(data_str, ",");
        if (rgb)light_entity->rgb.red = atoi(rgb);
        rgb = strtok(NULL, ",");
        if (rgb) light_entity->rgb.green = atoi(rgb);
        rgb = strtok(NULL, ",");
        if (rgb) light_entity->rgb.blue = atoi(rgb);

    }
    vPortFree(data_str);
}
#endif
/**
 * @brief 传感器开关实体
 *
*/
#if CONFIG_ENTITY_ENABLE_SENSOR
static char* sensor_class_type[] = { "None","apparent_power","aqi","atmospheric_pressure","battery",\
"carbon_dioxide","carbon_monoxide","current","data_rate","data_size","date","distance","duration",\
"energy","energy_storage","enum","frequency","gas","humidity","illuminance","irradiance","moisture",\
"monetary","nitrogen_dioxide","nitrogen_monoxide","nitrous_oxide","ozone","ph","pm1","pm25","pm10",\
"power_factor","power","precipitation","precipitation_intensity","pressure","reactive_power","signal_strength",\
"sound_pressure","speed","sulphur_dioxide","temperature","timestamp","volatile_organic_compounds","volatile_organic_compounds_parts",\
"voltage","volume","volume_flow_rate","volume_storage","water","weight","wind_speed" };

static void homeAssistant_create_sensor_data(ha_sensor_entity_t* sensor_entity, cJSON* device_json)
{
    if (sensor_entity==NULL) {
        HA_LOG_E("entity light buff is NULL\r\n");
        return;
    }

    cJSON* root = cJSON_CreateObject();
    if (sensor_entity->name!=NULL)cJSON_AddStringToObject(root, "name", sensor_entity->name);
    if (sensor_entity->unique_id!=NULL)
    {
        char* unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        sensor_entity->unique_id = strcat(sensor_entity->unique_id, unique_id);
        vPortFree(unique_id);
        cJSON_AddStringToObject(root, "unique_id", sensor_entity->unique_id);
    }
    else HA_LOG_E("unique id is null for entity:%s \r\n ", sensor_entity->name);

    if (sensor_entity->object_id!=NULL)cJSON_AddStringToObject(root, "object_id", sensor_entity->object_id);
    if (sensor_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", sensor_entity->icon);
    if (sensor_entity->availability_template!=NULL)cJSON_AddStringToObject(root, "availability_template", sensor_entity->availability_template);
    if (sensor_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", sensor_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (sensor_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", sensor_entity->payload_available);
    else  cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (sensor_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", sensor_entity->payload_not_available);
    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);
    if (sensor_entity->device_class) cJSON_AddStringToObject(root, "device_class", sensor_class_type[sensor_entity->device_class]);
    if (sensor_entity->suggested_display_precision)cJSON_AddNumberToObject(root, "suggested_display_precision", sensor_entity->suggested_display_precision);
    if (sensor_entity->json_attributes_template!=NULL) cJSON_AddStringToObject(root, "json_attributes_template", sensor_entity->json_attributes_template);
    if (sensor_entity->json_attributes_topic!=NULL)cJSON_AddStringToObject(root, "json_attributes_topic", sensor_entity->json_attributes_topic);
    if (sensor_entity->state_class!=NULL)cJSON_AddStringToObject(root, "state_class", sensor_entity->state_class);
    if (sensor_entity->state_topic==NULL) {
        sensor_entity->state_topic = pvPortMalloc(128);
        memset(sensor_entity->state_topic, 0, 128);
        sprintf(sensor_entity->state_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/state", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], sensor_entity->unique_id);
    }cJSON_AddStringToObject(root, "state_topic", sensor_entity->state_topic);

    if (sensor_entity->unit_of_measurement!=NULL) cJSON_AddStringToObject(root, "unit_of_measurement", sensor_entity->unit_of_measurement);
    if (sensor_entity->value_template!=NULL)cJSON_AddStringToObject(root, "value_template", sensor_entity->value_template);
    if (sensor_entity->expire_after)cJSON_AddNumberToObject(root, "expire_after", sensor_entity->expire_after);
    if (sensor_entity->force_update)cJSON_AddTrueToObject(root, "force_update");
    else cJSON_AddFalseToObject(root, "force_update");

    if (sensor_entity->qos) cJSON_AddNumberToObject(root, "qos", sensor_entity->qos);
    if (sensor_entity->retain) cJSON_AddTrueToObject(root, "retain");
    //添加设备信息
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);
    sensor_entity->config_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
}

static void  entity_sensor_add_node(ha_sensor_entity_t* sensor_new_node)
{
    if (sensor_new_node==NULL) return;
    ha_sensor_entity_t* sensor_list_handle = ha_device->entity_sensor->sensor_list->prev;

    homeAssistant_create_sensor_data(sensor_new_node, homeAssistant_device_create());
    if (sensor_new_node->entity_config_topic==NULL) {
        sensor_new_node->entity_config_topic = pvPortMalloc(128);
        memset(sensor_new_node->entity_config_topic, 0, 128);
        sprintf(sensor_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_SENSOR, sensor_new_node->unique_id);

    }
    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(sensor_new_node->entity_config_topic, sensor_new_node->config_data, 1, 1);
        if (sensor_new_node->availability_topic!=NULL)
            homeAssistant_mqtt_port_public(sensor_new_node->availability_topic, sensor_new_node->payload_available, 0, 0);
        HA_LOG_D("Topic:%s\r\n", sensor_new_node->entity_config_topic);
        HA_LOG_D("config data:%s\r\n", sensor_new_node->config_data);
    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }

    sensor_list_handle->next = sensor_new_node;
    sensor_new_node->prev = sensor_list_handle;
    sensor_new_node->next = ha_device->entity_sensor->sensor_list;
    ha_device->entity_sensor->sensor_list->prev = sensor_new_node;
    vPortFree(sensor_new_node->config_data);
}
#endif
/**
 * @brief 二进制传感器
 *
*/
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR
static char* Bsensor_class_type[] = { "None","battery","battery_charging","carbon_monoxide","cold","connectivity","door","garage_door",\
    "gas","heat","light","lock","moisture","motion","moving","occupancy","opening","plug","power", "presence", "problem", "running","safety", "smoke", "sound", "tamper", "update", "vibration", "window" };

static void homeAssistant_create_binary_sensor_data(ha_Bsensor_entity_t* binary_sensor_entity, cJSON* device_json)
{
    if (binary_sensor_entity==NULL) {
        HA_LOG_E("entity light buff is NULL\r\n");
        return;
    }

    cJSON* root = cJSON_CreateObject();
    if (binary_sensor_entity->name!=NULL)cJSON_AddStringToObject(root, "name", binary_sensor_entity->name);
    if (binary_sensor_entity->unique_id!=NULL)
    {
        char* unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        binary_sensor_entity->unique_id = strcat(binary_sensor_entity->unique_id, unique_id);
        vPortFree(unique_id);
        cJSON_AddStringToObject(root, "unique_id", binary_sensor_entity->unique_id);
    }
    else HA_LOG_E("unique id is null for entity:%s \r\n", binary_sensor_entity->name);

    if (binary_sensor_entity->object_id!=NULL)cJSON_AddStringToObject(root, "object_id", binary_sensor_entity->object_id);
    if (binary_sensor_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", binary_sensor_entity->icon);
    if (binary_sensor_entity->availability_template!=NULL)cJSON_AddStringToObject(root, "availability_template", binary_sensor_entity->availability_template);

    if (binary_sensor_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", binary_sensor_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (binary_sensor_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", binary_sensor_entity->payload_available);
    else  cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (binary_sensor_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", binary_sensor_entity->payload_not_available);

    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);
    if (binary_sensor_entity->device_class) cJSON_AddStringToObject(root, "device_class", Bsensor_class_type[binary_sensor_entity->device_class]);
    if (binary_sensor_entity->json_attributes_template!=NULL) cJSON_AddStringToObject(root, "json_attributes_template", binary_sensor_entity->json_attributes_template);
    if (binary_sensor_entity->json_attributes_topic!=NULL)cJSON_AddStringToObject(root, "json_attributes_topic", binary_sensor_entity->json_attributes_topic);
    if (binary_sensor_entity->state_class!=NULL)cJSON_AddStringToObject(root, "state_class", binary_sensor_entity->state_class);

    if (binary_sensor_entity->entity_category!=NULL)cJSON_AddStringToObject(root, "entity_category", binary_sensor_entity->entity_category);
    if (binary_sensor_entity->state_topic==NULL) {
        binary_sensor_entity->state_topic = pvPortMalloc(128);
        memset(binary_sensor_entity->state_topic, 0, 128);
        sprintf(binary_sensor_entity->state_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/state", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], binary_sensor_entity->unique_id);
    }
    cJSON_AddStringToObject(root, "state_topic", binary_sensor_entity->state_topic);
    if (binary_sensor_entity->payload_on==NULL) binary_sensor_entity->payload_on = "ON";
    cJSON_AddStringToObject(root, "payload_on", binary_sensor_entity->payload_on);
    if (binary_sensor_entity->payload_off==NULL) binary_sensor_entity->payload_off = "OFF";
    cJSON_AddStringToObject(root, "payload_off", binary_sensor_entity->payload_off);

    if (binary_sensor_entity->value_template!=NULL)cJSON_AddStringToObject(root, "value_template", binary_sensor_entity->value_template);
    if (binary_sensor_entity->expire_after)cJSON_AddNumberToObject(root, "expire_after", binary_sensor_entity->expire_after);
    if (binary_sensor_entity->force_update)cJSON_AddTrueToObject(root, "force_update");
    else cJSON_AddFalseToObject(root, "force_update");

    if (binary_sensor_entity->qos) cJSON_AddNumberToObject(root, "qos", binary_sensor_entity->qos);
    if (binary_sensor_entity->retain) cJSON_AddTrueToObject(root, "retain");
    //添加设备信息
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);
    binary_sensor_entity->config_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
}

static void  entity_binary_sensor_add_node(ha_Bsensor_entity_t* binary_sensor_new_node)
{
    if (binary_sensor_new_node==NULL) return;
    ha_Bsensor_entity_t* binary_sensor_list_handle = ha_device->entity_binary_sensor->binary_sensor_list->prev;

    homeAssistant_create_binary_sensor_data(binary_sensor_new_node, homeAssistant_device_create());
    if (binary_sensor_new_node->entity_config_topic==NULL) {
        binary_sensor_new_node->entity_config_topic = pvPortMalloc(128);
        memset(binary_sensor_new_node->entity_config_topic, 0, 128);
        sprintf(binary_sensor_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_BINARY_SENSOR, binary_sensor_new_node->unique_id);

    }
    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(binary_sensor_new_node->entity_config_topic, binary_sensor_new_node->config_data, 0, 1);
        if (binary_sensor_new_node->availability_topic!=NULL)
            homeAssistant_mqtt_port_public(binary_sensor_new_node->availability_topic, binary_sensor_new_node->payload_available, 0, 0);
        // HA_LOG_D("Topic:%s\r\n", binary_sensor_new_node->entity_config_topic);
        // HA_LOG_D("config data:%s\r\n", binary_sensor_new_node->config_data);
    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }

    binary_sensor_list_handle->next = binary_sensor_new_node;
    binary_sensor_new_node->prev = binary_sensor_list_handle;
    binary_sensor_new_node->next = ha_device->entity_binary_sensor->binary_sensor_list;
    ha_device->entity_binary_sensor->binary_sensor_list->prev = binary_sensor_new_node;
    vPortFree(binary_sensor_new_node->config_data);
}
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
static void homeAssistant_create_text_data(ha_text_entity_t* text_entity, cJSON* device_json)
{
    if (text_entity==NULL) {
        HA_LOG_E("entity light buff is NULL\r\n");
        return;
    }

    cJSON* root = cJSON_CreateObject();
    if (text_entity->name!=NULL)cJSON_AddStringToObject(root, "name", text_entity->name);
    if (text_entity->unique_id!=NULL)
    {
        char* unique_id = unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        HA_LOG_F("unique_id =%s\r\n", unique_id);
        text_entity->unique_id = strncat(text_entity->unique_id, unique_id, 5);
        cJSON_AddStringToObject(root, "unique_id", text_entity->unique_id);
        vPortFree(unique_id);

    }
    else HA_LOG_E("unique id is null for entity:%s \r\n", text_entity->name);

    if (text_entity->object_id!=NULL)cJSON_AddStringToObject(root, "object_id", text_entity->object_id);
    if (text_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", text_entity->icon);
    if (text_entity->availability_template!=NULL)cJSON_AddStringToObject(root, "availability_template", text_entity->availability_template);

    if (text_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", text_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (text_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", text_entity->payload_available);
    else  cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (text_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", text_entity->payload_not_available);
    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);

    if (text_entity->json_attributes_template!=NULL) cJSON_AddStringToObject(root, "json_attributes_template", text_entity->json_attributes_template);
    if (text_entity->json_attributes_topic!=NULL)cJSON_AddStringToObject(root, "json_attributes_topic", text_entity->json_attributes_topic);
    if (text_entity->enabled_by_default)cJSON_AddTrueToObject(root, "enabled_by_default");
    if (text_entity->entity_category!=NULL)cJSON_AddStringToObject(root, "entity_category", text_entity->entity_category);
    if (text_entity->encoding!=NULL)cJSON_AddStringToObject(root, "encoding", text_entity->encoding);

    if (text_entity->max)cJSON_AddNumberToObject(root, "max", text_entity->max);
    if (text_entity->min) cJSON_AddNumberToObject(root, "min", text_entity->min);
    if (text_entity->mode!=NULL)cJSON_AddStringToObject(root, "mode", text_entity->mode);
    else cJSON_AddStringToObject(root, "mode", "text");
    if (text_entity->command_template!=NULL)cJSON_AddStringToObject(root, "command_template", text_entity->command_template);
    if (text_entity->command_topic==NULL)
    {
        text_entity->command_topic = pvPortMalloc(128);
        memset(text_entity->command_topic, 0, 128);
        sprintf(text_entity->command_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/set", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], text_entity->unique_id);
    }
    cJSON_AddStringToObject(root, "command_topic", text_entity->command_topic);

    if (text_entity->state_topic==NULL) {
        text_entity->state_topic = pvPortMalloc(128);
        memset(text_entity->state_topic, 0, 128);
        sprintf(text_entity->state_topic, "%s/%02x%02x%02x%02x%02x%02x/%s/state", ha_device->name, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5], text_entity->unique_id);
    }
    cJSON_AddStringToObject(root, "state_topic", text_entity->state_topic);

    if (text_entity->value_template!=NULL)cJSON_AddStringToObject(root, "value_template", text_entity->value_template);
    if (text_entity->pattern!=NULL)cJSON_AddStringToObject(root, "pattern", text_entity->pattern);

    if (text_entity->qos) cJSON_AddNumberToObject(root, "qos", text_entity->qos);
    if (text_entity->retain) cJSON_AddTrueToObject(root, "retain");

    //添加设备信息
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);
    text_entity->config_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
}
/**
 * @brief 插入 text 实体节点
 *
 * @param text_new_node
*/
static void  entity_text_add_node(ha_text_entity_t* text_new_node)
{

    ha_text_entity_t* text_list_handle = ha_device->entity_text->text_list->prev;
    //开始创建新的实体数据
    homeAssistant_create_text_data(text_new_node, homeAssistant_device_create());
    if (text_new_node->entity_config_topic==NULL) {
        text_new_node->entity_config_topic = pvPortMalloc(128);
        memset(text_new_node->entity_config_topic, 0, 128);
        sprintf(text_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_TEXT, text_new_node->unique_id);
    }
    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(text_new_node->entity_config_topic, text_new_node->config_data, 1, 1);
        if (text_new_node->command_topic!=NULL)homeAssistant_mqtt_port_subscribe(text_new_node->command_topic, 1);
        if (text_new_node->availability_topic!=NULL)
            homeAssistant_mqtt_port_public(text_new_node->availability_topic, text_new_node->payload_available, 0, 0);
    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }
    //创建时检查默认内容是否存在
    if (text_new_node->text_value!=NULL) {
        char* value_buff = pvPortMalloc(256);
        memset(value_buff, 0, 256);
        sprintf(value_buff, "%s", text_new_node->text_value);
        memset(text_new_node->text_value, 0, strlen(text_new_node->text_value));
        text_new_node->text_value = pvPortMalloc(256);
        memset(text_new_node->text_value, 0, 256);
        sprintf(text_new_node->text_value, "%s", value_buff);
        vPortFree(value_buff);
    }
    //插入节点
    text_list_handle->next = text_new_node;
    text_new_node->prev = text_list_handle;
    text_new_node->next = ha_device->entity_text->text_list;
    ha_device->entity_text->text_list->prev = text_new_node;
    vPortFree(text_new_node->config_data);
}
#endif
/**
 * @brief 空调设备
 *
*/
#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC

static char* modes_def[] = { "auto",  "cool", "heat", "dry", "fan_only" };
static char* fan_modes_def[] = { "auto", "low", "medium", "high" };
static char* preset_modes_def[] = { "eco", "away", "boost", "comfort", "home", "sleep", "activity" };

static void homeAssistant_create_climate_HVAC_data(ha_climateHVAC_t* climateHVAC_entity, cJSON* device_json)
{
    if (climateHVAC_entity==NULL) {
        HA_LOG_E("entity climateHVAC buff is NULL\r\n");
        return;
    }
    cJSON* root = cJSON_CreateObject();
    if (climateHVAC_entity->name!=NULL)cJSON_AddStringToObject(root, "name", climateHVAC_entity->name);
    if (climateHVAC_entity->unique_id!=NULL)
    {
        char* unique_id = pvPortMalloc(5);
        memset(unique_id, 0, 5);
        sprintf(unique_id, "-%02x%2x", STA_MAC[4], STA_MAC[5]);
        climateHVAC_entity->unique_id = strncat(climateHVAC_entity->unique_id, unique_id, 5);
        HA_LOG_F("unique_id =%s\r\n", climateHVAC_entity->unique_id);
        cJSON_AddStringToObject(root, "unique_id", climateHVAC_entity->unique_id);
        vPortFree(unique_id);
    }

    if (climateHVAC_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", climateHVAC_entity->icon);
    if (climateHVAC_entity->availability_template!=NULL)cJSON_AddStringToObject(root, "availability_template", climateHVAC_entity->availability_template);

    if (climateHVAC_entity->availability_topic!=NULL)cJSON_AddStringToObject(root, "availability_topic", climateHVAC_entity->availability_topic);
    else cJSON_AddStringToObject(root, "availability_topic", ha_device->availability_topic);
    if (climateHVAC_entity->payload_available!=NULL)cJSON_AddStringToObject(root, "payload_available", climateHVAC_entity->payload_available);
    else  cJSON_AddStringToObject(root, "payload_available", ha_device->payload_available);
    if (climateHVAC_entity->payload_not_available!=NULL)cJSON_AddStringToObject(root, "payload_not_available", climateHVAC_entity->payload_not_available);
    else cJSON_AddStringToObject(root, "payload_not_available", ha_device->payload_not_available);

    if (climateHVAC_entity->payload_off!=NULL) cJSON_AddStringToObject(root, "payload_off", climateHVAC_entity->payload_off);
    if (climateHVAC_entity->payload_on!=NULL) cJSON_AddStringToObject(root, "payload_on", climateHVAC_entity->payload_on);
    //定义监听湿度的TOPIC
    if (climateHVAC_entity->current_humidity_template!=NULL) cJSON_AddStringToObject(root, "current_humidity_template", climateHVAC_entity->current_humidity_template);
    if (climateHVAC_entity->current_humidity_topic!=NULL)cJSON_AddStringToObject(root, "current_humidity_topic", climateHVAC_entity->current_humidity_topic);
    //定义监听温度的topic
    if (climateHVAC_entity->current_temperature_template!=NULL)cJSON_AddStringToObject(root, "current_temperature_template", climateHVAC_entity->current_temperature_template);
    if (climateHVAC_entity->current_temperature_topic!=NULL)cJSON_AddStringToObject(root, "current_temperature_topic", climateHVAC_entity->current_temperature_topic);
    //
    if (!climateHVAC_entity->enabled_by_default)cJSON_AddFalseToObject(root, "enabled_by_default");
    if (climateHVAC_entity->encoding!=NULL)cJSON_AddStringToObject(root, "encoding", climateHVAC_entity->encoding);
    if (climateHVAC_entity->entity_category!=NULL)cJSON_AddStringToObject(root, "entity_category", climateHVAC_entity->entity_category);

    //风力模式控制
    if (climateHVAC_entity->fan_mode_command_template!=NULL)cJSON_AddStringToObject(root, "fan_mode_command_template", climateHVAC_entity->fan_mode_command_template);
    if (climateHVAC_entity->fan_mode_command_topic!=NULL)cJSON_AddStringToObject(root, "fan_mode_command_topic", climateHVAC_entity->fan_mode_command_topic);
    if (climateHVAC_entity->fan_mode_state_template!=NULL)cJSON_AddStringToObject(root, "fan_mode_state_template", climateHVAC_entity->fan_mode_state_template);
    if (climateHVAC_entity->fan_modes[0]!=NULL) {
        cJSON* fan_modes = cJSON_CreateArray();
        if (climateHVAC_entity->fan_modes[0]!=NULL) cJSON_AddItemToArray(fan_modes, cJSON_CreateString(climateHVAC_entity->fan_modes[0]));
        if (climateHVAC_entity->fan_modes[1]!=NULL) cJSON_AddItemToArray(fan_modes, cJSON_CreateString(climateHVAC_entity->fan_modes[1]));
        if (climateHVAC_entity->fan_modes[2]!=NULL) cJSON_AddItemToArray(fan_modes, cJSON_CreateString(climateHVAC_entity->fan_modes[2]));
        if (climateHVAC_entity->fan_modes[3]!=NULL) cJSON_AddItemToArray(fan_modes, cJSON_CreateString(climateHVAC_entity->fan_modes[3]));
        cJSON_AddItemToObject(root, "fan_modes", fan_modes);
    }
    if (climateHVAC_entity->initial!=0.00)cJSON_AddNumberToObject(root, "initial", climateHVAC_entity->initial);
    if (climateHVAC_entity->icon!=NULL)cJSON_AddStringToObject(root, "icon", climateHVAC_entity->icon);
    if (climateHVAC_entity->json_attributes_template!=NULL)cJSON_AddStringToObject(root, "json_attributes_template", climateHVAC_entity->json_attributes_template);
    if (climateHVAC_entity->json_attributes_topic!=NULL)cJSON_AddStringToObject(root, "json_attributes_topic", climateHVAC_entity->json_attributes_topic);

    if (climateHVAC_entity->max_humidity!=0.00)cJSON_AddNumberToObject(root, "max_humidity", climateHVAC_entity->max_humidity);
    if (climateHVAC_entity->max_temp!=0.00)cJSON_AddNumberToObject(root, "max_temp", climateHVAC_entity->max_temp);
    if (climateHVAC_entity->min_humidity!=0.0)cJSON_AddNumberToObject(root, "min_humidity", climateHVAC_entity->min_humidity);
    if (climateHVAC_entity->min_temp!=0.0)cJSON_AddNumberToObject(root, "min_temp", climateHVAC_entity->min_temp);
    //定义模式
    if (climateHVAC_entity->mode_command_template!=NULL)cJSON_AddStringToObject(root, "mode_command_template", climateHVAC_entity->mode_command_template);
    if (climateHVAC_entity->mode_command_topic!=NULL)cJSON_AddStringToObject(root, "mode_command_topic", climateHVAC_entity->mode_command_topic);
    if (climateHVAC_entity->mode_state_template!=NULL)cJSON_AddStringToObject(root, "mode_state_template", climateHVAC_entity->mode_state_template);
    if (climateHVAC_entity->mode_state_topic!=NULL)cJSON_AddStringToObject(root, "mode_state_topic", climateHVAC_entity->mode_state_topic);
    if (climateHVAC_entity->modes[0]!=NULL) {
        cJSON* modes = cJSON_CreateArray();
        if (climateHVAC_entity->modes[0]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[0]));
        if (climateHVAC_entity->modes[1]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[1]));
        if (climateHVAC_entity->modes[2]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[2]));
        if (climateHVAC_entity->modes[3]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[3]));
        if (climateHVAC_entity->modes[4]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[4]));
        if (climateHVAC_entity->modes[5]!=NULL) cJSON_AddItemToArray(modes, cJSON_CreateString(climateHVAC_entity->modes[5]));
        cJSON_AddItemToObject(root, "modes", modes);
    }
    //定义开关
    if (climateHVAC_entity->power_command_template!=NULL)cJSON_AddStringToObject(root, "power_command_template", climateHVAC_entity->power_command_template);
    if (climateHVAC_entity->power_command_topic!=NULL) cJSON_AddStringToObject(root, "power_command_topic", climateHVAC_entity->power_command_topic);
    //控制精度
    if (climateHVAC_entity->precision!=0.0)cJSON_AddNumberToObject(root, "precision", climateHVAC_entity->precision);
    //预设模式配置
    if (climateHVAC_entity->preset_mode_command_template!=NULL)cJSON_AddStringToObject(root, "preset_mode_command_template", climateHVAC_entity->preset_mode_command_template);
    if (climateHVAC_entity->preset_mode_command_topic!=NULL)cJSON_AddStringToObject(root, "preset_mode_command_topic", climateHVAC_entity->preset_mode_command_topic);
    if (climateHVAC_entity->preset_mode_state_topic!=NULL)cJSON_AddStringToObject(root, "preset_mode_state_topic", climateHVAC_entity->preset_mode_state_topic);
    if (climateHVAC_entity->preset_mode_value_template!=NULL)cJSON_AddStringToObject(root, "preset_mode_value_template", climateHVAC_entity->preset_mode_value_template);
    if (climateHVAC_entity->preset_modes[0]!=NULL) {
        cJSON* preset_modes = cJSON_CreateArray();
        if (climateHVAC_entity->preset_modes[0]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[0]));
        if (climateHVAC_entity->preset_modes[1]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[1]));
        if (climateHVAC_entity->preset_modes[2]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[2]));
        if (climateHVAC_entity->preset_modes[3]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[3]));
        if (climateHVAC_entity->preset_modes[4]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[4]));
        if (climateHVAC_entity->preset_modes[5]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[5]));
        if (climateHVAC_entity->preset_modes[6]!=NULL) cJSON_AddItemToArray(preset_modes, cJSON_CreateString(climateHVAC_entity->preset_modes[6]));
        cJSON_AddItemToObject(root, "preset_modes", preset_modes);
    }
    if (climateHVAC_entity->qos!=0) cJSON_AddNumberToObject(root, "qos", climateHVAC_entity->qos);
    if (climateHVAC_entity->retain) cJSON_AddTrueToObject(root, "retain");
    //摆风模式
    if (climateHVAC_entity->swing_mode_command_template!=NULL)cJSON_AddStringToObject(root, "swing_mode_command_template", climateHVAC_entity->swing_mode_command_template);
    if (climateHVAC_entity->swing_mode_command_topic!=NULL)cJSON_AddStringToObject(root, "swing_mode_command_topic", climateHVAC_entity->swing_mode_command_topic);
    if (climateHVAC_entity->swing_mode_state_template!=NULL)cJSON_AddStringToObject(root, "swing_mode_state_template", climateHVAC_entity->swing_mode_state_template);
    if (climateHVAC_entity->swing_mode_state_topic!=NULL)cJSON_AddStringToObject(root, "swing_mode_state_topic", climateHVAC_entity->swing_mode_state_topic);
    if (climateHVAC_entity->swing_modes[0]!=NULL) {
        cJSON* swing_modes = cJSON_CreateArray();
        if (climateHVAC_entity->swing_modes[0]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[0]));
        if (climateHVAC_entity->swing_modes[1]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[1]));
        if (climateHVAC_entity->swing_modes[2]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[2]));
        if (climateHVAC_entity->swing_modes[3]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[3]));
        if (climateHVAC_entity->swing_modes[4]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[4]));
        if (climateHVAC_entity->swing_modes[5]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[5]));
        if (climateHVAC_entity->swing_modes[6]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[6]));
        if (climateHVAC_entity->swing_modes[7]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[7]));
        if (climateHVAC_entity->swing_modes[8]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[8]));
        if (climateHVAC_entity->swing_modes[9]!=NULL) cJSON_AddItemToArray(swing_modes, cJSON_CreateString(climateHVAC_entity->swing_modes[9]));
        cJSON_AddItemToObject(root, "swing_modes", swing_modes);
    }
    //目标湿度
    if (climateHVAC_entity->target_humidity_command_template!=NULL)cJSON_AddStringToObject(root, "target_humidity_command_template", climateHVAC_entity->target_humidity_command_template);
    if (climateHVAC_entity->target_humidity_command_topic!=NULL)cJSON_AddStringToObject(root, "target_humidity_command_topic", climateHVAC_entity->target_humidity_command_topic);

    if (climateHVAC_entity->target_humidity_state_template!=NULL)cJSON_AddStringToObject(root, "target_humidity_state_template", climateHVAC_entity->target_humidity_state_template);
    if (climateHVAC_entity->target_humidity_state_topic!=NULL)cJSON_AddStringToObject(root, "target_humidity_state_topic", climateHVAC_entity->target_humidity_state_topic);
    //目标温度配置
    if (climateHVAC_entity->temperature_command_template!=NULL)cJSON_AddStringToObject(root, "temperature_command_template", climateHVAC_entity->temperature_command_template);
    if (climateHVAC_entity->temperature_command_topic!=NULL)cJSON_AddStringToObject(root, "temperature_command_topic", climateHVAC_entity->temperature_command_topic);

    if (climateHVAC_entity->temperature_high_command_template!=NULL)cJSON_AddStringToObject(root, "temperature_high_command_template", climateHVAC_entity->temperature_high_command_template);
    if (climateHVAC_entity->temperature_high_command_topic!=NULL)cJSON_AddStringToObject(root, "temperature_high_command_topic", climateHVAC_entity->temperature_high_command_topic);

    if (climateHVAC_entity->temperature_high_state_template!=NULL)cJSON_AddStringToObject(root, "temperature_high_state_template", climateHVAC_entity->temperature_high_state_template);
    if (climateHVAC_entity->temperature_high_state_topic!=NULL)cJSON_AddStringToObject(root, "temperature_high_state_topic", climateHVAC_entity->temperature_high_state_topic);

    if (climateHVAC_entity->temperature_low_command_template!=NULL)cJSON_AddStringToObject(root, "temperature_low_command_template", climateHVAC_entity->temperature_low_command_template);
    if (climateHVAC_entity->temperature_low_command_topic!=NULL)cJSON_AddStringToObject(root, "temperature_low_command_topic", climateHVAC_entity->temperature_low_command_topic);

    if (climateHVAC_entity->temperature_low_state_template!=NULL)cJSON_AddStringToObject(root, "temperature_low_state_template", climateHVAC_entity->temperature_low_state_template);
    if (climateHVAC_entity->temperature_low_state_topic!=NULL)cJSON_AddStringToObject(root, "temperature_low_state_topic", climateHVAC_entity->temperature_low_state_topic);

    if (climateHVAC_entity->temperature_state_template!=NULL)cJSON_AddStringToObject(root, "temperature_state_template", climateHVAC_entity->temperature_state_template);
    if (climateHVAC_entity->temperature_state_topic!=NULL)cJSON_AddStringToObject(root, "temperature_state_topic", climateHVAC_entity->temperature_state_topic);

    if (climateHVAC_entity->temperature_unit!=NULL)cJSON_AddStringToObject(root, "temperature_unit", climateHVAC_entity->temperature_unit);
    if (climateHVAC_entity->temp_step!=0.0)cJSON_AddNumberToObject(root, "temp_step", climateHVAC_entity->temp_step);

    if (climateHVAC_entity->value_template!=NULL)cJSON_AddStringToObject(root, "value_template", climateHVAC_entity->value_template);
    if (device_json!=NULL)cJSON_AddItemToObject(root, "device", device_json);
    climateHVAC_entity->config_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
}

static void  entity_climate_HVAC_add_node(ha_climateHVAC_t* climateHVAC_new_node)
{
    ha_climateHVAC_t* climateHVAC_list_handle = ha_device->entity_climateHVAC->climateHVAC_list->prev;
    //初始化相关的参数
    climateHVAC_new_node->enabled_by_default = true;
    climateHVAC_new_node->modes_type = AC_MODES_AUTO;
    //
    if (climateHVAC_new_node->action_topic==NULL) {
        climateHVAC_new_node->action_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->action_topic, 0, 256);
        sprintf(climateHVAC_new_node->action_topic, "%s/%s/%s/action/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    //开关
    if (climateHVAC_new_node->power_command_topic==NULL) {
        climateHVAC_new_node->power_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->power_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->power_command_topic, "%s/%s/%s/power/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
        if (climateHVAC_new_node->payload_on==NULL)climateHVAC_new_node->payload_on = "ON";
        if (climateHVAC_new_node->payload_off==NULL)climateHVAC_new_node->payload_off = "OFF";
    }
    //模式
    if (climateHVAC_new_node->mode_command_topic==NULL) {
        climateHVAC_new_node->mode_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->mode_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->mode_command_topic, "%s/%s/%s/modes/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    if (climateHVAC_new_node->mode_command_template==NULL) {
        climateHVAC_new_node->mode_command_template = "{\"mode\":\"{{value}}\"}";
    }

    if (climateHVAC_new_node->mode_state_topic==NULL) {
        climateHVAC_new_node->mode_state_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->mode_state_topic, 0, 256);
        sprintf(climateHVAC_new_node->mode_state_topic, "%s/%s/%s/modes/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    //温度
    // if (climateHVAC_new_node->current_temperature_topic==NULL) {
    //     climateHVAC_new_node->current_temperature_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->current_temperature_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->current_temperature_topic, "%s/%s/%s/current_temperature/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    if (climateHVAC_new_node->temperature_command_template==NULL)climateHVAC_new_node->temperature_command_template = "{\"temperature\":{{value}}}";

    if (climateHVAC_new_node->temperature_command_topic==NULL) {
        climateHVAC_new_node->temperature_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->temperature_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->temperature_command_topic, "%s/%s/%s/temperature/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    // if (climateHVAC_new_node->temperature_state_topic==NULL) {
    //     climateHVAC_new_node->temperature_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->temperature_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->temperature_state_topic, "%s/%s/%s/temperature/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    // if (climateHVAC_new_node->max_temp==0.0) climateHVAC_new_node->max_temp = 30.0;
    // if (climateHVAC_new_node->min_temp==0.0) climateHVAC_new_node->min_temp = 17.0;

    // if (climateHVAC_new_node->temperature_high_command_template==NULL)climateHVAC_new_node->temperature_high_command_template = "{\"high_temperature\":{{value}}}";
    // if (climateHVAC_new_node->temperature_high_command_topic==NULL) {
    //     climateHVAC_new_node->temperature_high_command_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->temperature_high_command_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->temperature_high_command_topic, "%s/%s/%s/high_temperature/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    // if (climateHVAC_new_node->temperature_high_state_topic==NULL) {
    //     climateHVAC_new_node->temperature_high_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->temperature_high_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->temperature_high_state_topic, "%s/%s/%s/high_temperature/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }

    if (climateHVAC_new_node->temperature_low_command_template==NULL)climateHVAC_new_node->temperature_low_command_template = "{\"low_temperature\":{{value}}}";
    if (climateHVAC_new_node->temperature_low_command_topic==NULL) {
        climateHVAC_new_node->temperature_low_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->temperature_low_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->temperature_low_command_topic, "%s/%s/%s/low_temperature/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    // if (climateHVAC_new_node->temperature_low_state_topic==NULL) {
    //     climateHVAC_new_node->temperature_low_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->temperature_low_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->temperature_low_state_topic, "%s/%s/%s/low_temperature/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }


    // 预设值
    // if (climateHVAC_new_node->preset_mode_command_topic==NULL) {
    //     climateHVAC_new_node->preset_mode_command_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->preset_mode_command_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->preset_mode_command_topic, "%s/%s/%s/preset_mode/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    // if (climateHVAC_new_node->preset_mode_state_topic==NULL) {
    //     climateHVAC_new_node->preset_mode_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->preset_mode_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->preset_mode_state_topic, "%s/%s/%s/preset_mode/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    // //风扇模式
    if (climateHVAC_new_node->fan_mode_command_topic==NULL) {
        climateHVAC_new_node->fan_mode_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->fan_mode_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->fan_mode_command_topic, "%s/%s/%s/fan_mode/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    // if (climateHVAC_new_node->fan_mode_state_topic==NULL) {
    //     climateHVAC_new_node->fan_mode_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->fan_mode_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->fan_mode_state_topic, "%s/%s/%s/fan_mode/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }
    //摆风模式
    if (climateHVAC_new_node->swing_mode_command_topic==NULL) {
        climateHVAC_new_node->swing_mode_command_topic = pvPortMalloc(256);
        memset(climateHVAC_new_node->swing_mode_command_topic, 0, 256);
        sprintf(climateHVAC_new_node->swing_mode_command_topic, "%s/%s/%s/swing_mode/set", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    }
    // if (climateHVAC_new_node->swing_mode_state_topic==NULL) {
    //     climateHVAC_new_node->swing_mode_state_topic = pvPortMalloc(256);
    //     memset(climateHVAC_new_node->swing_mode_state_topic, 0, 256);
    //     sprintf(climateHVAC_new_node->swing_mode_state_topic, "%s/%s/%s/swing_mode/state", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
    // }

    homeAssistant_create_climate_HVAC_data(climateHVAC_new_node, homeAssistant_device_create());


    if (climateHVAC_new_node->entity_config_topic==NULL) {
        climateHVAC_new_node->entity_config_topic = pvPortMalloc(128);
        memset(climateHVAC_new_node->entity_config_topic, 0, 128);
        sprintf(climateHVAC_new_node->entity_config_topic, "%s/%s/%s/config", CONFIG_HA_AUTOMATIC_DISCOVERY, CONFIG_HA_ENTITY_CLIMATE_HVAC, climateHVAC_new_node->unique_id);
        HA_LOG_I("config topic=%s\r\n", climateHVAC_new_node->entity_config_topic);
    }
    if (ha_device->mqtt_info.mqtt_connect_status) {
        homeAssistant_mqtt_port_public(climateHVAC_new_node->entity_config_topic, climateHVAC_new_node->config_data, 1, 1);
        if (climateHVAC_new_node->availability_topic!=NULL)homeAssistant_mqtt_port_public(climateHVAC_new_node->availability_topic, climateHVAC_new_node->payload_available, 0, 0);

        if (climateHVAC_new_node->power_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->power_command_topic, 1);
        if (climateHVAC_new_node->mode_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->mode_command_topic, 1);
        if (climateHVAC_new_node->temperature_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->temperature_command_topic, 1);
        if (climateHVAC_new_node->temperature_low_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->temperature_low_command_topic, 1);
        if (climateHVAC_new_node->temperature_high_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->temperature_high_command_topic, 1);
        if (climateHVAC_new_node->current_temperature_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->current_temperature_topic, 1);
        if (climateHVAC_new_node->preset_mode_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->preset_mode_command_topic, 1);
        if (climateHVAC_new_node->fan_mode_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->fan_mode_command_topic, 1);
        if (climateHVAC_new_node->swing_mode_command_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->swing_mode_command_topic, 1);
        if (climateHVAC_new_node->action_topic!=NULL)homeAssistant_mqtt_port_subscribe(climateHVAC_new_node->action_topic, 1);

    }
    else {
        HA_LOG_E("MQTT server is diconnenct\r\n");
    }

    climateHVAC_list_handle->next = climateHVAC_new_node;
    climateHVAC_new_node->prev = climateHVAC_new_node;
    climateHVAC_new_node->next = ha_device->entity_climateHVAC->climateHVAC_list;
    ha_device->entity_climateHVAC->climateHVAC_list->prev = climateHVAC_new_node;

    vPortFree(climateHVAC_new_node->config_data);
}
#endif
/**
 * @brief homeAssistant_get_command
 *      获取 服务器指令事件，并把相应实体指针指向对应的实体
 * @param topic
 * @param topic_len
 * @param data
 * @param data_len
 * @return ha_event_t
*/
ha_event_t homeAssistant_get_command(const char* topic, unsigned short topic_len, const char* data, unsigned short data_len)
{
    if (topic==NULL ||data==NULL) {
        HA_LOG_E("params is NULL\r\n");
        return HA_EVENT_NONE;
    }
    ha_event_t event = HA_EVENT_NONE;
    //识别homeassistant 平台的出生消息
    if (!strncmp(topic, CONFIG_HA_STATUS_TOPIC, topic_len)) {
        if (!strncmp(data, CONFIG_HA_STATUS_MESSAGE_ON, data_len)) {
            event = HA_EVENT_HOMEASSISTANT_STATUS_ONLINE;
            ha_device->homeassistant_online = true;
        }
        else {
            event = HA_EVENT_HOMEASSISTANT_STATUS_OFFLINE;
            ha_device->homeassistant_online = false;
        }
        return event;
    }
    //查找出switch 实体
#if CONFIG_ENTITY_ENABLE_SWITCH
    ha_sw_entity_t* switch_cur = ha_device->entity_switch->switch_list->next;

    while (switch_cur!=ha_device->entity_switch->switch_list) {
        if (!strncmp(topic, switch_cur->command_topic, topic_len)) {
            if (switch_cur->payload_on!=NULL)
                switch_cur->switch_state = strncmp(data, switch_cur->payload_on, data_len)?false:true;
            else
                switch_cur->switch_state = strncmp(data, "ON", data_len)?false:true;
            event = HA_EVENT_MQTT_COMMAND_SWITCH;
            //找出该实体之后，方便后面操作实体
            ha_device->entity_switch->command_switch = switch_cur;
            return event;
        }
        //指针指向下一个节点
        if (switch_cur->next==ha_device->entity_switch->switch_list) break;
        else
            switch_cur = switch_cur->next;
    }
#endif
    //查找相应的Light 实体
#if CONFIG_ENTITY_ENABLE_LIGHT
    ha_lh_entity_t* light_cur = ha_device->entity_light->light_list->next;
    while (light_cur!=ha_device->entity_light->light_list)
    {
        if (!strncmp(topic, light_cur->rgb.rgb_command_topic, topic_len)) {
            homeAssistant_get_light_rgb(light_cur, data, data_len);
            event = HA_EVENT_MQTT_COMMAND_LIGHT_RGB_UPDATE;
            light_cur->light_state = true;
            ha_device->entity_light->command_light = light_cur;
            return event;
        }
        else  if (!strncmp(topic, light_cur->command_topic, topic_len)) {
            if (light_cur->payload_on!=NULL)light_cur->light_state = strncmp(data, light_cur->payload_on, data_len)?false:true;
            else light_cur->light_state = strncmp(data, "ON", data_len)?false:true;
            event = HA_EVENT_MQTT_COMMAND_LIGHT_SWITCH;
            ha_device->entity_light->command_light = light_cur;
            return event;
        }

        if (light_cur->next==ha_device->entity_light->light_list)break;
        else
            light_cur = light_cur->next;
    }
#endif

    //查找相应的tet 实体
#if CONFIG_ENTITY_ENABLE_TEXT
    ha_text_entity_t* text_cur = ha_device->entity_text->text_list->next;

    while (text_cur!=ha_device->entity_text->text_list) {
        if (!strncmp(topic, text_cur->command_topic, topic_len)) {
            memset(text_cur->text_value, 0, 256);
            strncpy(text_cur->text_value, data, data_len);
            event = HA_EVENT_MQTT_COMMAND_TEXT_VALUE;
            ha_device->entity_text->command_text = text_cur;
            return event;
        }
        if (text_cur->next == ha_device->entity_text->text_list)break;
        else
            switch_cur = switch_cur->next;
    }
#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    ha_climateHVAC_t* climateHVAC_cur = ha_device->entity_climateHVAC->climateHVAC_list->next;
    while (climateHVAC_cur!=ha_device->entity_climateHVAC->climateHVAC_list)
    {
        //通过开关
        if (climateHVAC_cur->power_command_topic!=NULL) {
            if (!strncmp(topic, climateHVAC_cur->power_command_topic, topic_len)) {
                if (!strncmp(data, climateHVAC_cur->payload_on, data_len)) {
                    climateHVAC_cur->power_state = true;
                }
                else {
                    climateHVAC_cur->power_state = false;
                }

                // if (climateHVAC_cur->payload_on==NULL) {
                //     climateHVAC_cur->payload_on = pvPortMalloc(2);

                // }
                // else if (!strcmp(data, "ON")) {
                //     climateHVAC_cur->power_state = true;
                // }
                // if (climateHVAC_cur->payload_off==NULL) {
                //     climateHVAC_cur->payload_off = pvPortMalloc(3);
                //     if (!strcmp(data, "OFF")) {
                //         climateHVAC_cur->power_state = false;
                //     }
                // }
                // else 

                event = HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_POWER;
                ha_device->entity_climateHVAC->command_climateHVAC = climateHVAC_cur;
                return event;
            }
        }
        //识别出模式设置事件
        if (climateHVAC_cur->mode_command_topic!=NULL) {
            if (!strncmp(topic, climateHVAC_cur->mode_command_topic, topic_len)) {
                cJSON* root = cJSON_Parse(data);
                if (root==NULL) {
                    HA_LOG_E("%.*s is NO json\r\n", data_len, data);
                    cJSON_Delete(root);
                }
                cJSON* modes = cJSON_GetObjectItem(root, "mode");

                if (climateHVAC_cur->modes[0]==NULL) {
                    for (size_t i = 0; modes_def[i]!=NULL; i++)
                    {
                        if (!memcmp(modes->valuestring, modes_def[i], strlen(modes->valuestring))) {
                            climateHVAC_cur->modes_type = i;
                            goto _exit;
                        }
                    }

                }
                else {

                    for (size_t i = 0; climateHVAC_cur->modes[i]!=NULL; i++)
                    {
                        if (!memcmp(modes->valuestring, climateHVAC_cur->modes[i], strlen(modes->valuestring))) {
                            climateHVAC_cur->modes_type = i;
                            goto _exit;
                        }
                    }
                }
                cJSON_Delete(root);
            }
        _exit:
            event = HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_MODES;
            ha_device->entity_climateHVAC->command_climateHVAC = climateHVAC_cur;
            return event;
        }
        if (climateHVAC_cur->next == ha_device->entity_climateHVAC->climateHVAC_list)break;
        else
            climateHVAC_cur = climateHVAC_cur->next;
    }
#endif
    return event;
}

//单独更新所有实体的配置，当homeAssistant重启时更新实体上线
void update_all_entity_to_homeassistant(void)
{
    //更新所有switch 实体
    int ret = 0;
#if CONFIG_ENTITY_ENABLE_SWITCH

    ha_sw_entity_t* switch_cur = ha_device->entity_switch->switch_list->next;
    if (switch_cur!=ha_device->entity_switch->switch_list) {
        while (switch_cur!=ha_device->entity_switch->switch_list) {
            if (switch_cur->entity_config_topic!=NULL) {
                homeAssistant_create_switch_data(switch_cur, homeAssistant_device_create());
                homeAssistant_mqtt_port_public(switch_cur->entity_config_topic, switch_cur->config_data, 0, 0);
                vPortFree(switch_cur->config_data);
            }
            //实体上线
            if (switch_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(switch_cur->availability_topic, switch_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //实体上报当前状态
            if (ha_device->entity_switch->command_switch!=NULL)
                homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_SWITCH, ha_device->entity_switch->command_switch, ha_device->entity_switch->command_switch->switch_state);
            else
                homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_SWITCH, switch_cur, switch_cur->switch_state);

            switch_cur = switch_cur->next;
        }
    }
#endif
    //更新所有light 实体
#if CONFIG_ENTITY_ENABLE_LIGHT
    ha_lh_entity_t* light_cur = ha_device->entity_light->light_list->next;
    if (light_cur!=ha_device->entity_light->light_list) {
        while (light_cur!=ha_device->entity_light->light_list) {
            homeAssistant_create_light_data(light_cur, homeAssistant_device_create());
            ret = homeAssistant_mqtt_port_public(light_cur->entity_config_topic, light_cur->config_data, 0, 0);
            vPortFree(light_cur->config_data);
            //实体上线
            if (light_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(light_cur->availability_topic, light_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //实体上报当前状态
            if (ha_device->entity_light->command_light!=NULL)
                homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_LIGHT, ha_device->entity_light->command_light, ha_device->entity_light->command_light->light_state);
            else
                homeAssistant_device_send_entity_state(CONFIG_HA_ENTITY_LIGHT, light_cur, light_cur->light_state);
            light_cur = light_cur->next;
        }
    }

#endif
    //更新所有sensor 实体
#if CONFIG_ENTITY_ENABLE_SENSOR 
    ha_sensor_entity_t* sensor_cur = ha_device->entity_sensor->sensor_list->next;
    if (sensor_cur!=ha_device->entity_sensor->sensor_list) {
        while (sensor_cur!=ha_device->entity_sensor->sensor_list) {
            if (sensor_cur->entity_config_topic!=NULL) {
                homeAssistant_create_sensor_data(sensor_cur, homeAssistant_device_create());
                homeAssistant_mqtt_port_public(sensor_cur->entity_config_topic, sensor_cur->config_data, 0, 0);
                vPortFree(sensor_cur->config_data);
            }
            //实体上线
            if (sensor_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(sensor_cur->availability_topic, sensor_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //更新实体状态
            homeAssistant_device_send_entity_state(ha_device->entity_sensor->entity_type, sensor_cur, 0);
            sensor_cur = sensor_cur->next;
        }
    }

#endif
    //更新所有binary_sensor 实体
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    ha_Bsensor_entity_t* binary_sensor_cur = ha_device->entity_binary_sensor->binary_sensor_list->next;
    if (binary_sensor_cur!=ha_device->entity_binary_sensor->binary_sensor_list) {
        while (binary_sensor_cur!= ha_device->entity_binary_sensor->binary_sensor_list) {
            homeAssistant_create_binary_sensor_data(binary_sensor_cur, homeAssistant_device_create());
            homeAssistant_mqtt_port_public(binary_sensor_cur->entity_config_topic, binary_sensor_cur->config_data, 0, 1);
            vPortFree(binary_sensor_cur->config_data);
            //实体上线
            if (binary_sensor_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(binary_sensor_cur->availability_topic, binary_sensor_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //更新实体状态
            homeAssistant_device_send_entity_state(ha_device->entity_binary_sensor->entity_type, binary_sensor_cur, binary_sensor_cur->state);
            binary_sensor_cur = binary_sensor_cur->next;
        }
    }
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    ha_text_entity_t* text_cur = ha_device->entity_text->text_list->next;
    if (text_cur!=ha_device->entity_text->text_list) {
        while (text_cur!=ha_device->entity_text->text_list) {
            homeAssistant_create_text_data(text_cur, homeAssistant_device_create());
            homeAssistant_mqtt_port_public(text_cur->entity_config_topic, text_cur->config_data, 0, 0);
            vPortFree(text_cur->config_data);
            //
            if (text_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(text_cur->availability_topic, text_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //更新实体状态
            if (text_cur->text_value!=NULL) {
                homeAssistant_device_send_entity_state(ha_device->entity_text->entity_type, text_cur, 0);
            }
            text_cur = text_cur->next;
        }
    }

#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    ha_climateHVAC_t* climateHVAC_cur = ha_device->entity_climateHVAC->climateHVAC_list->next;
    if (climateHVAC_cur!=ha_device->entity_climateHVAC->climateHVAC_list) {
        while (climateHVAC_cur!=ha_device->entity_climateHVAC->climateHVAC_list) {
            homeAssistant_create_climate_HVAC_data(climateHVAC_cur, homeAssistant_device_create());
            homeAssistant_mqtt_port_public(climateHVAC_cur->entity_config_topic, climateHVAC_cur->config_data, 0, 0);
            vPortFree(climateHVAC_cur->config_data);

            if (climateHVAC_cur->availability_topic!=NULL)
                ret = homeAssistant_mqtt_port_public(climateHVAC_cur->availability_topic, climateHVAC_cur->payload_available, 0, 1);
            else  ret = homeAssistant_mqtt_port_public(ha_device->availability_topic, ha_device->payload_available, 0, 1);
            //更新实体状态
            // if (climateHVAC_cur.) {
            //     homeAssistant_device_send_entity_state(ha_device->entity_climateHVAC->entity_type, climateHVAC_cur, 0);

            // }
            climateHVAC_cur = climateHVAC_cur->next;
        }
    }
#endif
}



static void homeAssistant_mqtt_init(homeAssisatnt_device_t* ha_dev)
{
    if (ha_dev==NULL) {
        HA_LOG_E("client is NULL\r\n");
        return;
    }
    homeAssistant_mqtt_port_init(ha_dev);
}


void homeAssistant_device_init(homeAssisatnt_device_t* ha_dev, void(*event_cb)(ha_event_t, homeAssisatnt_device_t*))
{
    if (ha_dev==NULL) {
        HA_LOG_E("param is NULL");
        return;
    }
    static char* buff = NULL;

    ha_device = ha_dev;
    homeAssistant_get_sta_mac((char*)STA_MAC);
    if (ha_device->mqtt_info.mqtt_clientID==NULL)
    {
        ha_device->mqtt_info.mqtt_clientID = pvPortMalloc(128);
        memset(ha_device->mqtt_info.mqtt_clientID, 0, 128);
        sprintf(ha_device->mqtt_info.mqtt_clientID, "%s-%02x%02x%02x%02x%02x%02x", CONFIG_HA_MQTT_CLIENT_DEF_ID, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5]);
    }
    if (ha_device->mqtt_info.mqtt_host==NULL)ha_device->mqtt_info.mqtt_host = CONFIG_HA_MQTT_SERVER_HOST;
    if (ha_device->mqtt_info.port==0)ha_device->mqtt_info.port = CONFIG_HA_MQTT_SERVER_PORT;
    if (ha_device->mqtt_info.mqtt_username==NULL)ha_device->mqtt_info.mqtt_username = CONFIG_HA_MQTT_CLIENT_DEF_USERNAME;
    if (ha_device->mqtt_info.mqtt_password==NULL)ha_device->mqtt_info.mqtt_password = CONFIG_HA_MQTT_CLIENT_DEF_PASSWORLD;
    if (ha_device->mqtt_info.mqtt_keeplive==0)ha_device->mqtt_info.mqtt_keeplive = CONFIG_HA_MQTT_CLIENT_DEF_KEEPALIVE;

    if (ha_device->mqtt_info.will.will_msg==NULL) {
        ha_device->mqtt_info.will.will_msg = "offline";
        ha_device->mqtt_info.will.will_msg_len = strlen("offline");
    }
    if (ha_device->mqtt_info.will.will_topic==NULL) {
        buff = pvPortMalloc(128);
        memset(buff, 0, 128);
        sprintf(buff, "%s/%02x%02x%02x%02x%02x%02x/status", CONFIG_HA_AUTOMATIC_DISCOVERY, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5]);
        ha_device->mqtt_info.will.will_topic = buff;
        ha_device->mqtt_info.will.will_qos = 0;
        ha_device->mqtt_info.will.will_retain = true;
    }
    ha_device->mqtt_info.mqtt_connect_status = false;

    HA_LOG_I(".....................MQTT client connect info......................\r\n");
    HA_LOG_I("clientID:%s\r\n", ha_dev->mqtt_info.mqtt_clientID);
    HA_LOG_I("host:%s\r\n", ha_dev->mqtt_info.mqtt_host);
    HA_LOG_I("port:%d\r\n", ha_dev->mqtt_info.port);
    HA_LOG_I("username:%s\r\n", ha_dev->mqtt_info.mqtt_username);
    HA_LOG_I("password:%s\r\n", ha_dev->mqtt_info.mqtt_password);
    HA_LOG_I("will topic:%s\r\n", ha_device->mqtt_info.will.will_topic);
    HA_LOG_I("will msg:%s\r\n", ha_device->mqtt_info.will.will_msg);
    HA_LOG_I("...................................................................\r\n");


    if (ha_device->name==NULL)ha_device->name = CONFIG_HA_DEVICE_NAME;
    if (ha_device->hw_version==NULL)ha_device->hw_version = CONFIG_HA_DEVICE_HW_VERSION;
    if (ha_device->model==NULL)ha_device->model = CONFIG_HA_DEVICE_MODULE;
    if (ha_device->identifiers==NULL) {
        ha_device->identifiers = pvPortMalloc(64);
        memset(ha_device->identifiers, 0, 64);
        sprintf(ha_device->identifiers, "%s_%02x%02x", ha_device->name, STA_MAC[4], STA_MAC[5]);
    }
    if (ha_device->sw_version==NULL)ha_device->sw_version = CONFIG_HA_DEVICE_SW_VERSION;
    if (ha_device->payload_available==NULL)ha_device->payload_available = "online";
    if (ha_device->payload_not_available==NULL)ha_device->payload_not_available = "offline";
    if (ha_device->manufacturer==NULL)ha_device->manufacturer = CONFIG_HA_DEVICE_MANUFACTURER;

    if (ha_device->availability_topic==NULL) {
        ha_device->availability_topic = pvPortMalloc(128);
        memset(ha_device->availability_topic, 0, 128);
        sprintf(ha_device->availability_topic, "%s/%02x%02x%02x%02x%02x%02x/status", CONFIG_HA_AUTOMATIC_DISCOVERY, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5]);
        // ha_device->availability_topic = ha_device->mqtt_info.will.will_topic;
    }
    //初始化 binary sensor 实体
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    ha_device->entity_binary_sensor = pvPortMalloc(sizeof(ha_Bsensor_entity_t));
    ha_device->entity_binary_sensor->entity_type = CONFIG_HA_ENTITY_BINARY_SENSOR;
    ha_device->entity_binary_sensor->binary_sensor_list = pvPortMalloc(sizeof(ha_Bsensor_entity_t));
    ha_device->entity_binary_sensor->binary_sensor_list->prev = ha_device->entity_binary_sensor->binary_sensor_list;
    ha_device->entity_binary_sensor->binary_sensor_list->next = ha_device->entity_binary_sensor->binary_sensor_list;
#endif
    //初始化Sensor 实体
#if CONFIG_ENTITY_ENABLE_SENSOR 
    ha_device->entity_sensor = pvPortMalloc(sizeof(ha_sensor_entity_t));
    ha_device->entity_sensor->entity_type = CONFIG_HA_ENTITY_SENSOR;
    ha_device->entity_sensor->sensor_list = pvPortMalloc(sizeof(ha_sensor_entity_t));
    ha_device->entity_sensor->sensor_list->prev = ha_device->entity_sensor->sensor_list;
    ha_device->entity_sensor->sensor_list->next = ha_device->entity_sensor->sensor_list;
#endif
    //初始化Light 实体
#if CONFIG_ENTITY_ENABLE_LIGHT
    ha_device->entity_light = pvPortMalloc(sizeof(ha_lhlist_t));
    ha_device->entity_light->entity_type = CONFIG_HA_ENTITY_LIGHT;
    ha_device->entity_light->light_list = pvPortMalloc(sizeof(ha_lh_entity_t));
    ha_device->entity_light->light_list->prev = ha_device->entity_light->light_list;
    ha_device->entity_light->light_list->next = ha_device->entity_light->light_list;
#endif
    //初始化开关实体双向连接表
#if CONFIG_ENTITY_ENABLE_SWITCH
    ha_device->entity_switch = pvPortMalloc(sizeof(ha_swlist_t));
    ha_device->entity_switch->entity_type = CONFIG_HA_ENTITY_SWITCH;
    ha_device->entity_switch->switch_list = pvPortMalloc(sizeof(ha_sw_entity_t));
    ha_device->entity_switch->switch_list->prev = ha_device->entity_switch->switch_list;
    ha_device->entity_switch->switch_list->next = ha_device->entity_switch->switch_list;
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    ha_device->entity_text = pvPortMalloc(sizeof(ha_text_list_t));
    ha_device->entity_text->entity_type = CONFIG_HA_ENTITY_TEXT;
    ha_device->entity_text->text_list = pvPortMalloc(sizeof(ha_text_entity_t));
    ha_device->entity_text->text_list->prev = ha_device->entity_text->text_list;
    ha_device->entity_text->text_list->next = ha_device->entity_text->text_list;
#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    ha_device->entity_climateHVAC = pvPortMalloc(sizeof(ha_climateHVAC_list_t));
    ha_device->entity_climateHVAC->entity_type = CONFIG_HA_ENTITY_CLIMATE_HVAC;
    ha_device->entity_climateHVAC->climateHVAC_list = pvPortMalloc(sizeof(ha_climateHVAC_t));
    ha_device->entity_climateHVAC->climateHVAC_list->prev = ha_device->entity_climateHVAC->climateHVAC_list;
    ha_device->entity_climateHVAC->climateHVAC_list->next = ha_device->entity_climateHVAC->climateHVAC_list;
#endif

    ha_device->event_cb = event_cb;
    homeAssistant_mqtt_init(ha_device);
    vPortFree(buff);
}

void homeAssistant_device_start(void)
{
    if (ha_device==NULL) {
        HA_LOG_E("param is NULL\r\n");
        return;
    }
    homeAssistant_mqtt_port_start();
}

void homeAssisatnt_device_stop(void)
{
    if (ha_device==NULL) {
        HA_LOG_E("param is NULL\r\n");
        return;
    }
    homeAssistant_device_send_status(false);
    vTaskDelay(pdMS_TO_TICKS(100));
    homeAssistant_mqtt_port_stop();
}

void homeAssistant_device_send_status(bool status)
{
    if (ha_device->mqtt_info.mqtt_connect_status) {
        if (ha_device->availability_topic==NULL) goto   __c_topic;
        else goto   send_status;
    __c_topic:
        ha_device->availability_topic = pvPortMalloc(128);
        memset(ha_device->availability_topic, 0, 128);
        sprintf(ha_device->availability_topic, "%s/%02x%02x%02x%02x%02x%02x/status", CONFIG_HA_AUTOMATIC_DISCOVERY, STA_MAC[0], STA_MAC[1], STA_MAC[2], STA_MAC[3], STA_MAC[4], STA_MAC[5]);
    send_status:
        if (ha_device->payload_available==NULL)ha_device->payload_available = "online";
        if (ha_device->payload_not_available==NULL)ha_device->payload_not_available = "offline";
        homeAssistant_mqtt_port_public(ha_device->availability_topic, status?ha_device->payload_available:ha_device->payload_not_available, 0, 1);
    }
    else {
        HA_LOG_E("MQTT server is disconnect\r\n");
    }
}

void homeAssistant_device_add_entity(char* entity_type, void* ha_entity_list)
{
    //添加开关实体
#if CONFIG_ENTITY_ENABLE_SWITCH
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SWITCH)) {
        //添加 switch 节点
        ha_sw_entity_t* switch_node = (ha_sw_entity_t*)ha_entity_list;
        entity_swith_add_node(switch_node);
    }
#endif
    //添加light 实体
#if CONFIG_ENTITY_ENABLE_LIGHT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_LIGHT)) {
        HA_LOG_I("HomeAssistant add light entity\r\n");
        ha_lh_entity_t* light_node = (ha_lh_entity_t*)ha_entity_list;
        entity_light_add_node(light_node);
    }
#endif
    //添加传感器
#if CONFIG_ENTITY_ENABLE_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SENSOR)) {
        HA_LOG_I("HomeAssistant add sensor entity\r\n");
        ha_sensor_entity_t* sensor_node = (ha_sensor_entity_t*)ha_entity_list;
        entity_sensor_add_node(sensor_node);
    }
#endif
    //添加二进制传感器实体
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_BINARY_SENSOR)) {
        HA_LOG_I("HomeAssistant add binary sensor entity\r\n");
        ha_Bsensor_entity_t* binary_sensor_node = (ha_Bsensor_entity_t*)ha_entity_list;
        entity_binary_sensor_add_node(binary_sensor_node);
    }
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_TEXT)) {
        HA_LOG_I("HomeAssistant add text entity\r\n");
        ha_text_entity_t* text_node = (ha_text_entity_t*)ha_entity_list;
        entity_text_add_node(text_node);
    }
#endif
#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_CLIMATE_HVAC)) {
        HA_LOG_I("HomeAssistant add text entity\r\n");
        ha_climateHVAC_t* climateHVAC_node = (ha_climateHVAC_t*)ha_entity_list;
        entity_climate_HVAC_add_node(climateHVAC_node);
    }
#endif

}

int homeAssistant_device_send_entity_state(char* entity_type, void* ha_entity_list, unsigned short state)
{
    int ret_id = -1;
    if (entity_type==NULL||ha_entity_list==NULL) {
        HA_LOG_E("params error\r\n");
        return ret_id;
    }
#if CONFIG_ENTITY_ENABLE_SWITCH
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SWITCH)) {
        ha_sw_entity_t* switch_node = (ha_sw_entity_t*)ha_entity_list;
        ret_id = homeAssistant_mqtt_port_public(switch_node->state_topic, state?switch_node->payload_on:switch_node->payload_off, 0, 1);
    }
#endif
#if CONFIG_ENTITY_ENABLE_LIGHT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_LIGHT)) {
        ha_lh_entity_t* light_node = (ha_lh_entity_t*)ha_entity_list;
        //上报RGB值
        if (light_node->rgb.rgb_command_topic!=NULL) {
            char* rgb_data = pvPortMalloc(16);
            if (light_node->rgb.rgb_value_template==NULL) {
                memset(rgb_data, 0, 16);
                sprintf(rgb_data, "%d,%d,%d", light_node->rgb.red, light_node->rgb.green, light_node->rgb.blue);
                ret_id = homeAssistant_mqtt_port_public(light_node->rgb.rgb_state_topic, rgb_data, 0, 0);
                if (ret_id<false) HA_LOG_E("publish is fali\r\n");
            }
            vPortFree(rgb_data);
        }
        if (light_node->payload_on!=NULL && light_node->payload_off!=NULL)
            ret_id = homeAssistant_mqtt_port_public(light_node->state_topic, state?light_node->payload_on:light_node->payload_off, 0, 1);
        else {
            ret_id = homeAssistant_mqtt_port_public(light_node->state_topic, state?"ON":"OFF", 0, 0);
        }
    }
#endif
#if CONFIG_ENTITY_ENABLE_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SENSOR)) {
        ha_sensor_entity_t* sensor_node = (ha_sensor_entity_t*)ha_entity_list;
        if (sensor_node->sensor_data==NULL) {
            // HA_LOG_E("sensor_node sensor_data is NULL,data is state=%d", state);
            sensor_node->sensor_data = pvPortMalloc(16);
            memset(sensor_node->sensor_data, 0, 16);
            sprintf(sensor_node->sensor_data, "%d", state);
            ret_id = homeAssistant_mqtt_port_public(sensor_node->state_topic, sensor_node->sensor_data, 0, 1);
            memset(sensor_node->sensor_data, 0, 16);
            vPortFree(sensor_node->sensor_data);
            sensor_node->sensor_data = NULL;
        }
        else
            ret_id = homeAssistant_mqtt_port_public(sensor_node->state_topic, sensor_node->sensor_data, 0, 1);

    }
#endif
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_BINARY_SENSOR)) {
        ha_Bsensor_entity_t* binary_sensor_node = (ha_sensor_entity_t*)ha_entity_list;
        ret_id = homeAssistant_mqtt_port_public(binary_sensor_node->state_topic, state?binary_sensor_node->payload_on:binary_sensor_node->payload_off, 0, 1);
    }
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_TEXT)) {
        ha_text_entity_t* text_node = (ha_text_entity_t*)ha_entity_list;
        if (text_node->text_value!=NULL&& text_node->state_topic!=NULL) {
            ret_id = homeAssistant_mqtt_port_public(text_node->state_topic, text_node->text_value, 0, 1);
        }
        else HA_LOG_E("text value is null\r\n");
    }
#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_CLIMATE_HVAC)) {
        ha_climateHVAC_t* climateHVAC_node = (ha_climateHVAC_t*)ha_entity_list;
        if (state) {
            //返回模式
            if (climateHVAC_node->mode_state_topic!=NULL &&climateHVAC_node->modes[0]==NULL) {
                ret_id = homeAssistant_mqtt_port_public(climateHVAC_node->mode_state_topic, modes_def[climateHVAC_node->modes_type], 0, 1);
            }
            else {
                ret_id = homeAssistant_mqtt_port_public(climateHVAC_node->mode_state_topic, climateHVAC_node->modes[climateHVAC_node->modes_type], 0, 1);
            }
        }
        else {
            if (climateHVAC_node->mode_state_topic!=NULL &&climateHVAC_node->modes[0]==NULL) {
                ret_id = homeAssistant_mqtt_port_public(climateHVAC_node->mode_state_topic, "off", 0, 1);
            }
            else {
                ret_id = homeAssistant_mqtt_port_public(climateHVAC_node->mode_state_topic, "off", 0, 1);
            }
        }

    }
#endif
    return ret_id;
}

void* homeAssistant_fine_entity(char* entity_type, const char* unique_id)
{
    if (entity_type==NULL || unique_id==NULL) {
        HA_LOG_E("parama is NULL\r\n");
        return NULL;
    }
    //查找switch实体
#if CONFIG_ENTITY_ENABLE_SWITCH
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SWITCH)) {
        ha_sw_entity_t* switch_cur = ha_device->entity_switch->switch_list->next;
        while (switch_cur!=ha_device->entity_switch->switch_list) {
            if (!strncmp(switch_cur->unique_id, unique_id, strlen(unique_id))) {
                return switch_cur;
            }
            switch_cur = switch_cur->next;
        }

    }
#endif
    //查找 light 实体
#if CONFIG_ENTITY_ENABLE_LIGHT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_LIGHT)) {
        ha_lh_entity_t* light_cur = ha_device->entity_light->light_list->next;
        while (light_cur!=ha_device->entity_light->light_list) {
            if (!strncmp(light_cur->unique_id, unique_id, strlen(unique_id))) {
                return light_cur;
            }
            light_cur = light_cur->next;
        }

    }
#endif
    //查找 sensor 实体
#if CONFIG_ENTITY_ENABLE_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_SENSOR)) {
        ha_sensor_entity_t* sensor_cur = ha_device->entity_sensor->sensor_list->next;
        while (sensor_cur !=ha_device->entity_sensor->sensor_list) {
            if (!strncmp(sensor_cur->unique_id, unique_id, strlen(unique_id))) {
                return sensor_cur;
            }
            sensor_cur = sensor_cur->next;
        }

    }
#endif
    //查找 binary sensor 实体
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_BINARY_SENSOR)) {
        ha_Bsensor_entity_t* binary_sensor_cur = ha_device->entity_binary_sensor->binary_sensor_list->next;
        while (binary_sensor_cur !=ha_device->entity_binary_sensor->binary_sensor_list) {
            if (!strncmp(binary_sensor_cur->unique_id, unique_id, strlen(unique_id))) {
                return binary_sensor_cur;
            }
            binary_sensor_cur = binary_sensor_cur->next;
        }

    }
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_TEXT)) {
        ha_text_entity_t* text_cur = ha_device->entity_text->text_list->next;
        while (text_cur!=ha_device->entity_text->text_list) {
            if (!strncmp(text_cur->unique_id, unique_id, strlen(unique_id))) return text_cur;
            text_cur = text_cur->next;
        }
    }
#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    if (!strcmp(entity_type, CONFIG_HA_ENTITY_CLIMATE_HVAC)) {
        ha_climateHVAC_t* climateHVAC_cur = ha_device->entity_climateHVAC->climateHVAC_list->next;

        while (climateHVAC_cur!=ha_device->entity_climateHVAC->climateHVAC_list) {
            if (!strncmp(climateHVAC_cur->unique_id, unique_id, strlen(unique_id))) return climateHVAC_cur;
            climateHVAC_cur = climateHVAC_cur->next;
        }
    }
#endif

    HA_LOG_E("There is no %s entity unique id %s\r\n", entity_type, unique_id);
    return NULL;
}
