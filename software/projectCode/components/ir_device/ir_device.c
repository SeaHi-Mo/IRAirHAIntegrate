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
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "ir_uart.h"
#include "blog.h"
#include <bl_gpio.h>
#include "easy_flash.h"
#define DBG_TAG "IR-DEVICE"
TimerHandle_t hxd_busy_timer;
TimerHandle_t hxd_power_off_timer;
bool hxd_power_off_timer_start = 0;
#endif

#define BUFFER_SIZE 2
#define POWER_OFF 1
#define POWER_ON 0
typedef struct {
    char buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} CircularBuffer;
static CircularBuffer hxd_recv_buff;

static int __hxd039b_busy = 1;
unsigned char ac_codeGrud[2] = { 0x03,0xF8 };//默认空调码组
static unsigned char hxd039b2_code_lren[] = { 0x30,0x70,0xa0 };//固定学习码

static void initBuffer(CircularBuffer* cb);
static bool isEmpty(CircularBuffer* cb);
static bool isFull(CircularBuffer* cb);
static bool enqueue(CircularBuffer* cb, unsigned char data);
static bool dequeue(CircularBuffer* cb, unsigned char* data);
static void printBuffer(CircularBuffer* cb);
static void hxd_039b2_delay_ms(uint32_t ms);
/**
 * @brief hxd039b2 串口接收回调，把这函数放到MCU的串口接收中断中运行
 *
 * @param uart_data 单字节接收数据
*/
void hxd_039b_uart_recv_cb(unsigned char uart_data)
{
    enqueue(&hxd_recv_buff, uart_data);

    if (isFull(&hxd_recv_buff)) {
        // printBuffer(&hxd_recv_buff);
        dequeue(&hxd_recv_buff, &ac_codeGrud[0]);
        dequeue(&hxd_recv_buff, &ac_codeGrud[1]);
        if (ac_codeGrud[0]!=0XFF) {
            /* 读取成功之后，保存到flash*/
#ifdef MCU_AI_WB2
            blog_info_hexdump("ir_acCode", ac_codeGrud, 2);
#endif
        }

    }
}
/**
 * @brief 保存空调码组至，可以保存至flash 或者其他地址，以实现掉电保存
 *
 * @param ac_codeGrud
 * @param size_len
*/
void hxd_039b2_save_ac_codeGrud(unsigned char* ac_codeGrud, int size_len)
{
#ifdef MCU_AI_WB2
    flash_save_new_ac_gcode(ac_codeGrud, size_len);
#endif
}
/**
 * @brief 读取
 *
 * @param ac_codeGrud
*/
int hxd_039b2_get_ac_codeGrud(unsigned char* ac_codeGrud)
{
    int g_code_len = 0;
#ifdef MCU_AI_WB2
    memset(ac_codeGrud, 0, 2);
    g_code_len = flash_get_ac_gcode(ac_codeGrud);
#endif
    return g_code_len;
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
    //如果芯片处于忙碌状态，则直接退出，不做任何操作
    __hxd039b_busy = bl_gpio_input_get_value(HXD039B2_BUSY_GPIO);
    if (!__hxd039b_busy) {
#ifdef MCU_AI_WB2
        blog_error("hxd is busy don't send data");
#endif
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
 * @brief 延迟函数，需要MCU的延时函数在此处调用
 *
 * @param ms
*/
static void hxd_039b2_delay_ms(uint32_t ms)
{
#ifdef MCU_AI_WB2
    vTaskDelay(pdMS_TO_TICKS(ms));
#endif
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
    // hxd_039b_send_data(hxd039b2_code_lren, 3);
    uint16_t cnt = 0;
    while (cnt < 3) {
        bl_uart_data_send(IR_UART_NUM, hxd039b2_code_lren[cnt]);
        cnt++;
    }
    return 0;
}
/**
 * @brief 长时间无操作，关闭HXD芯片电源，
 *
 * @param xTimer
*/
static void hxd_power_timers_cb(TimerHandle_t xTimer)
{
    uint32_t ulCount;
    configASSERT(xTimer);
    ulCount = (uint32_t)pvTimerGetTimerID(xTimer);
    /* 识别是否已经超过2s 没有操作？*/
    if (ulCount>=20) {
        /* 关闭HXD 电源并停止此定时器 */
#ifdef MCU_AI_WB2
        blog_info("hxd039 power off");
        bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_OFF);
#endif
        xTimerStop(xTimer, 0);
        vTimerSetTimerID(xTimer, 0);
        hxd_power_off_timer_start = false;
        return;
    }
    ulCount++;
    vTimerSetTimerID(xTimer, (void*)ulCount);
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

    bl_gpio_enable_output(HXD039B2_POWER_CTRL_GPIO, true, false);//使能HXD039B2 控制IO
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_OFF);//关闭HXD039B2的电源
    //初始化判忙IO
    bl_gpio_enable_input(HXD039B2_BUSY_GPIO, true, false);
    /* 读取flash 内部的空调码组 */
    if (hxd_039b2_get_ac_codeGrud(ac_codeGrud)==0) {
        /* 如果读不到，则设置默认值 */
        if (ac_codeGrud[0]==0&&ac_codeGrud[1]==0) {
            ac_codeGrud[0] = 0x03;
            ac_codeGrud[1] = 0xf8;
        }
    }
    /* 创建电源关闭定时器，默认100ms 定时*/
    hxd_power_off_timer = xTimerCreate("power off", pdMS_TO_TICKS(100), pdTRUE, (void*)0, hxd_power_timers_cb);
#endif
    /*初始化缓冲器*/
    initBuffer(&hxd_recv_buff);
}
#ifdef MCU_AI_WB2
/**
 * @brief 软件定时器定时判忙
 *
 * @param xTimer
*/
static void vTimerCallback(TimerHandle_t xTimer)
{
    uint32_t ulCount;
    configASSERT(xTimer);
    __hxd039b_busy = bl_gpio_input_get_value(HXD039B2_BUSY_GPIO);
    ulCount = (uint32_t)pvTimerGetTimerID(xTimer);
    blog_info("__hxd039b_busy=%d ulCont=%d", __hxd039b_busy, ulCount);
    //如果超时或者退出忙碌
    if (__hxd039b_busy&&ulCount>=50) {
        // 学习成功，关闭HXD039B电源
        blog_info("Learn success,ac code 0x%02X 0x%02X", ac_codeGrud[0], ac_codeGrud[1]);
        hxd_039b2_save_ac_codeGrud(ac_codeGrud, 2);
        bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_OFF);
        __hxd039b_busy = 0;
        xTimerStop(xTimer, 0);
        vTimerSetTimerID(xTimer, 0);
        xTimerDelete(xTimer, 0);

        return;
    }

    if (ulCount>=2000) {
        //停止定时器
        xTimerStop(xTimer, 0);
        /* */
        if (ulCount>=2000) {
            blog_error("Learn timerout");
            bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_OFF);
        }
        /* 删除定时器 */
         /* 归零定时器ID*/
        vTimerSetTimerID(xTimer, 0);
        __hxd039b_busy = 0;
        xTimerDelete(xTimer, 0);

        return;
    }
    ulCount++;
    vTimerSetTimerID(xTimer, (void*)ulCount);
}
#endif
/**
 * @brief 启动学习，运行此函数之后，对着红外接收头，按遥控器的开关键即可匹配红外码
 *
*/
void ir_codec_start_learn(void)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    uint16_t time_out_cont = 0;
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
    hxd_busy_timer = xTimerCreate("busy timer", pdMS_TO_TICKS(10), pdTRUE, (void*)time_out_cont, vTimerCallback);
