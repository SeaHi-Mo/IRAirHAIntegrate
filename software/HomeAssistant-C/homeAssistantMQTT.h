/**
 * @file homeAssisatntMQTT.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-02-03
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef HOMEASSISTANTMQTT_H
#define HOMEASSISTANTMQTT_H

#include "homeAssistantDevConfig.h"
#include "stdbool.h"
#define HOMEASSISTANT_STATUS_ONLINE 1
#define HOMEASSISTANT_STATUS_OFFLINE 0

#define HOMEASSISTANT_CONFIG_DATA_SIZE 1024


//HomeAssistant 的MQTT 事件 只要部分事件，更多事件还在更新当中
typedef enum {
    HA_EVENT_NONE = 0,
    HA_EVENT_MQTT_CONNECED, //服务器连接成功事件
    HA_EVENT_MQTT_DISCONNECT,//服务器断开事件
    HA_EVENT_HOMEASSISTANT_STATUS_ONLINE, //HomeAssisstant 在线事件
    HA_EVENT_HOMEASSISTANT_STATUS_OFFLINE, //HomeAssistant 掉线事件
    HA_EVENT_MQTT_COMMAND_SWITCH,//服务器下发开关命令事件，当在HA操作开关时，会触发这个事件
    HA_EVENT_MQTT_COMMAND_LIGHT_SWITCH,//light 灯的开关事件
    HA_EVENT_MQTT_COMMAND_LIGHT_RGB_UPDATE,//light 灯的RGB 颜色下发事件
    HA_EVENT_MQTT_COMMAND_LIGHT_BRIGHTNESS,//light 灯的亮度数据下发事件
    HA_EVENT_MQTT_COMMAND_TEXT_VALUE,  //服务器下发text内容事件

    HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_POWER, //服务器下发的空调开关事件
    HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_MODES, //设置模式
    HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_TEMP,  //设置温度
    HA_EVENT_MQTT_COMMAND_CLIMATE_HVAC_FAN_MODES,//设置风力  
    HA_EVENT_MQTT_ERROR,
}ha_event_t;

/**
 * @brief 连接信息 ，这个没啥用，用不到
 *
*/
typedef struct homeAssisatnt_netinfo {
    char* ssid; //路由器名称，不支持中文和5GHz
    char* password;//路由器密码
    char* bssid;//路由器bssid
    char* ipv4_addr;//获取到的IP地址
}homeAssisatnt_netinfo_t;
/**
 * @brief 设备遗嘱，用于掉线之后发送下线信息
 *
*/
typedef struct homeAssisatnt_device_will
{
    char* will_topic; //遗嘱需要发送的Topic
    char* will_msg;  //需要发送的消息，一般: offline
    char will_msg_len;//消息的长度
    char will_qos;//服务质量
    bool will_retain;//是否保留
}homeAssisatnt_device_will_t;
/**
 * @brief MQTT 连接信息
 *
*/
typedef struct homeAssisatnt_mqtt_info {
    bool mqtt_connect_status; //MQTT 服务器连接状态，true 为已连接。false 为未连接
    char* mqtt_host;      //MQTT 服务器地址，支持域名解析
    unsigned short port;           // MQTT 服务器端口
    char* mqtt_clientID;  //MQTT 的客户端ID
    char* mqtt_username;  //MQTT 接入的用户名
    char* mqtt_password;  //MQTT 接入密码
    int mqtt_keeplive;      //保活时间
    homeAssisatnt_device_will_t will;//遗嘱内容
}ha_mqtt_info_t;
/**
 * @brief switch实体信息
 * @brief 成员含义可以参考：https://www.home-assistant.io/integrations/switch.mqtt/
*/
#if CONFIG_ENTITY_ENABLE_SWITCH

