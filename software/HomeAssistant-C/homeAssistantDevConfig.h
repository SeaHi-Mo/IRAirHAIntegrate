/**
 * @file homeAssistantDevConfig.h
 * @author your name (you@domain.com)
 * @brief  主要用来配置设备信息，实体类型，发现前缀等
 * @version 0.1
 * @date 2024-02-03
 *
 * @copyright Copyright (c) 2024
 *
*/

#ifndef HOMEASSISTANTDEVCONFIG_H
#define HOMEASSISTANTDEVCONFIG_H


/***********  设备信息  **********/
#define CONFIG_HA_DEVICE_NAME "Ai-M61_DEV"                                 //设备名称
#define CONFIG_HA_DEVICE_HW_VERSION "Ai-M61-32S-Kit_01"                    //硬件版本号
#define CONFIG_HA_DEVICE_SW_VERSION "V1.0.0"                               //软件版本号
#define CONFIG_HA_DEVICE_MANUFACTURER "安信可科技"                          //产商名称
#define CONFIG_HA_DEVICE_IDENTIFIERS "aithinker_ai_m61_kit01"              //唯一标识符，序列号
#define CONFIG_HA_DEVICE_CONNECTIONS "connections"                         //连接信息，这里是json的名称例如： "connections": [["mac", "02:5b:26:a8:dc:12"]]
#define CONFIG_HA_DEVICE_MODULE "Ai-M6x"                                   //模组名称
/***********  MQTT 服务器信息****************/
#define CONFIG_HA_MQTT_SERVER_HOST "wx.ai-thinker.com"                     //服务器的URL,支持域名解析，通常：mqtt://192.168.1.10
#define CONFIG_HA_MQTT_SERVER_PORT 1883                                    //MQTT 端口号
#define CONFIG_HA_MQTT_CLIENT_DEF_ID  CONFIG_HA_DEVICE_HW_VERSION          //MQTT 默认客户端ID
#define CONFIG_HA_MQTT_CLIENT_DEF_USERNAME   CONFIG_HA_DEVICE_NAME         //MQTT 默认用户名
#define CONFIG_HA_MQTT_CLIENT_DEF_PASSWORLD  "12345678"                    //MQTT 默认密码                                    
#define CONFIG_HA_MQTT_CLIENT_DEF_KEEPALIVE  60                            //MQTT 默认心跳

/********** HomeAssistant 配置 *************/
#define CONFIG_HA_AUTOMATIC_DISCOVERY   "homeassistant"                    //自动发现的前缀
#define CONFIG_HA_STATUS_TOPIC "homeassistant/Ai/status"                      //HomeAssistant 上线topic
#define CONFIG_HA_STATUS_MESSAGE_ON "online"                               //homeAssistant 上线时发送的内容
#define CONFIG_HA_STATUS_MESSAGE_OFF "offline"                             //homeAssistant 掉线时发送的内容

#define CONFIG_HA_ENTITY_ALARM_CONTROL_PANEL "alarm_control_panel"         //报警控制面板 实体
#define CONFIG_HA_ENTITY_BINARY_SENSOR       "binary_sensor"               //高低电平传感器 实体
#define CONFIG_HA_ENTITY_BUTTON              "button"                      //按钮实体
#define CONFIG_HA_ENTITY_CAMERA              "camera"                      //摄像头实体
#define CONFIG_HA_ENTITY_COVER               "cover"                       //门类实体，窗帘、车门等
#define CONFIG_HA_ENTITY_DEVICE_TRACKER      "device_tracker"              //跟踪器实体，GPS定位等 
#define CONFIG_HA_ENTITY_DEVICE_TRIGGER      "device_automation"           //触发器实体
#define CONFIG_HA_ENTITY_EVENT               "event"                       //时间实体
#define CONFIG_HA_ENTITY_FAN                 "fan"                         //风扇实体
#define CONFIG_HA_ENTITY_HUMIDIFIER          "humidifier"                  //加湿器实体
#define CONFIG_HA_ENTITY_IMAGE               "image"                       //图片实体
#define CONFIG_HA_ENTITY_CLIMATE_HVAC        "climate"                      //空调实体
#define CONFIG_HA_ENTITY_LAWN_MOWER          "lawn_mower"                  //割草机实体
#define CONFIG_HA_ENTITY_LIGHT               "light"                       //灯实体
#define CONFIG_HA_ENTITY_LOCK                "lock"                        //门锁实体
#define CONFIG_HA_ENTITY_NUMBER              "number"                      //数字实体
#define CONFIG_HA_ENTITY_SCENE               "scene"                       //场景实体
#define CONFIG_HA_ENTITY_SELECT              "select"                      //选择器实体
#define CONFIG_HA_ENTITY_SENSOR              "sensor"                      //传感器实体
#define CONFIG_HA_ENTITY_SIREN               "siren"                       //警报器实体
#define CONFIG_HA_ENTITY_SWITCH              "switch"                      //开关实体
#define CONFIG_HA_ENTITY_UPDATE              "update"                      //更新实体              
#define CONFIG_HA_ENTITY_TAG_SCANNER         "tag_scanner"                 //标签扫描仪实体
#define CONFIG_HA_ENTITY_TEXT                "text"                        //文本实体
#define CONFIG_HA_ENTITY_VACUUM              "vacuum"                      //真空吸尘器实体
#define CONFIG_HA_ENTITY_VALVE               "valve"                       //阀门实体
#define CONFIG_HA_ENTITY_WATER_HEATER        "water_heater"                //热水器实体

