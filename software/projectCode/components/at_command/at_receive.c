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
#include "bl_uart.h"
#include "at_receive.h"
#include "at_command.h"

#define AT_CMD_HEADER "AT"
#define AT_CMD_DATA_SIZE_MAX 256
#define AT_UART_NUM 0
static TaskHandle_t atFunction_task;

static uint8_t at_data[AT_CMD_DATA_SIZE_MAX];
static uint16_t at_data_size = 0;
static void hosal_uart_callback(void* p_arg);
static bool uart_reading_data = false;
void atCommandExecute(char* buff);

static void hosal_uart_callback(void* p_arg)
{


    uint8_t data;

    int ch;
    // uart_reading_data = true;
    while ((ch = bl_uart_data_recv(AT_UART_NUM)) >= 0) {
        data = (uint8_t)ch;
        at_data[at_data_size] = data;
        at_data_size++;

    }
    // uart_reading_data = false;
}

int HAL_at_uart_send(char* buf, uint16_t size)
{
    uint16_t cnt = 0;

    while (cnt < size) {
        bl_uart_data_send(AT_UART_NUM, ((uint8_t*)buf)[cnt]);
        cnt++;
    }
    return 0;
}

static void __uart_tx_irq(void* p_arg)
{
    bl_uart_int_tx_disable(AT_UART_NUM);
}

static void atCommandHandleFunction(void* arg)
{

    while (1)
    {

        if (at_data_size<=3)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        //读取缓冲区内容
        char* uart_ring_buffer = pvPortMalloc(AT_CMD_DATA_SIZE_MAX);
        memset(uart_ring_buffer, 0, AT_CMD_DATA_SIZE_MAX);
        memcpy(uart_ring_buffer, at_data, at_data_size);
        // blog_debug("%s", uart_ring_buffer);
        if ((at_data_size >= 4) && ('\r' == uart_ring_buffer[at_data_size-2]) && ('\n' == uart_ring_buffer[at_data_size-1]))
        {
            printf("data=%s\r\n", uart_ring_buffer);
            uart_ring_buffer[at_data_size-2] = '\0';
            atCommandExecute((char*)uart_ring_buffer);
            memset(at_data, 0, AT_CMD_DATA_SIZE_MAX);
            at_data_size = 0;
            vTaskDelay(pdMS_TO_TICKS(50));
            blog_debug("HeapSize=%d ", xPortGetFreeHeapSize());
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        //处理完成，清空缓冲区内容，方便重新写入
        vTaskDelay(pdMS_TO_TICKS(50));
        at_data_size = 0;
        memset(uart_ring_buffer, 0, AT_CMD_DATA_SIZE_MAX);
        vPortFree(uart_ring_buffer);
    }
}

void atUartInit(uint32_t baud_rate)
{
    // uart_dev.config.baud_rate = baud_rate;
    int dataBits = UART_DATABITS_8;
    int stopBits = UART_STOPBITS_1;
    int parity = UART_PARITY_NONE;
    int cts_pin = 0xff;
    int rts_pin = 0xff;

    // hosal_uart_init(&uart_dev);
    bl_uart_init_ex(AT_UART_NUM, 16, 7, cts_pin, rts_pin, baud_rate,
                   dataBits, stopBits, parity);
    bl_uart_int_tx_notify_register(AT_UART_NUM, __uart_tx_irq, NULL);
    bl_uart_int_rx_notify_register(AT_UART_NUM, hosal_uart_callback, NULL);
    bl_uart_int_enable(AT_UART_NUM);
    bl_uart_int_tx_disable(AT_UART_NUM);
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
    bool cmdStatus = false;
    if (memcmp(buff, AT_CMD_HEADER, 2) == 0)
    {
        cmdPoint = strstr(buff, AT_CMD_HEADER);
        if (cmdPoint!=NULL)
        {
            cmd_data_len = strlen(cmdPoint);
            blog_info("cmdPoint=%s", cmdPoint);
            for (uint16_t num = 0; at_cmd_list[num].cmd[0] != '\0'; num++)
            {
                cmd_len = strlen(at_cmd_list[num].cmd);
                if (!strncmp(cmdPoint, at_cmd_list[num].cmd, cmd_len)) {

                    if (containsChar(cmdPoint, '=')) {//识别为设置指令
                        if (at_cmd_list[num].set_func!=NULL)
                            at_cmd_list[num].set_func((cmdPoint + cmd_len+1), cmd_len);
                    }
                    else if (containsChar(cmdPoint, '?')) {//识别为设置查询指令
                        if (at_cmd_list[num].check_func!=NULL)
                            at_cmd_list[num].check_func((cmdPoint + cmd_len+1), cmd_len);
                    }
                    else if (strlen(cmdPoint)==strlen(at_cmd_list[num].cmd)) {//识别为测试指令
                        if (at_cmd_list[num].test_func!=NULL)
                            at_cmd_list[num].test_func(cmdPoint, cmd_len);
                    }
                    cmdStatus = true;
                    return;
                }
                // vTaskDelay(pdMS_TO_TICKS(2));
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

    // hosal_uart_send(&uart_dev, cmd, strlen(cmd));
    HAL_at_uart_send(cmd, strlen(cmd));
    // printf("%.*s", strlen(cmd), cmd);
}



