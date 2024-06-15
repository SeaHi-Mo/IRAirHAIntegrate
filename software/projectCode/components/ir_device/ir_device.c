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
#include <bl_gpio.h>


static int hosal_uart_callback(void* p_arg);


static bool uart_get_status = false;
static ring_buff_t ir_ring_buff_hd = { 0 };
static dev_cmd_t ir_uart_read_data;
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
          .port = 1,
};

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


static int hosal_uart_callback(void* p_arg)
{
    hosal_uart_dev_t* uart_dev = (hosal_uart_dev_t*)p_arg;
    static uint32_t uart_receive_len = 0;
    static uint8_t uart_data[IR_UART_DATA_SIZE_MAX];
    memset(uart_data, 0, IR_UART_DATA_SIZE_MAX);
    uart_receive_len = hosal_uart_receive(uart_dev, uart_data, IR_UART_DATA_SIZE_MAX);

    for (size_t i = 0; i < uart_receive_len; i++)
    {
        if (uart_data[i]==IR_DEVICE_DATA_END) {
            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len++] = uart_data[i];
            uart_get_status = true;
        }
        else if (uart_data[i]==IR_DEVICE_DATA_HEAD) {

            ir_uart_read_data.cmd_date_len = 0;
            memset(ir_uart_read_data.cmd_data, 0, IR_UART_DATA_SIZE_MAX);

            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len] = uart_data[i];
            ir_uart_read_data.cmd_date_len++;
        }
        else {
            ir_uart_read_data.cmd_data[ir_uart_read_data.cmd_date_len] = uart_data[i];
            ir_uart_read_data.cmd_date_len++;
        }
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


void ir_dvice_init(void)
{
    hosal_uart_init(&ir_uart_dev);


    xTaskCreate(ir_data_ring_task, "ir task", 1024, NULL, 8, NULL);

    blog_info("ir device statrt......");
}

void ir_device_send_cmd(ir_dev_cmd_t ir_dev_cmd)
{
    dev_cmd_t* ir_cmd_data = create_ir_cmd(ir_dev_cmd);
    blog_debug("cmdlen=%d =%02x %02x %02x %02x %02x %02x %02x %02x", ir_cmd_data->cmd_date_len, ir_cmd_data->cmd_data[0], ir_cmd_data->cmd_data[1], ir_cmd_data->cmd_data[2], ir_cmd_data->cmd_data[3], ir_cmd_data->cmd_data[4], ir_cmd_data->cmd_data[5], ir_cmd_data->cmd_data[6], ir_cmd_data->cmd_data[7]);
    hosal_uart_send(&ir_uart_dev, ir_cmd_data->cmd_data, ir_cmd_data->cmd_date_len);
}