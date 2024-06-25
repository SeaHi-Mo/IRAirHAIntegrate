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
#include "device_state.h"
#include "ir_codec_config.h"
#define IR_UART_DATA_SIZE_MAX 512
#define IR_DEVICE_DATA_HEAD 0x68
#define IR_DEVICE_DATA_END 0x16
#define IR_DEVICE_UART_ADDR 0x00 //模块默认地址


#define IR_CMD_SET_BUAD_RATE_BYTE 0x03
#define IR_CMD_GET_BUAD_RATE_BYTE 0x04
#define IR_CMD_SET_ADDR_BYTE 0x05
#define IR_CMD_GET_ADDR_BYTE 0x06
#define IR_CMD_REBOOT_BYTE 0x07
#define IR_CMD_RESET_BYTE 0x08
#define IR_CMD_GOTO_IN_LEARN_BYTE 0x10
#define IR_CMD_EXIT_IN_LEARN_BYTE 0x11
#define IR_CMD_SEND_IN_CODE_BYTE 0x12
#define IR_CMD_SET_AUTO_SEND_BYTE 0x13
#define IR_CMD_GET_AUTO_SEND_BYTE 0x14
#define IR_CMD_SET_AUTO_SEND_TIMER_BYTE 0x15
#define IR_CMD_GET_AUTO_SEND_TIMER_BYTE 0x16
#define IR_CMD_WRITE_FLASH_CODE_BYTE 0x17
#define IR_CMD_READ_FLASH_CODE_BYTE 0x18
#define IR_CMD_GOTO_LEARN_CODE_BYTE 0x20
#define IR_CMD_EXIT_LEARN_CODE_BYTE 0x21
#define IR_CMD_SEND_LEARN_CODE_BYTE 0x22

typedef enum {
    IR_DEVICE_CMD_NONE = -1,
    IR_DEVICE_CMD_SET_BAUD_RATE_9600,    /*   设置模块波特率 9600  */
    IR_DEVICE_CMD_SET_BAUD_RATE_19200,    /*   设置模块波特率 19200   */
    IR_DEVICE_CMD_SET_BAUD_RATE_38400,    /*   设置模块波特率 38400  */
    IR_DEVICE_CMD_SET_BAUD_RATE_57600,    /*   设置模块波特率 57600  */
    IR_DEVICE_CMD_SET_BAUD_RATE_115200,    /*   设置模块波特率 115200  */
    IR_DEVICE_CMD_GET_BAUD_RATE,    /*   读取模块波特率   */
    IR_DEVICE_CMD_SET_ADDR,         /*   设置模块地址     */
    IR_DEVICE_CMD_GET_ADDR,         /*   读取模块地址     */
    IR_DEVICE_CMD_RESET,            /*   复位模块         */
    IR_DEVICE_CMD_DEINIT,           /*   格式化模块       */
    IR_DEVICE_CMD_GO_FLASH_LEARN,   /*   进入内部学习模式   */
    IR_DEVICE_CMD_EXIT_FLASH_LEARN,  /*   退出内部学习模式   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_1,  /*   发送内部学习编码1   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_2,  /*   发送内部学习编码2   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_3,  /*   发送内部学习编码3   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_4,  /*   发送内部学习编码4   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_5,  /*   发送内部学习编码5   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_6,  /*   发送内部学习编码6   */
    IR_DEVICE_CMD_SEND_FLASH_CODE_7,  /*   发送内部学习编码7   */
    IR_DEVICE_CMD_SET_AUTO_SEND_CODE_NUMBER,/*  设置上电自动发送的编码序号    */
    IR_DEVICE_CMD_GET_AUTO_SEND_CODE_NUMBER,/*  获取上电自动发送的编码序号    */
    IR_DEVICE_CMD_SET_AUTO_SEND_TIMER, /*  设置上电自动发送的延时时间    */
    IR_DEVICE_CMD_GET_AUTO_SEND_TIMER, /*  设置上电自动发送的延时时间  */
    IR_DEVICE_CMD_WRIRE_FLASH_CODE,    /*  写入内部存储编码 */
    IR_DEVICE_CMD_READ_FLASH_CODE,     /*  读取内部存储编码 */
    IR_DEVICE_CMD_GO_CODE_LEARN,      /* 进入外部存储编码学习 */
    IR_DEVICE_CMD_EXIT_CODE_LEARN,    /*  退出外部存储编码学习 */
    IR_DEVICE_CMD_SEND_MIDEA_CODE,         /*  发送外部编码 开启*/
}ir_dev_cmd_t;

typedef enum ir_dev_type {
    IR_DEVICE_TYPE_NONE = -1,
    IR_DEVICE_TYPE_AC_BRAND_MIDEA = 0, //美的空调
    IR_DEVICE_TYPE_AC_BRAND_GREE,      //格力空调
    IR_DEVICE_TYPE_AC_BRAND_TCL,       //TCL 空调
    IR_DEVICE_TYPE_AC_BRAND_TYPE_HAIER,     //海尔空调
}ir_dev_type_t;


typedef struct ir_dev {
    //空调设备
    ac_dev_t* ac_dev;
    ir_dev_type_t ac_brand_type;
}ir_dev_t;


void ir_dvice_init(void);
/**
 * @brief 空调开关含描述
 *
 * @param ac_brand_type 空调的类型
 * @param power_state 0 为关，1 为开
*/
void ir_codec_config_set_power(int ac_brand_type, int power_state);
/**
 * @brief 空调设置模式
 *
 * @param ac_brand_type
 * @param modes_cnt
*/
void ir_codec_config_set_modes(int ac_brand_type, uint8_t modes_cnt);
/**
 * @brief 控制空调温度
 *
 * @param ac_brand_type 空调的类型
 * @param temperature  温度值
*/
void ir_codec_config_set_temperature(int ac_brand_type, float temperature);
/**
 * @brief
 *
 * @param ac_brand_type
 * @param modes_cnt
*/
void ir_codec_config_set_fan_modes(int ac_brand_type, uint8_t modes_cnt);

#endif