typedef struct homeAssisatnt_entity_switch {
    char* name;                   //实体名称 必须要赋值
    char* entity_config_topic;    //实体自动发现需要的topic，已经自动赋值，可以不配置
    char* object_id;              //实体 工程id 可以为NULL
    char* availability_mode;      //实体上下线的模式 可以为NULL
    char* availability_template;  //实体上下线的数据格式，建议为NULL，采用默认
    char* availability_topic;     //实体上下线上报的Topic,建议保持默认
    char* command_topic;          //命令接收的Topic,需要订阅
    char* state_topic;            //上报给HA的数据的Topic
    char* device_class;           //设备类型，可以留空
    bool enabled_by_default;         //默认LED的状态
    char* encoding;               //编码方式
    char* entity_category;        //实体属性，保持NULL
    char* icon;                    //图标
    char* json_attributes_template;//json 数据模板
    char* optimistic;              //记忆模式
    char* payload_available;       //在线消息内容 默认"online"
    char* payload_not_available;   //离线消息内容 默认"offline"
    char* payload_off;             //开关状态内容，默认"ON"
    char* payload_on;               //开关状态内容，默认"OFF"
    int qos;                      //消息服务质量
    bool retain;                       //是否保留该信息     
    char* state_off;               //状态 关          
    char* state_on;                  //状态 开
    char* unique_id;                // 唯一的识别码，这个必须配置 
    char* value_template;           //数据格式 
    char* config_data;      //开关的自动发现的json数据
    bool switch_state;                 //当前开关状态 
    struct homeAssisatnt_entity_switch* prev;
    struct homeAssisatnt_entity_switch* next;
}ha_sw_entity_t;

typedef struct {
    char* entity_type;
    ha_sw_entity_t* switch_list;
    ha_sw_entity_t* command_switch;
}ha_swlist_t;
#endif

/**
 * @brief ligth实体信息
 * @brief 成员含义可以参考：https://www.home-assistant.io/integrations/switch.mqtt/
*/
#if CONFIG_ENTITY_ENABLE_LIGHT
struct light_brightness_t {
    char* brightness_command_topic;
    char* brightness_command_template;
    char* brightness_scale;
    char* brightness_state_topic;
    char* brightness_value_template;
    char brightness;
};

struct light_color_temp_t {
    char* color_mode_state_topic;
    char* color_mode_value_template;
    char* color_temp_command_template;
    char* color_temp_command_topic;
    char* color_temp_state_topic;
    char* color_temp_value_template;
};

struct light_effect_t {
    char* effect_command_topic;
    char* effect_command_template;
    char* effect_list;
    char* effect_state_topic;
    char* effect_value_template;
};

struct light_hs_t {
    char* hs_command_template;
    char* hs_command_topic;
    char* hs_state_topic;
    char* hs_value_template;
};

struct light_rgb_t {
    char* rgb_command_template;
    char* rgb_command_topic;
    char* rgb_state_topic;
    char* rgb_value_template;
    char red;
    char green;
    char blue;
};

struct light_rgbw_t {
    char* rgbw_command_template;
    char* rgbw_command_topic;
    char* rgbw_state_topic;
    char* rgbw_value_template;
};

struct light_rgbww_t {
    char* rgbww_command_template;
    char* rgbww_command_topic;
    char* rgbww_state_topic;
    char* rgbww_value_template;
};

struct light_white_t {
    char* white_command_topic;
    char* white_scale;
};

struct light_xy_t {

    char* xy_command_template;
    char* xy_command_topic;
    char* xy_state_topic;
    char* xy_value_template;
};

typedef  struct homeAssisatnt_entity_light {
    char* name;
    char* entity_config_topic;
    char* object_id;
    char* availability_mode;
    char* availability_template;
    char* availability_topic;
    char* command_topic;
    char* state_topic;
    char* device_class;
    bool enabled_by_default;
    char* encoding;
    char* entity_category;
    char* icon;
    char* json_attributes_template;
    char* optimistic;
    char* payload_available;
    char* payload_not_available;
    char* payload_off;
    char* payload_on;
    int qos;
    char* config_data;
    struct light_brightness_t brightness;
    struct light_color_temp_t color_temp;
    struct light_effect_t effect;
    struct light_hs_t hs;
    struct light_rgb_t rgb;
    struct light_rgbw_t rgbw;
    struct light_rgbww_t rbgww;
    struct light_white_t white;
    struct light_xy_t xy;
    bool retain;
    char* state_off;
    char* state_on;
    char* unique_id;
    char* value_template;
    bool light_state;
    struct homeAssisatnt_entity_light* prev;
    struct homeAssisatnt_entity_light* next;
}ha_lh_entity_t;

typedef struct {
    char* entity_type;
    ha_lh_entity_t* light_list;
    ha_lh_entity_t* command_light;
}ha_lhlist_t;
#endif

