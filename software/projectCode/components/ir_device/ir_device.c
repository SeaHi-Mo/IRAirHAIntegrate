/**
 * @file ir_device.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-03
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hosal_uart.h>
#include "blog.h"
#include "ir_device.h"
#include "at_ringbuff.h"

#define IR_UART_DATA_SIZE_MAX 25

static int hosal_uart_callback(void* p_arg);
static uint8_t uart_data[IR_UART_DATA_SIZE_MAX];
static uint8_t uart_ring_buffer[IR_UART_DATA_SIZE_MAX];
static ring_buff_t ir_ring_buff_hd = { 0 };

static dev_cmd_t ir_cmd;

static hosal_uart_dev_t ir_uart_dev = {
         .config = {
             .uart_id = 1,
             .tx_pin = 4, // TXD GPIO
             .rx_pin = 3, // RXD GPIO
             .cts_pin = 255,
             .rts_pin = 255,
             .data_width = HOSAL_DATA_WIDTH_8BIT,
             .parity = HOSAL_NO_PARITY,
             .stop_bits = HOSAL_STOP_BITS_1,
             .mode = HOSAL_UART_MODE_INT,
             .baud_rate = 115200,
         },
         .rx_cb = hosal_uart_callback,
         .p_rxarg = &ir_uart_dev,
};

static int hosal_uart_callback(void* p_arg)
{
    hosal_uart_dev_t* uart_dev = (hosal_uart_dev_t*)p_arg;
    static uint32_t uart_receive_len = 0;
    memset(uart_data, 0, IR_UART_DATA_SIZE_MAX);
    int len = hosal_uart_receive(uart_dev, uart_data, IR_UART_DATA_SIZE_MAX);
    ring_buff_push_data(&ir_ring_buff_hd, (uint8_t*)uart_data, strlen((char*)uart_data));
    return 0;
}

static void ir_data_ring_task(void* arg)
{
    uint32_t uart_receive_len = 0;
    while (1)
    {
        if (ring_buff_get_size(&ir_ring_buff_hd) <= 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        uart_receive_len = ring_buff_get_size(&ir_ring_buff_hd);
        //读取缓冲区内容
        ring_buff_pop_data(&ir_ring_buff_hd, uart_ring_buffer, ring_buff_get_size(&ir_ring_buff_hd)+1);

        if (uart_receive_len >= IR_UART_DATA_SIZE_MAX)
        {
            ring_buff_flush(&ir_ring_buff_hd);
            uart_receive_len = 0;
            memset(uart_ring_buffer, 0, IR_UART_DATA_SIZE_MAX);
            continue;
        }

        //处理完成，清空缓冲区内容，方便重新写入
        ring_buff_flush(&ir_ring_buff_hd);
    }


}

void ir_dvice_init(void)
{
    hosal_uart_init(&ir_uart_dev);
    ring_buff_init(&ir_ring_buff_hd, (char*)uart_ring_buffer, IR_UART_DATA_SIZE_MAX);
    xTaskCreate(ir_data_ring_task, "ir task", 1024, NULL, 13, NULL);
}

void ir_device_send_cmd()
{

}