#endif
    /* 延时时间，让HXD039B 进入工作状态*/
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //发送数据
    hxd_039b_find_code_groud();
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*10);
    //启动定时器判断HXD是否忙碌
#ifdef MCU_AI_WB2
    xTimerStart(hxd_busy_timer, 0);
#endif
}
/**
 * @brief 发送电源控制指令
 *
 * @param power_state 1为打开，0为关闭
*/
void ir_codec_set_power(int power_state)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1],power_state?IR_CODE_BYTE_AC_ON:IR_CODE_BYTE_AC_OFF };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    /* 判断电源定时器是否正在运行 */
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

    // bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_OFF);
#endif 
}
/**
 * @brief 设置模式，发送0~4
 *
 * @param mode 0: 自动模式 1:制冷模式 2:除湿模式 3：送风模式 4: 制热模式
*/
void ir_codec_set_mode(int mode)
{
    uint32_t ulCount = 0;
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1], IR_CODE_BYTE_AC_MODE_AUTO+mode };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
}
/**
 * @brief 设置温度，
 *
 * @param temperature
*/
void ir_codec_set_temperature(unsigned char temperature)
{
    uint32_t ulCount = 0;
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1], (temperature-16)+IR_CODE_BYTE_AC_TEMPERATURE_16 };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
}
/**
 * @brief 设置风力大小
 *
 * @param fan_mode 0:低速风 1:中等风力 2:强风
*/
void ir_codec_set_fan_mode(unsigned char fan_mode)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1], fan_mode+IR_CODE_BYTE_AC_FAN_MODE_AUTO };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
}
/**
 * @brief 设置风向
 *
 * @param trend 0：向上 1：中间 2：向下
*/
void ir_codec_set_trend(unsigned char trend)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1], trend+IR_CODE_BYTE_AC_TREND_UP };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
}
/**
 * @brief 自动风向开关
 *
 * @param trend_auto 0:关闭 1：开启
*/
void ir_codec_set_trend_auto(unsigned char trend_auto)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1],trend_auto?IR_CODE_BYTE_AC_TREND_AUTO_ON:IR_CODE_BYTE_AC_TREND_AUTO_OFF };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
}
/**
 * @brief 灯光开关
 *
 * @param light_power 0：关闭灯光 1：打开灯光
*/
void ir_codec_set_light_power(unsigned char light_power)
{
#ifdef MCU_AI_WB2
    // 开启HXD039B电源
    bl_gpio_output_set(HXD039B2_POWER_CTRL_GPIO, POWER_ON);
#endif 
    hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS);
    //组织数据 例如打开空调： 0x30  0x06	0x03	0x03e	 81  
    uint8_t ir_code[5] = { IR_CODE_BYTE_HANDLE,IR_CODE_BYTE_AC_TYPE,ac_codeGrud[0],ac_codeGrud[1],light_power?IR_CODE_BYTE_AC_LIGHT_ON:IR_CODE_BYTE_AC_LIGHT_OFF };
    hxd_039b_send_data(ir_code, 5);
    //延时等待发送完成后关闭HXD039B的电源
    // hxd_039b2_delay_ms(HXD_039B2_START_TIME_MS*20);
#ifdef MCU_AI_WB2
    // 关闭HXD039B的电源
    blog_info_hexdump("uart_data", ir_code, 5);
    if (hxd_power_off_timer_start) {
        /* 如果正在运行，则重新设置ID数，让定时器重新进行2S的定时*/
        vTimerSetTimerID(hxd_power_off_timer, 0);
    }
    else {
        /* 如果没在运行，则启动定时器 */
        hxd_power_off_timer_start = true;
        xTimerStart(hxd_power_off_timer, 0);
    }

#endif   
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
static bool enqueue(CircularBuffer* cb, unsigned char data)
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
static bool dequeue(CircularBuffer* cb, unsigned char* data)
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