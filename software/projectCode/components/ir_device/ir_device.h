/**
 * @file ir_device.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-03
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef IR_DEVICE_H
#define IR_DEVICE_H

typedef enum {
    IR_CODE_BYTE_HANDLE = 0X30,
    IR_CODE_BYTE_AC_TYPE = 0X06,
    IR_CODE_BYTE_AC_OFF = 0X80,
    IR_CODE_BYTE_AC_ON,
    IR_CODE_BYTE_AC_MODE_AUTO = 0XA1,
    IR_CODE_BYTE_AC_MODE_COOL,
    IR_CODE_BYTE_AC_MODE_DRY,
    IR_CODE_BYTE_AC_MODE_FAN_ONLY,
    IR_CODE_BYTE_AC_MODE_HEAT,
    IR_CODE_BYTE_AC_TEMPERATURE_16 = 0X40,
    IR_CODE_BYTE_AC_TEMPERATURE_17,
    IR_CODE_BYTE_AC_TEMPERATURE_18,
    IR_CODE_BYTE_AC_TEMPERATURE_19,
    IR_CODE_BYTE_AC_TEMPERATURE_20,
    IR_CODE_BYTE_AC_TEMPERATURE_21,
    IR_CODE_BYTE_AC_TEMPERATURE_22,
    IR_CODE_BYTE_AC_TEMPERATURE_23,
    IR_CODE_BYTE_AC_TEMPERATURE_24,
    IR_CODE_BYTE_AC_TEMPERATURE_25,
    IR_CODE_BYTE_AC_TEMPERATURE_26,
    IR_CODE_BYTE_AC_TEMPERATURE_27,
    IR_CODE_BYTE_AC_TEMPERATURE_28,
    IR_CODE_BYTE_AC_TEMPERATURE_29,
    IR_CODE_BYTE_AC_TEMPERATURE_30,
    IR_CODE_BYTE_AC_TEMPERATURE_31,
    IR_CODE_BYTE_AC_FAN_MODE_AUTO = 0X51,
    IR_CODE_BYTE_AC_FAN_MODE_LOW,
    IR_CODE_BYTE_AC_FAN_MODE_MEDIUM,
    IR_CODE_BYTE_AC_FAN_MODE_HIGH,
    IR_CODE_BYTE_AC_TREND_UP = 0X61,//风向向上
    IR_CODE_BYTE_AC_TREND_MEDIUM,  //中间风向
    IR_CODE_BYTE_AC_TREND_DOWN,   //风向向下
    IR_CODE_BYTE_AC_TREND_AUTO_ON = 0X70, //打开自动风向
    IR_CODE_BYTE_AC_TREND_AUTO_OFF,  //关闭自动风向
    IR_CODE_BYTE_AC_LIGHT_ON = 0XD0, //打开灯光
    IR_CODE_BYTE_AC_LIGHT_OFF,    //关闭灯光
}ir_code_byte_t;

void ir_dvice_init(void);
void ir_codec_start_learn(void);
void ir_codec_set_power(int power_state);
void ir_codec_set_mode(int mode);
void ir_codec_set_temperature(unsigned char temperature);
void ir_codec_set_fan_mode(unsigned char fan_mode);
void ir_codec_set_trend(unsigned char trend);
void ir_codec_set_trend_auto(unsigned char trend_auto);
void ir_codec_set_light_power(unsigned char light_power);
void hxd_039b_uart_recv_cb(unsigned char uart_data);
#endif
