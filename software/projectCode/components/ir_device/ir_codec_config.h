/**
 * @file ir_code_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef IR_CODEC_CONFIG_H
#define IR_CODEC_CONFIG_H

typedef signed char ir_int8_t;
typedef unsigned char ir_uint8_t;
typedef signed short int ir_int16_t;
typedef unsigned short int ir_uint16_t;
typedef signed int ir_int32_t;
typedef unsigned int ir_uint32_t;

typedef struct ir_darta_config {
    ir_uint16_t guided_code_low_time;    //引导码的低电平时间
    ir_uint16_t guided_code_high_time;   //引导码的高电平时间
    ir_uint16_t data_1_code_low_time;    //数据码"1"的低电平的时间
    ir_uint16_t data_1_code_high_time;   //数据码 “1” 的高电平时间
    ir_uint16_t data_0_code_low_time;    //数据码"0"的低电平的时间
    ir_uint16_t data_0_code_high_time;   //数据码 “0” 的高电平时间
    ir_uint8_t data_code_numble;         //数据码的长度

    ir_uint16_t apart_code_low_time;     //分隔码 低电平时间
    ir_uint16_t apart_code_high_time;    //分隔码 高电平时间
    ir_uint8_t apart_code_numble;        //分隔码数量
}ir_codec_cfg_t;

typedef struct dev_cmd_list {
    char* data;       //设备需要发送的数据
    ir_int16_t cmd_date_len; //数据长度
}dev_cmd_t;

typedef enum {
    AC_FAN_MODE_NONE = 0,
    AC_FAN_MODE_AUTO,
    AC_FAN_MODE_LOW,
    AC_FAN_MODE_MEDIUNM,
    AC_FAN_MODE_HIGH,
    AC_FAN_MODE_FIXED,
    AC_FAN_MODE_MAX,
}ac_fan_mode_t;


/**
 * @brief
 *
*/
typedef struct ac_param
{
    float temperature;  //空调温度控制
    ir_uint8_t modes;        //空调运行模式
    ir_uint8_t modes_data[6];      //空调运行模式数据
    ac_fan_mode_t fan_mode;     //送风模式
    ir_uint8_t fan_mode_data[6];
    ir_uint8_t timer;        //定时
    ir_uint8_t temp_data[32];//温度代码
}ac_param_t;

typedef struct ac_device {
    dev_cmd_t cmd_data;   //需要发送的波形数据
    ac_param_t param;     // 空调参数配置
    ir_codec_cfg_t codec_fig;//波形信息参数配置
    float min_temp;       // 可设置的温度下限
    float max_temp;       //可设置的温度上限
    char ir_data[16];       //空调的红外码
    char ir_data_off[16];   //空调固定的关闭码
    ir_uint8_t ac_state;
    ir_uint8_t ir_data_len;
    char* name;          //空调厂家名称
}ac_dev_t;

extern ac_dev_t ac_dev[];

ir_uint16_t ir_data_encode(ac_dev_t* ac_device);
#endif