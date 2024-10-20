/**
 * @file ir_uart.c
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
#include "ir_uart.h"
#include <bl_gpio.h>
#include "blog.h"

#define DBG_TAG "IR_UART"

static ir_recv_cb_t ir_recv_cb;

static void hosal_uart_callback(void* p_arg)
{
    uint8_t data;

    int ch;

    while ((ch = bl_uart_data_recv(IR_UART_NUM)) >= 0) {
        data = (uint8_t)ch;
        if (ir_recv_cb)
        {
            ir_recv_cb(data);
        }

    }

}

static void __uart_tx_irq(void* p_arg)
{
    bl_uart_int_tx_disable(IR_UART_NUM);
}

void ir_uart_dvice_init(ir_recv_cb_t _ir_recv_cb)
{

    int dataBits = UART_DATABITS_8;
    int stopBits = UART_STOPBITS_1;
    int parity = UART_PARITY_NONE;
    int cts_pin = 0xff;
    int rts_pin = 0xff;
    // bl_uart_init_ex(IR_UART_NUM, 4, 3, cts_pin, rts_pin, 115200,
    //                 dataBits, stopBits, parity);
    bl_uart_init(IR_UART_NUM, IR_UART_TX, IR_UART_RX, cts_pin, rts_pin, 57600);
    bl_uart_int_tx_notify_register(IR_UART_NUM, __uart_tx_irq, NULL);
    bl_uart_int_rx_notify_register(IR_UART_NUM, hosal_uart_callback, NULL);
    bl_uart_int_enable(IR_UART_NUM);
    bl_uart_int_tx_enable(IR_UART_NUM);
    // flash_set_ir_code("ir_on", midea[0].cmd_data, midea[0].cmd_date_len);
    blog_info("ir device statrt......");
    ir_recv_cb = _ir_recv_cb;
}

void ir_uart_send_data(char* data, int data_len)
{
    bl_uart_datas_send(IR_UART_NUM, (uint8_t*)data, data_len);
}