/**
 * @brief 传感器实体
 *
*/
#if CONFIG_ENTITY_ENABLE_SENSOR
typedef enum {
    Class_None = 0,
    Class_apparent_power,
    Class_aqi,
    Class_atmospheric_pressure,
    Class_battery,
    Class_carbon_dioxide,
    Class_carbon_monoxide,
    Class_current,
    Class_data_rate,
    Class_data_size,
    Class_date,
    Class_distance,
    Class_duration,
    Class_energy,
    Class_energy_storage,
    Class_ha_enum,
    Class_frequency,
    Class_gas,
    Class_humidity,
    Class_illuminance,
    Class_irradiance,
    Class_moisture,
    Class_monetary,
    Class_nitrogen_dioxide,
    Class_nitrogen_monoxide,
    Class_nitrous_oxide,
    Class_ozone,
    Class_ph,
    Class_pm1,
    Class_pm25,
    Class_pm10,
    Class_power_factor,
    Class_power,
    Class_precipitation,
    Class_precipitation_intensity,
    Class_pressure,
    Class_reactive_power,
    Class_signal_strength,
    Class_sound_pressure,
    Class_speed,
    Class_sulphur_dioxide,
    Class_temperature,
    Class_timestamp,
    Class_volatile_organic_compounds,
    Class_volatile_organic_compounds_parts,
    Class_voltage,
    Class_volume,
    Class_volume_flow_rate,
    Class_volume_storage,
    Class_water,
    Class_weight,
    Class_wind_speed
}ha_sensor_class_t;

typedef  struct homeAssisatnt_entity_sensor {
    char* name;
    char* entity_config_topic;
    char* config_data;
    char* object_id;
    char* unique_id;
    char* availability_mode;
    char* availability_template;
    char* availability_topic;
    ha_sensor_class_t device_class;
    char* payload_available;
    char* payload_not_available;
    unsigned short suggested_display_precision;
    bool enabled_by_default;
    char* entity_category;
    char* icon;
    char* json_attributes_template;
    char* json_attributes_topic;
    char* last_reset_value_template;
    int qos;
    bool retain;
    char* state_class;
    char* state_topic;
    char* unit_of_measurement;
    char* value_template;
    unsigned short expire_after;
    bool force_update;
    void* sensor_data;

    struct homeAssisatnt_entity_sensor* prev;
    struct homeAssisatnt_entity_sensor* next;
}ha_sensor_entity_t;

typedef struct {
    char* entity_type;
    ha_sensor_entity_t* sensor_list;
}ha_sensorlist_t;

#endif
/**
 * @brief 二进制传感器实体
 *
*/
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR
typedef enum {
    Bclass_None = 0,
    Bclass_battery,
    Bclass_battery_charging,
    Bclass_carbon_monoxide,
    Bclass_cold,
    Bclass_connectivity,
    Bclass_door,
    Bclass_garage_door,
    Bclass_gas,
    Bclass_heat,
    Bclass_light,
    Bclass_lock,
    Bclass_moisture,
    Bclass_motion,
    Bclass_moving,
    Bclass_occupancy,
    Bclass_opening,
    Bclass_plug,
    Bclass_power,
    Bclass_presence,
    Bclass_problem,
    Bclass_running,
    Bclass_afety,
    Bclass_smoke,
    Bclass_sound,
    Bclass_tamper,
    Bclass_update,
    Bclass_vibration,
    Bclass_window,
}ha_Bsensor_class_t;

typedef  struct homeAssisatnt_entity_binary_sensor {
    char* name;
    char* entity_config_topic;
    char* config_data;
    char* object_id;
    char* unique_id;
    char* availability_mode;
    char* availability_template;
    char* availability_topic;
    char* payload_available;
    char* payload_not_available;
    ha_Bsensor_class_t device_class;
    char* entity_category;
    char* icon;
    char* json_attributes_template;
    char* json_attributes_topic;
    int qos;
    bool retain;
    char* state_class;
    char* state_topic;
    char* value_template;
    unsigned short expire_after;
    bool force_update;
    bool enabled_by_default;
    bool state;
    char* payload_off;
    char* payload_on;
    struct homeAssisatnt_entity_binary_sensor* prev;
    struct homeAssisatnt_entity_binary_sensor* next;
}ha_Bsensor_entity_t;

