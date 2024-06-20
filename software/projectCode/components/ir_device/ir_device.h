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
    IR_DEVICE_CMD_SEND_MIDEA_CODE_ON,         /*  发送外部编码 开启*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_OFF,         /*  发送外部编码  关闭*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_MODE_AUTO,   /*  发送外部编码  自动模式*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_MODE_COOL,   /*  发送外部编码  制冷*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_MODE_DRY,   /*  发送外部编码  除湿*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_MODE_HEAT,   /*  发送外部编码  制热*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_MODE_FAN_ONLY,   /*  发送外部编码  只送风*/
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_17,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_18,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_19,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_20,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_21,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_22,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_23,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_24,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_25,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_26,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_27,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_28,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_29,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_TEMP_30,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_FAM_MODE_MUTE,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_FAM_MODE_LOW,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_FAM_MODE_MEDIUM,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_FAM_MODE_HIGH,
    IR_DEVICE_CMD_SEND_MIDEA_CODE_FAM_MODE_AUTO,
}ir_dev_cmd_t;

typedef struct dev_cmd_list {
    char cmd_data[IR_UART_DATA_SIZE_MAX];
    uint16_t cmd_date_len;
}dev_cmd_t;

void ir_dvice_init(void);
void ir_device_send_cmd(ir_dev_cmd_t ir_dev_cmd);

#endif
