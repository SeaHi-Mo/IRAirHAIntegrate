/**
 * @file at_receive.c
 * @author seahi-mo (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <FreeRTOS.h>
#include <task.h>
#include <hosal_uart.h>
#include <blog.h>

#include "at_receive.h"
#include "at_ringbuff.h"
#include "at_command.h"

#define AT_CMD_HEADER "AT"
#define AT_CMD_DATA_SIZE_MAX 256

static TaskHandle_t atFunction_task;
static cmd_callback_t AT_cmd_info_tab[];
static ring_buff_t uart_ring_buff_hd = { 0 };
static uint8_t uart_data[AT_CMD_DATA_SIZE_MAX];
static uint8_t uart_ring_buffer[AT_CMD_DATA_SIZE_MAX];

static int hosal_uart_callback(void* p_arg);
static hosal_uart_dev_t uart_dev = {
         .config = {
             .uart_id = 0,
             .tx_pin = 16, // TXD GPIO
             .rx_pin = 7,  // RXD GPIO
             .cts_pin = 255,
             .rts_pin = 255,
             .data_width = HOSAL_DATA_WIDTH_8BIT,
             .parity = HOSAL_NO_PARITY,
             .stop_bits = HOSAL_STOP_BITS_1,
             .mode = HOSAL_UART_MODE_INT,
         },
         .rx_cb = hosal_uart_callback,
         .p_rxarg = &uart_dev,
};

void atCommandExecute(char* buff);

static int hosal_uart_callback(void* p_arg)
{
    hosal_uart_dev_t* uart_dev = (hosal_uart_dev_t*)p_arg;
    static uint32_t uart_receive_len = 0;
    memset(uart_data, 0, AT_CMD_DATA_SIZE_MAX);
    int len = hosal_uart_receive(uart_dev, uart_data, AT_CMD_DATA_SIZE_MAX);
    ring_buff_push_data(&uart_ring_buff_hd, (uint8_t*)uart_data, strlen((char*)uart_data));

    return 0;
}

static void atCommandHandleFunction(void* arg)
{
    uint32_t uart_receive_len = 0;
    while (1)
    {

        if (ring_buff_get_size(&uart_ring_buff_hd) <= 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        uart_receive_len = ring_buff_get_size(&uart_ring_buff_hd);
        //读取缓冲区内容
        ring_buff_pop_data(&uart_ring_buff_hd, uart_ring_buffer, ring_buff_get_size(&uart_ring_buff_hd)+1);

        if (uart_receive_len >= AT_CMD_DATA_SIZE_MAX)
        {
            ring_buff_flush(&uart_ring_buff_hd);
            uart_receive_len = 0;
            memset(uart_ring_buffer, 0, AT_CMD_DATA_SIZE_MAX);
            continue;
        }

        if ((uart_receive_len >= 4) && ('\r' == uart_ring_buffer[uart_receive_len-2]) && ('\n' == uart_ring_buffer[uart_receive_len-1]))
        {
            // blog_info("rev:%s ", uart_ring_buffer);
            // blog_info("cmdlen:%d ", uart_receive_len);
            uart_ring_buffer[uart_receive_len-2] = '\0';
            atCommandExecute((char*)uart_ring_buffer);
            uart_receive_len = 0;
            memset(uart_ring_buffer, 0, AT_CMD_DATA_SIZE_MAX);
        }
        //处理完成，清空缓冲区内容，方便重新写入
        ring_buff_flush(&uart_ring_buff_hd);
        // vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void atUartInit(uint32_t baud_rate)
{


    uart_dev.config.baud_rate = baud_rate;
    hosal_uart_init(&uart_dev);
    ring_buff_init(&uart_ring_buff_hd, (char*)uart_ring_buffer, AT_CMD_DATA_SIZE_MAX);
    xTaskCreate(atCommandHandleFunction, "AT task", 1024, NULL, 14, &atFunction_task);
}

bool containsChar(const char* str, char c)
{

    while (*str!='\0') {  // 遍历字符串直到遇到结束符'\0'  
        if (*str == c) {
            return true;  // 如果找到匹配的字符，返回true  
        }
        str++;  // 移动到下一个字符  
    }
    return false;  // 如果没有找到匹配的字符，返回false  
}

void atCommandExecute(char* buff)
{
    char* cmdPoint = NULL;
    uint16_t cmd_data_len = 0;
    uint16_t cmd_len = 0;
    uint16_t len = 0;
    bool cmdStatus = false;
    if (memcmp(buff, AT_CMD_HEADER, strlen(AT_CMD_HEADER)) == 0)
    {
        cmdPoint = strstr(buff, AT_CMD_HEADER);
        if (cmdPoint)
        {
            cmd_data_len = strlen(cmdPoint);
            for (uint16_t num = 0; at_cmd_list[num].cmd != NULL; num++)
            {
                cmd_len = strlen(at_cmd_list[num].cmd);
                if (!memcmp(cmdPoint, at_cmd_list[num].cmd, cmd_len)) {

                    if (containsChar(cmdPoint, '=')) {//识别为设置指令
                        if (at_cmd_list[num].set_func!=NULL)
                            at_cmd_list[num].set_func((cmdPoint + cmd_len+1), len);
                    }
                    else if (containsChar(cmdPoint, '?')) {//识别为设置查询指令
                        if (at_cmd_list[num].check_func!=NULL)
                            at_cmd_list[num].check_func((cmdPoint + cmd_len+1), len);
                    }
                    else if (strlen(cmdPoint)==strlen(at_cmd_list[num].cmd)) {//识别为测试指令
                        if (at_cmd_list[num].test_func!=NULL)
                            at_cmd_list[num].test_func(cmdPoint, len);
                    }
                    cmdStatus = true;
                    return;
                }
            }
            if (!cmdStatus)
            {
                blog_error("Can not find the cmd[%s] \r\n", cmdPoint);
                AT_RESPONSE("ERR\r\n");
            }
        }
        else
        {
            AT_RESPONSE("ERR\r\n");
            blog_error("cmd parse err \r\n");
        }
    }
    else
    {
        blog_error("Can not find the cmd[%s] \r\n", buff);
        AT_RESPONSE("ERR\r\n");
    }
}

void at_cmd_response(char* cmd)
{
    if (cmd == NULL)
    {
        blog_error("param err \r\n");
        return;
    }

    hosal_uart_send(&uart_dev, cmd, strlen(cmd));
}



