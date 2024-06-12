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

#define IR_DEVICE_DATA_HEAD 0x68
#define IR_DEVICE_DATA_END 0x16
#define IR_DEVICE_UART_ADDR 0x00 //模块默认地址

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
    IR_DEVICE_CMD_SEND_CODE,         /*  发送外部编码 */
}ir_dev_cmd_t;

typedef struct dev_cmd_list {
    char cmd_data[10];
    uint8_t cmd_date_len;
}dev_cmd_t;

void ir_dvice_init(void);
#endif
