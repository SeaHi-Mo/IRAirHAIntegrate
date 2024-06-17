/**
 * @file homeAssistantPort.h
 * @author Seahi (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-06
 *
 * @copyright Copyright (c) 2024
 *
*/

#ifndef HOMEASSISTANTPORT_H
#define HOMEASSISTANTPORT_H
#include "homeAssistantMQTT.h"
#include "homeAssistantDevConfig.h"

// #define CONFIG_Ai_M6x
#define CONFIG_Ai_WB2

#define HA_LOG_ENABLE

#ifdef HA_LOG_ENABLE
#define DBG_TAG "HomeAssistant"
#define HA_LOG_D(...) printf("\033[0m[D][" DBG_TAG "] " __VA_ARGS__ )
#define HA_LOG_E(...) printf("\033[0;31m[E][" DBG_TAG "] " __VA_ARGS__ )
#define HA_LOG_I(...) printf("\033[0;32m[I][" DBG_TAG "] " __VA_ARGS__ )
#define HA_LOG_W(...) printf("\033[0;33m[W][" DBG_TAG "] " __VA_ARGS__ )
#define HA_LOG_F(...) printf("\033[0;35m[F][" DBG_TAG "] " __VA_ARGS__ )
#endif
void homeAssistant_get_sta_mac(char* mac);
int homeAssistant_mqtt_port_init(homeAssisatnt_device_t* ha_dev);
int homeAssistant_mqtt_port_start(void);
int homeAssistant_mqtt_port_stop(void);
int homeAssistant_mqtt_port_public(const char* topic, const char* payload, int qos, bool retain);
int homeAssistant_mqtt_port_subscribe(const char* topic, int qos);
#endif