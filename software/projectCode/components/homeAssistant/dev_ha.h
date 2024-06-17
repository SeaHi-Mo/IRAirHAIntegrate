/**
 * @file dev_ha.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef DEV_HA_H
#define DEV_HA_H

#include "device_state.h"
#include "homeAssistantPort.h"

#define MQTT_SERVER_DEFAULT_HOST ""
#define MQTT_SERVER_DEFAULT_PORT 1883

void device_homeAssistant_init(homeAssisatnt_device_t* dev_ha);

#endif