typedef struct {
    char* entity_type;
    ha_Bsensor_entity_t* binary_sensor_list;
}ha_binary_sensorlist_t;
#endif
/**
 * @brief Text 文本实体
 *
*/
#if CONFIG_ENTITY_ENABLE_TEXT
typedef  struct homeAssisatnt_entity_text {
    char* name;
    char* entity_config_topic;
    char* config_data;
    char* object_id;
    char* unique_id;

    char* availability_mode;
    char* availability_template;
    char* availability_topic;

    char* payload_available;
    char* payload_not_available;
    bool enabled_by_default;
    char* encoding;

    char* entity_category;
    char* icon;
    char* json_attributes_template;
    char* json_attributes_topic;
    int max;  //默认255
    int min;  //默认 0
    char* mode; //关闭文本实体的模式
    char* pattern;//要设置或接收的文本必须与有效的正则表达式匹配。
    char* command_template;//接收文本的格式
    char* command_topic;//接收文本的主题

    int qos;
    bool retain;
    char* state_topic;//返回发布文本的主题
    char* value_template;//发布文本的格式

    char* text_value;   //当前文本内容
    struct homeAssisatnt_entity_text* prev;
    struct homeAssisatnt_entity_text* next;
}ha_text_entity_t;

typedef struct {
    char* entity_type;
    ha_text_entity_t* text_list;
    ha_text_entity_t* command_text;
}ha_text_list_t;
#endif

#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC

typedef enum {
    AC_MODES_AUTO = 0,
    // AC_MODES_OFF,
    AC_MODES_COOL,
    AC_MODES_HEAT,
    AC_MODES_DRY,
    AC_MODES_FAN_ONLY,
    AC_MODES_OTHER,

}ac_modes_t;

typedef enum {
    AC_FAN_MODES_AUTO = 0,
    // AC_MODES_OFF,
    AC_FAN_MODES_LOW,
    AC_FAN_MODES_MEDIUM,
    AC_FAN_MODES_HIGH,
}ac_fan_modes_trype_t;

typedef  struct homeAssisatnt_entity_climateHVAC {
    char* action_template;
    char* action_topic;
    char* availability_mode;
    char* availability_template;
    char* availability_topic;
    //监听当前的湿度
    char* current_humidity_template;
    char* current_humidity_topic;
    //监听当前的温度
    char* current_temperature_template;
    char* current_temperature_topic;

    bool enabled_by_default;
    char* encoding;
    char* entity_category;
    //空调风力控制
    char* fan_mode_command_template;
    char* fan_mode_command_topic;
    char* fan_mode_state_template;
    char* fan_mode_state_topic;
    char* fan_modes[4];
    ac_fan_modes_trype_t fan_modes_type;
    //初始化温度设定
    float initial;  //设置初始目标温度。默认值取决于温度单位，为 21° 或 69.8°F。
    char* icon;

    char* json_attributes_template;
    char* json_attributes_topic;

    float max_humidity; //可设置的最大湿度值
    float max_temp; //可设置的最大温度值
    float min_humidity;//可设置的最小湿度值
    float min_temp; //可设置的最小温度值

    char* mode_command_template;
    char* mode_command_topic;
    char* mode_state_template;
    char* mode_state_topic;
    char* modes[6];
    ac_modes_t modes_type;

    char* name;
    char* object_id;
    bool optimistic;
    char* payload_available;
    char* payload_not_available;
    char* payload_off;
    char* payload_on;

    char* power_command_template;
    char* power_command_topic;
    bool power_state;

    float precision;
    char* preset_mode_command_template;
    char* preset_mode_command_topic;
    char* preset_mode_state_topic;
    char* preset_mode_value_template;
    char* preset_modes[7];

    int qos;
    bool retain;
    char* swing_mode_command_template;
    char* swing_mode_command_topic;
    char* swing_mode_state_template;
    char* swing_mode_state_topic;
    char* swing_modes[10];

    char* target_humidity_command_template;
    char* target_humidity_command_topic;
    char* target_humidity_state_topic;
    char* target_humidity_state_template;

    char* temperature_command_template;
    char* temperature_command_topic;
    char* temperature_high_command_template;
    char* temperature_high_command_topic;
    char* temperature_high_state_template;
    char* temperature_high_state_topic;
    char* temperature_low_command_template;
    char* temperature_low_command_topic;
    char* temperature_low_state_template;
    char* temperature_low_state_topic;
    char* temperature_state_template;
    char* temperature_state_topic;
    char* temperature_unit;
    float temperature_value;
    float temp_step;
    char* unique_id;
    char* value_template;
    char* entity_config_topic;
    char* config_data;

    struct homeAssisatnt_entity_climateHVAC* prev;
    struct homeAssisatnt_entity_climateHVAC* next;
}ha_climateHVAC_t;

