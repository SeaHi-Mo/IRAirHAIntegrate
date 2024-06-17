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
#include "blog.h"
#include "ir_device.h"
#include "bl_uart.h"
#include <bl_gpio.h>
#define IR_UART_NUM 1

static bool uart_get_status = false;
static dev_cmd_t ir_uart_read_data;
static dev_cmd_t ir_cmd;

static dev_cmd_t* create_ir_cmd(ir_dev_cmd_t ir_dev_cmd)
{
    dev_cmd_t* ir_cmd_data = &ir_cmd;
    memset(ir_cmd_data->cmd_data, 0, sizeof(ir_cmd_data->cmd_data));
    ir_cmd_data->cmd_date_len = 0;
    ir_cmd_data->cmd_data[0] = IR_DEVICE_DATA_HEAD;
    ir_cmd_data->cmd_data[3] = IR_DEVICE_UART_ADDR;
    switch (ir_dev_cmd)
    {
        case IR_DEVICE_CMD_SET_BAUD_RATE_9600:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x00;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_19200:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x01;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_38400:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x02;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_57600:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x03;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_115200:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x04;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_GET_BAUD_RATE:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_GET_BUAD_RATE_BYTE;
            ir_cmd_data->cmd_data[5] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[6] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_GO_FLASH_LEARN:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_GOTO_IN_LEARN_BYTE;
            ir_cmd_data->cmd_data[5] = 0x00;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_1:
        {
            ir_cmd_data->cmd_data[2] = 0x00;
            ir_cmd_data->cmd_data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->cmd_data[5] = 0x00;
            ir_cmd_data->cmd_data[6] = (ir_cmd_data->cmd_data[3]+ ir_cmd_data->cmd_data[4]+ir_cmd_data->cmd_data[5])%256;
            ir_cmd_data->cmd_data[7] = IR_DEVICE_DATA_END;
        }
        break;
        default:
            break;
    }
    for (size_t i = 0;ir_cmd_data->cmd_data[i]!=IR_DEVICE_DATA_END; i++)
    {
        ir_cmd_data->cmd_data[1] += 1;
    }
    ir_cmd_data->cmd_data[1] += 1;
    ir_cmd_data->cmd_date_len = ir_cmd_data->cmd_data[1];

    return ir_cmd_data;
}


static void hosal_uart_callback(void* p_arg)
{
    uint8_t data;

    int ch;

    while ((ch = bl_uart_data_recv(IR_UART_NUM)) >= 0) {
        data = (uint8_t)ch;
        if (data ==IR_DEVICE_DATA_END) {
            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len++] = data;
            uart_get_status = true;
        }
        else if (data ==IR_DEVICE_DATA_HEAD) {

            ir_uart_read_data.cmd_date_len = 0;
            memset(ir_uart_read_data.cmd_data, 0, IR_UART_DATA_SIZE_MAX);

            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len] = data;
            ir_uart_read_data.cmd_date_len++;
        }
        else {
            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len] = data;
            ir_uart_read_data.cmd_date_len++;
        }
    }
}


static int HAL_at_uart_send(char* buf, uint16_t size)
{
    uint16_t cnt = 0;

    while (cnt < size) {
        bl_uart_data_send(IR_UART_NUM, ((uint8_t*)buf)[cnt]);
        cnt++;
    }

    return 0;
}
static void ir_data_ring_task(void* arg)
{
    uint32_t uart_receive_len = 0;
    blog_info("ir task start .......");
    while (1)
    {
        if (!uart_get_status)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        uart_get_status = false;
        for (size_t i = 0; i < ir_uart_read_data.cmd_date_len; i++)
        {
            printf("%02x ", ir_uart_read_data.cmd_data[i]);
        }
        printf("\r\n");

    }
}

static void __uart_tx_irq(void* p_arg)
{
    bl_uart_int_tx_disable(IR_UART_NUM);
}

void ir_dvice_init(void)
{

    int dataBits = UART_DATABITS_8;
    int stopBits = UART_STOPBITS_1;
    int parity = UART_PARITY_NONE;
    int cts_pin = 0xff;
    int rts_pin = 0xff;


    // bl_uart_init_ex(IR_UART_NUM, 4, 3, cts_pin, rts_pin, 115200,
    //                 dataBits, stopBits, parity);
    bl_uart_init(IR_UART_NUM, 4, 3, cts_pin, rts_pin, 115200);
    bl_uart_int_tx_notify_register(IR_UART_NUM, __uart_tx_irq, NULL);
    bl_uart_int_rx_notify_register(IR_UART_NUM, hosal_uart_callback, NULL);
    bl_uart_int_enable(IR_UART_NUM);
    bl_uart_int_tx_enable(IR_UART_NUM);

    xTaskCreate(ir_data_ring_task, "ir task", 1024, NULL, 8, NULL);

    blog_info("ir device statrt......");
}

void ir_device_send_cmd(ir_dev_cmd_t ir_dev_cmd)
{
    dev_cmd_t* ir_cmd_data = create_ir_cmd(ir_dev_cmd);
    blog_debug("cmdlen=%d =%02x %02x %02x %02x %02x %02x %02x %02x", ir_cmd_data->cmd_date_len, ir_cmd_data->cmd_data[0], ir_cmd_data->cmd_data[1], ir_cmd_data->cmd_data[2], ir_cmd_data->cmd_data[3], ir_cmd_data->cmd_data[4], ir_cmd_data->cmd_data[5], ir_cmd_data->cmd_data[6], ir_cmd_data->cmd_data[7]);
    // hosal_uart_send(&ir_uart_dev, ir_cmd_data->cmd_data, ir_cmd_data->cmd_date_len);
    HAL_at_uart_send(ir_cmd_data->cmd_data, ir_cmd_data->cmd_date_len);

}