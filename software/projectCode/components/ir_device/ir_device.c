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
#define AI-WB2_IR

#ifdef AI-WB2_IR
#include "ir_uart.h"
#include "blog.h"
#define DBG_TAG "IR-DEVICE"
#endif
//空调列表
static hxd039b_msg_t hxd_code_list[] = {
    {"格力",{0x03,0x3E}}, //格力空调
};

/**
 * @brief 发送给hxb_039b 的代码，应该把串口发送函数再次运行
 *
 * @param data
 * @param data_len
*/
static void hxd_039b_send_data(char* data, int data_len)
{

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

}