typedef struct {
    char* entity_type;
    ha_climateHVAC_t* climateHVAC_list;
    ha_climateHVAC_t* command_climateHVAC;
}ha_climateHVAC_list_t;

#endif

#if CONFIG_ENTITY_ENABLE_SELECT

typedef  struct homeAssisatnt_entity_select {
    char* availability_topic;
    char* availability_mode;
    char* availability_template;
    char* command_template;//接收文本的格式
    char* command_topic;//接收文本的主题
    bool enabled_by_default;
    char* encoding;
    char* entity_category;
    char* icon;

    char* json_attributes_template;
    char* json_attributes_topic;
    char* name;
    char* object_id;
    bool optimistic;

    char** options;
    int options_numble;
    int qos;
    bool retain;

    char* state_topic;
    char* unique_id;
    char* value_template;
    char* options_value;

    char* entity_config_topic;
    char* config_data;
    struct homeAssisatnt_entity_select* prev;
    struct homeAssisatnt_entity_select* next;
}ha_select_t;

typedef struct {
    char* entity_type;
    ha_select_t* select_list;
    ha_select_t* command_select;
}ha_select_list_t;
#endif
/**
 * @brief  设备信息
 *
*/
typedef struct homeAssisatnt_device {
    char* name;
    char* hw_version;
    char* sw_version;
    char* identifiers;
    char* manufacturer;
    char* model;
    char* via_device;
    char* suggest_area;
    char* availability_topic;
    char* payload_available;
    char* payload_not_available;
    homeAssisatnt_netinfo_t wifi_info;
#if CONFIG_ENTITY_ENABLE_SWITCH
    ha_swlist_t* entity_switch;
#endif

#if CONFIG_ENTITY_ENABLE_LIGHT
    ha_lhlist_t* entity_light;
#endif

#if CONFIG_ENTITY_ENABLE_SENSOR 
    ha_sensorlist_t* entity_sensor;
#endif
#if CONFIG_ENTITY_ENABLE_BINARY_SENSOR 
    ha_binary_sensorlist_t* entity_binary_sensor;
#endif

#if CONFIG_ENTITY_ENABLE_TEXT
    ha_text_list_t* entity_text;
#endif


#if CONFIG_ENTITY_ENABLE_CLIMATE_HVAC
    ha_climateHVAC_list_t* entity_climateHVAC;
#endif
#if CONFIG_ENTITY_ENABLE_SELECT
    ha_select_list_t* entity_select;
#endif
    ha_mqtt_info_t mqtt_info;
    bool homeassistant_online;
    void (*event_cb)(ha_event_t event, struct homeAssisatnt_device* ha_dev);
}homeAssisatnt_device_t;


/**
 * @brief HomeAssistant 库初始化函数
 *
 * @param ha_dev HomeAssistant 设备句柄
 * @param event_cb HomeAssistant 事件回调函数
*/
void homeAssistant_device_init(homeAssisatnt_device_t* ha_dev, void(*event_cb)(ha_event_t, homeAssisatnt_device_t*));
/**
 * @brief HomeAssistant 启动连接
 *
*/
void homeAssistant_device_start(void);
/**
 * @brief HomeAssistant 断开连接
 *
*/
void homeAssisatnt_device_stop(void);
/**
 * @brief 添加实体
 *
 * @param entity_type 实体名称
 * @param ha_entity_list 实体描述
*/
void homeAssistant_device_add_entity(char* entity_type, void* ha_entity_list);
/**
 * @brief 发送设备状态
 *
 * @param status 1：发送“online” 0：发送“offline”
*/
void homeAssistant_device_send_status(bool status);

/**
 * @brief homeAssistant_device_send_entity_state
 *       发送实体状态
 * @param entity_type 实体类型
 * @param ha_entity_list 相应实体结构体
 * @param state 状态
 * @return int 成功返回消息ID，失败返回-1
*/
int homeAssistant_device_send_entity_state(char* entity_type, void* ha_entity_list, unsigned short state);
/**
 * @brief homeAssisatant_fine_entity
 *          通过unique id 查找实体，这个功能必须自定义unique_id 如果使用随机的unique_id 则会无法查找
 * @param entity_type 实体类型
 * @param unique_id 实体的 unique id
 * @return void* 返回的实体指针
*/
void* homeAssistant_fine_entity(char* entity_type, const char* unique_id);

#endif

