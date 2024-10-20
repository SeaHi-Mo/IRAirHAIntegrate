/**
 * @file ir_device.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ir_device.h"

/* 芯片头文件使能 如果使用了其他的芯片 把屏蔽掉*/
#define MCU_AI_WB2

#ifdef MCU_AI_WB2
#include "ir_uart.h"
#include "blog.h"
#include <bl_gpio.h>
#define DBG_TAG "IR-DEVICE"
#endif

#define BUFFER_SIZE 2

typedef struct {
    char buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} CircularBuffer;
static CircularBuffer hxd_recv_buff;

static char ac_codeGrud[2] = { 0x03,0x3e };//默认空调码组
static unsigned char hxd039b2_code_lren[] = { 0x30,0x70,0xa0 };//固定学习码

static void initBuffer(CircularBuffer* cb);
static bool isEmpty(CircularBuffer* cb);
static bool isFull(CircularBuffer* cb);
static bool enqueue(CircularBuffer* cb, char data);
static bool dequeue(CircularBuffer* cb, char* data);
static void printBuffer(CircularBuffer* cb);

/**
 * @brief hxd039b2 串口接收回调，把这函数放到MCU的串口接收中断中运行
 *
 * @param uart_data 单字节接收数据
*/
void hxd_039b_uart_recv_cb(unsigned char uart_data)
{
    enqueue(&hxd_recv_buff, uart_data);

    if (isFull(&hxd_recv_buff)) {
        printBuffer(&hxd_recv_buff);
        dequeue(&hxd_recv_buff, &ac_codeGrud[0]);
        dequeue(&hxd_recv_buff, &ac_codeGrud[1]);
    }
}
/**
 * @brief 发送给hxb_039b 的代码，应该把串口发送函数再次运行
 *
 * @param data
 * @param data_len
*/
static void hxd_039b_send_data(unsigned char* data, int data_len)
{
    //Ai-WB2的串口发送函数
    if (data==NULL) {
        printf("data is NULL \r\n");
        return;
    }
#ifdef MCU_AI_WB2
    // bl_uart_datas_send(IR_UART_NUM, data, data_len);
    uint16_t cnt = 0;
    while (cnt < data_len) {
        bl_uart_data_send(IR_UART_NUM, data[cnt]);
        cnt++;
    }
#endif
    /*  其他主控的串口发送函数*/
}
/**
 * @brief hxd_039b 开始匹配红外码,需要按匹配键，之后使用目标遥控器开关进行匹配
 *
 * @param code_groud 缓存红外码组，成功则缓存
 * @return int
 * 成功输出 红外码组，失败输出 -1。输出的码组为16位整数，需要自行分离2个8位的数据
*/
static int hxd_039b_find_code_groud(void)
{
    hxd_039b_send_data(hxd039b2_code_lren, 3);
    return 0;
}
/**
 * @brief 红外设备初始化
 *
*/
void ir_dvice_init(void)
{
    //串口初始化接口
#ifdef MCU_AI_WB2
    ir_uart_dvice_init(hxd_039b_uart_recv_cb);
    bl_gpio_enable_output(3, true, false);//使能HXD039B2 控制IO
    bl_gpio_output_set(3, 1);//关闭HXD039B2的电源
#endif
    /*初始化缓冲器*/
    initBuffer(&hxd_recv_buff);

}
/**
 * @brief 启动学习，按遥控器的开关键匹配
 *
*/
void ir_codec_start_learn(void)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(3, 0);

#endif
    //发送数据
    hxd_039b_find_code_groud();
}

/**
 * @brief 以下是缓冲器程序
 *
*/

/**
 * @brief 初始化回环缓冲
 *
 * @param cb
 * @return * void
*/
static void initBuffer(CircularBuffer* cb)
{
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    memset(cb->buffer, 0, BUFFER_SIZE);
}
/**
 * @brief 判断 检查缓冲区是否为空
 *
 * @param cb 缓冲器枚举
 * @return true 为空
 * @return false 不为空
*/
static bool isEmpty(CircularBuffer* cb)
{
    return cb->count==0;
}
/**
 * @brief 判断缓冲器是否已满？
 *
 * @param cb 缓冲器枚举
 * @return true 已满
 * @return false 没满
*/
static bool isFull(CircularBuffer* cb)
{
    return cb->count == BUFFER_SIZE;
}
/**
 * @brief 向缓冲区添加数据
 *
 * @param cb  缓冲器枚举
 * @param data 数据
 * @return true 添加成功
 * @return false 添加失败
*/
static bool enqueue(CircularBuffer* cb, char data)
{
    if (isFull(cb)) {
        return false;  // 缓冲区已满，添加失败  
    }
    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;
    return true;
}
/**
 * @brief 从缓冲器取数据
 *
 * @param cb 缓冲器枚举
 * @param data 数据
 * @return true 取出成功
 * @return false 取出失败
*/
static bool dequeue(CircularBuffer* cb, char* data)
{
    if (isEmpty(cb)) {
        return false;  // 缓冲区为空，取出失败  
    }
    *data = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;
    return true;
}
/**
 * @brief 打印缓冲区内容（用于调试）
 *
 * @param cb
 * @return * void
*/
static void printBuffer(CircularBuffer* cb)
{
    printf("Buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int index = (cb->tail + i) % BUFFER_SIZE;
        if (index == cb->head && i < cb->count) {
            printf("|");  // 指示缓冲区头和尾之间的分隔  
        }
        printf("0x%02X ", cb->buffer[index]);
    }
    printf("\r\n");
}