/**********************  需要开启的实体 *************************/
//报警控制面板 实体 默认不开启，需要使用就置 1
#define CONFIG_ENTITY_ENABLE_ALARM_CONTROL_PANEL 0    

//高低电平传感器 实体     默认不开启，需要使用就置 1
#define CONFIG_ENTITY_ENABLE_BINARY_SENSOR 0  

//按钮实体     默认不开启，需要使用就置 1         
#define CONFIG_ENTITY_ENABLE_BUTTON 0   

//摄像头实体   默认不开启，需要使用就置 1               
#define CONFIG_ENTITY_ENABLE_CAMERA 0  

//门类实体，窗帘、车门等   默认不开启，需要使用就置 1                  
#define CONFIG_ENTITY_ENABLE_COVER 0     

//跟踪器实体，GPS定位等  默认不开启，需要使用就置 1                  
#define CONFIG_ENTITY_ENABLE_DEVICE_TRACKER 0     

//触发器实体    默认不开启，需要使用就置 1    
#define CONFIG_ENTITY_ENABLE_DEVICE_TRIGGER 0  

//时间实体    默认不开启，需要使用就置 1      
#define CONFIG_ENTITY_ENABLE_EVENT 0     

//风扇实体    默认不开启，需要使用就置 1               
#define CONFIG_ENTITY_ENABLE_FAN  0  

//加湿器实体  默认不开启，需要使用就置 1                   
#define CONFIG__ENTITY_ENABLE_HUMIDIFIER  0   

//图片实体      默认不开启，需要使用就置 1        
#define CONFIG_ENTITY_ENABLE_IMAGE 0   

//空调实体  默认不开启，需要使用就置 1                  
#define CONFIG_ENTITY_ENABLE_CLIMATE_HVAC 1    

//割草机实体   默认不开启，需要使用就置 1          
#define CONFIG_ENTITY_ENABLE_LAWN_MOWER 0     

//灯实体  默认不开启，需要使用就置 1           
#define CONFIG_ENTITY_ENABLE_LIGHT 0       

//门锁实体   默认不开启，需要使用就置 1           
#define CONFIG_ENTITY_ENABLE_LOCK 0         

//数字实体   默认不开启，需要使用就置 1          
#define CONFIG_ENTITY_ENABLE_NUMBER 0   

 //场景实体   默认不开启，需要使用就置 1                 
#define CONFIG_ENTITY_ENABLE_SCENE 0  

 //选择器实体  默认不开启，需要使用就置 1                 
#define CONFIG_ENTITY_ENABLE_SELECT 1      

//传感器实体   默认不开启，需要使用就置 1            
#define CONFIG_ENTITY_ENABLE_SENSOR 1    

//警报器实体  默认不开启，需要使用就置 1                
#define CONFIG_ENTITY_ENABLE_SIREN 0     

//开关实体   默认开启        
#define CONFIG_ENTITY_ENABLE_SWITCH 0

//更新实体   默认不开启，需要使用就置 1                   
#define CONFIG_ENTITY_ENABLE_UPDATE  0     

 //标签扫描仪实体   默认不开启，需要使用就置 1                      
#define CONFIG_ENTITY_ENABLE_TAG_SCANNER 0 

 //文本实体  默认不开启，需要使用就置 1            
#define CONFIG_ENTITY_ENABLE_TEXT 0     

//真空吸尘器实体   默认不开启，需要使用就置 1              
#define CONFIG_ENTITY_ENABLE_VACUUM  0      

 //阀门实体  默认不开启，需要使用就置 1             
#define CONFIG_ENTITY_ENABLE_VALVE 0        

//热水器实体  默认不开启，需要使用就置 1            
#define CONFIG_ENTITY_ENABLE_WATER_HEATER 0             

#endif
