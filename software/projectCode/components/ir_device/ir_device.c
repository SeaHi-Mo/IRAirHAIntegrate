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

BLOG_DECLARE(ir_device);

static bool uart_get_status = false;
static dev_cmd_t ir_uart_read_data;
static dev_cmd_t ir_cmd;
static char ir_data[16] = { 0 };

static ir_dev_t ir_dev;

extern char tcl_ir_code[][16];

static dev_cmd_t* create_ir_cmd(ir_dev_type_t ir_dev_type, ir_dev_cmd_t ir_dev_cmd)
{
    dev_cmd_t* ir_cmd_data = &ir_cmd;


    if (ir_cmd_data->data==NULL)
    {
        ir_cmd_data->data = pvPortMalloc(IR_UART_DATA_SIZE_MAX);
    }

    memset(ir_cmd_data->data, 0, IR_UART_DATA_SIZE_MAX);
    ir_cmd_data->cmd_date_len = 0;
    ir_cmd_data->data[0] = IR_DEVICE_DATA_HEAD;
    ir_cmd_data->data[3] = IR_DEVICE_UART_ADDR;
    switch (ir_dev_cmd)
    {
        case IR_DEVICE_CMD_SET_BAUD_RATE_9600:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = 0x00;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_19200:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = 0x01;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_38400:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = 0x02;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_57600:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = 0x03;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;

        }
        break;
        case IR_DEVICE_CMD_SET_BAUD_RATE_115200:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = 0x04;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_GET_BAUD_RATE:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_GET_BUAD_RATE_BYTE;
            ir_cmd_data->data[5] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[6] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_GO_FLASH_LEARN:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_GOTO_IN_LEARN_BYTE;
            ir_cmd_data->data[5] = 0x00;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_1:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x00;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_2:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x01;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_3:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x02;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_4:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x03;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_5:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x04;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_6:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x05;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_FLASH_CODE_7:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_IN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x06;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_GO_CODE_LEARN:
        {
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_GOTO_LEARN_CODE_BYTE;
            ir_cmd_data->data[5] = 0x00;
            ir_cmd_data->data[6] = (ir_cmd_data->data[3]+  ir_cmd_data->data[4]+ ir_cmd_data->data[5])%256;
            ir_cmd_data->data[7] = IR_DEVICE_DATA_END;
        }
        break;
        case IR_DEVICE_CMD_SEND_MIDEA_CODE:
        {
            ir_dev.ac_dev = &ac_dev[ir_dev_type];
            ir_cmd_data->data[2] = 0x00;
            ir_cmd_data->data[4] = IR_CMD_SEND_LEARN_CODE_BYTE;
            uint16_t _data_sum = ir_cmd_data->data[3]+ ir_cmd_data->data[4];
            uint16_t i = 0;
            for (i = 0; i < ir_dev.ac_dev->cmd_data.cmd_date_len; i++)
            {

                ir_cmd_data->data[5+i] = ir_dev.ac_dev->cmd_data.data[i];
                _data_sum += ir_dev.ac_dev->cmd_data.data[i];
            }
            ir_cmd_data->data[i+5] = _data_sum%256;
            ir_cmd_data->data[i+6] = IR_DEVICE_DATA_END;
            ir_cmd_data->cmd_date_len = i+7;
            ir_cmd_data->data[1] = ir_cmd_data->cmd_date_len&0xff;
            ir_cmd_data->data[2] = ir_cmd_data->cmd_date_len>>8;
            goto __exit;
        }
        break;
        default:
            break;
    }
    for (size_t i = 0; ir_cmd_data->data[i]!=IR_DEVICE_DATA_END; i++)
    {
        ir_cmd_data->data[1] += 1;
    }
    ir_cmd_data->data[1] += 1;
    ir_cmd_data->cmd_date_len = ir_cmd_data->data[1];
__exit:

    // blog_info(" data numble=%d %02x%02x ", ir_cmd_data->cmd_date_len,  ir_cmd_data->data[1],  ir_cmd_data->data[2]);
    return ir_cmd_data;
}


static void hosal_uart_callback(void* p_arg)
{
    uint8_t data;

    int ch;

    while ((ch = bl_uart_data_recv(IR_UART_NUM)) >= 0) {
        data = (uint8_t)ch;
        if (data ==IR_DEVICE_DATA_END) {
            ir_uart_read_data.data[ir_uart_read_data.cmd_date_len++] = data;
            uart_get_status = true;
        }
        else if (data ==IR_DEVICE_DATA_HEAD) {

            ir_uart_read_data.cmd_date_len = 0;
            memset(ir_uart_read_data.data, 0, IR_UART_DATA_SIZE_MAX);

            ir_uart_read_data.data[ir_uart_read_data.cmd_date_len] = data;
            ir_uart_read_data.cmd_date_len++;
        }
        else {
            ir_uart_read_data.data[ir_uart_read_data.cmd_date_len] = data;
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
    if (ir_uart_read_data.data==NULL)
    {
        ir_uart_read_data.data = pvPortMalloc(IR_UART_DATA_SIZE_MAX);
    }

    while (1)
    {
        if (!uart_get_status)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        uart_get_status = false;
        blog_debug_hexdump("uart_data", (uint8_t*)ir_uart_read_data.data, ir_uart_read_data.cmd_date_len);
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
    // flash_set_ir_code("ir_on", midea[0].cmd_data, midea[0].cmd_date_len);
    xTaskCreate(ir_data_ring_task, "ir task", 1024, NULL, 8, NULL);

    ac_dev[0].param.modes = flash_get_ac_mode();
    ac_dev[1].param.modes = flash_get_ac_mode();

    ac_dev[0].param.temperature = flash_get_temperature();
    if (ac_dev[0].param.temperature ==0.0) {
        ac_dev[0].param.temperature = 25.0;
    }
    ac_dev[1].param.temperature = flash_get_temperature();
    if (ac_dev[1].param.temperature ==0.0) {
        ac_dev[1].param.temperature = 25.0;
    }
    blog_info("ir device statrt......");
}

void ir_codec_start_learn(void)
{
    dev_cmd_t* start_code = create_ir_cmd(IR_DEVICE_TYPE_NONE, IR_DEVICE_CMD_GO_CODE_LEARN);
    HAL_at_uart_send(start_code->data, start_code->cmd_date_len);
}

void ir_codec_config_set_temperature(int ac_brand_type, float temperature)
{
    ir_dev.ac_dev = &ac_dev[ac_brand_type];

    if (temperature < ir_dev.ac_dev->min_temp&& temperature>ir_dev.ac_dev->max_temp) {

        blog_error("temperature = %.2f < %.2f or >%.2f", temperature, ir_dev.ac_dev->min_temp, ir_dev.ac_dev->max_temp);
        ir_dev.ac_dev = NULL;
        return;
    }
    // ac_dev[ac_brand_type].param.temperature = temperature;
    ir_dev.ac_dev->param.temperature = temperature;

    memset(ir_data, 0, 16);

    switch (ac_brand_type)
    {
        //美的空调的数据
        case IR_DEVICE_TYPE_AC_BRAND_MIDEA:
        {
            ir_uint8_t temp_hex = 0;
            ir_uint8_t _temp_hex = (ir_uint8_t)ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp;

            temp_hex = ir_dev.ac_dev->param.temp_data[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)];
            temp_hex <<= 4;

            ir_dev.ac_dev->ir_data[4] = ir_dev.ac_dev->ir_data[4]&0x0f;
            ir_dev.ac_dev->ir_data[4] += temp_hex;
            ir_dev.ac_dev->ir_data[5] = ir_dev.ac_dev->ir_data[4]^0xff;
            blog_debug_hexdump("Midea CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
        }
        break;
        case  IR_DEVICE_TYPE_AC_BRAND_TCL:
        {
            //制冷模式下的调温
            if (ir_dev.ac_dev->param.modes==AC_MODES_COOL) {
                memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)], ir_dev.ac_dev->ir_data_len);
            }
            else if (ir_dev.ac_dev->param.modes==AC_MODES_HEAT) {
                //制热模式下的调温
                memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)+15], ir_dev.ac_dev->ir_data_len);
            }
            blog_debug_hexdump("TCL CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
        }
        break;
        default:
            break;
    }
    //创建出波形数据

    ir_dev.ac_dev->cmd_data.cmd_date_len = ir_data_encode(ir_dev.ac_dev);
    dev_cmd_t* ir_cmd_data = create_ir_cmd(ac_brand_type, IR_DEVICE_CMD_SEND_MIDEA_CODE);
    if (ir_dev.ac_dev->ac_state)
        HAL_at_uart_send(ir_cmd_data->data, ir_cmd_data->cmd_date_len);

    ir_dev.ac_dev = NULL;
}



void ir_codec_config_set_power(int ac_brand_type, int power_state)
{
    ir_dev.ac_dev = &ac_dev[ac_brand_type];
    if (!power_state) {
        ir_dev.ac_dev->ac_state = 0;
    }
    else {
        ir_dev.ac_dev->ac_state = 1;
    }

    switch (ac_brand_type)
    {
        //美的空调的数据
        case IR_DEVICE_TYPE_AC_BRAND_MIDEA:
        {
            ir_dev.ac_dev->ir_data_len = 6;
            blog_debug_hexdump("TCL CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
        }
        break;
        case IR_DEVICE_TYPE_AC_BRAND_TCL:
        {
            ir_dev.ac_dev->ir_data_len = 14;
            if (ir_dev.ac_dev->ac_state) {

                switch (ir_dev.ac_dev->param.modes)
                {
                    case AC_MODES_COOL:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_HEAT:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)+15], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_AUTO:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[30], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_FAN_ONLY:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[31], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_DRY:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[32], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    default:
                        break;
                }


                blog_debug_hexdump("TCL CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
            }
        }
        break;
        default:
            break;
    }
    //创建出波形数据
    ir_dev.ac_dev->cmd_data.cmd_date_len = ir_data_encode(ir_dev.ac_dev);
    dev_cmd_t* ir_cmd_data = create_ir_cmd(ac_brand_type, IR_DEVICE_CMD_SEND_MIDEA_CODE);
    HAL_at_uart_send(ir_cmd_data->data, ir_cmd_data->cmd_date_len);

    ir_dev.ac_dev = NULL;
}

void ir_codec_config_set_modes(int ac_brand_type, uint8_t modes_cnt)
{
    ir_dev.ac_dev = &ac_dev[ac_brand_type];
    ir_dev.ac_dev->param.modes = modes_cnt;
    if (ir_dev.ac_dev->param.modes>6) {

        blog_error("modes = %d ,There is no preset mode for this", ir_dev.ac_dev->param.modes);
        ir_dev.ac_dev = NULL;
        return;
    }
    ir_dev.ac_dev->ac_state = 1;

    switch (ac_brand_type)
    {
        case IR_DEVICE_TYPE_AC_BRAND_MIDEA:
        {
            ir_dev.ac_dev->ir_data_len = 6;
            ir_uint8_t _temp_hex = ir_dev.ac_dev->param.modes_data[ir_dev.ac_dev->param.modes];
            if (ir_dev.ac_dev->param.modes==AC_MODES_AUTO||ir_dev.ac_dev->param.modes==AC_MODES_DRY) {

                ir_dev.ac_dev->param.fan_mode = AC_FAN_MODE_FIXED;
                uint8_t fam_data = ir_dev.ac_dev->param.fan_mode_data[(uint8_t)(ir_dev.ac_dev->param.fan_mode)];
                fam_data <<= 4;
                ir_dev.ac_dev->ir_data[2] = 0x1f;
                ir_dev.ac_dev->ir_data[2] += fam_data;
                ir_dev.ac_dev->ir_data[3] = ir_dev.ac_dev->ir_data[2]^0xff;
            }
            else if (ir_dev.ac_dev->param.modes==AC_MODES_COOL||ir_dev.ac_dev->param.modes==AC_MODES_HEAT) {
                ir_dev.ac_dev->param.fan_mode = AC_FAN_MODE_AUTO;
                uint8_t fam_data = ir_dev.ac_dev->param.fan_mode_data[(uint8_t)(ir_dev.ac_dev->param.fan_mode)];
                fam_data <<= 4;
                ir_dev.ac_dev->ir_data[2] = ir_dev.ac_dev->ir_data[2]& 0x1f;
                ir_dev.ac_dev->ir_data[2] += fam_data;
                ir_dev.ac_dev->ir_data[3] = ir_dev.ac_dev->ir_data[2]^0xff;
            }

            if (ir_dev.ac_dev->param.modes==AC_MODES_FAN_ONLY)ir_dev.ac_dev->ir_data[4] = 0B11100000;
            else {

                ir_uint8_t temp = 0;
                temp = ir_dev.ac_dev->param.temp_data[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)];
                temp <<= 4;
                ir_dev.ac_dev->ir_data[4] = ir_dev.ac_dev->ir_data[4]&0x0f;
                ir_dev.ac_dev->ir_data[4] += temp;
            }
            _temp_hex <<= 2;
            ir_dev.ac_dev->ir_data[4] = ir_dev.ac_dev->ir_data[4]&0xf0;
            ir_dev.ac_dev->ir_data[4] += _temp_hex;
            ir_dev.ac_dev->ir_data[5] = ir_dev.ac_dev->ir_data[4]^0xff;
            blog_debug_hexdump("Midea CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
        }
        break;
        case IR_DEVICE_TYPE_AC_BRAND_TCL:
        {
            ir_dev.ac_dev->ir_data_len = 14;
            //制冷模式下的调温
            if (ir_dev.ac_dev->ac_state) {
                switch (ir_dev.ac_dev->param.modes)
                {
                    case AC_MODES_COOL:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_HEAT:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[(uint8_t)(ir_dev.ac_dev->param.temperature-ir_dev.ac_dev->min_temp)+15], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_AUTO:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[30], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_FAN_ONLY:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[31], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    case AC_MODES_DRY:
                    {
                        memcpy(ir_dev.ac_dev->ir_data, tcl_ir_code[32], ir_dev.ac_dev->ir_data_len);
                    }
                    break;
                    default:
                        break;
                }
                blog_debug_hexdump("TCL CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
            }
        }
        break;
        default:
            break;
    }

    //创建出波形数据
    blog_debug("ir data=%02x %02x %02x %02x %02x %02x ", ir_dev.ac_dev->ir_data[0], ir_dev.ac_dev->ir_data[1], ir_dev.ac_dev->ir_data[2], ir_dev.ac_dev->ir_data[3], ir_dev.ac_dev->ir_data[4], ir_dev.ac_dev->ir_data[5]);
    ir_dev.ac_dev->cmd_data.cmd_date_len = ir_data_encode(ir_dev.ac_dev);
    dev_cmd_t* ir_cmd_data = create_ir_cmd(ac_brand_type, IR_DEVICE_CMD_SEND_MIDEA_CODE);
    if (ir_dev.ac_dev->ac_state)
        HAL_at_uart_send(ir_cmd_data->data, ir_cmd_data->cmd_date_len);

    ir_dev.ac_dev = NULL;
}

void ir_codec_config_set_fan_modes(int ac_brand_type, uint8_t modes_cnt)
{
    if (modes_cnt>AC_FAN_MODE_MAX)
    {
        blog_error("fan modes = %d ,There is no preset mode for this", modes_cnt);
        return;
    }
    ir_dev.ac_dev = &ac_dev[ac_brand_type];
    ir_dev.ac_dev->param.fan_mode = modes_cnt;

    if (ir_dev.ac_dev->ac_state) {
        switch (ac_brand_type)
        {
            case IR_DEVICE_TYPE_AC_BRAND_MIDEA:
            {
                ir_dev.ac_dev->ir_data_len = 6;
                if (ir_dev.ac_dev->param.modes==AC_MODES_DRY||ir_dev.ac_dev->param.modes==AC_MODES_AUTO) {
                    ir_dev.ac_dev->param.fan_mode = AC_FAN_MODE_FIXED;
                    blog_error("ac mode=%d,fan_mode only =%d", ir_dev.ac_dev->param.modes, ir_dev.ac_dev->param.fan_mode);
                }
                else if (ir_dev.ac_dev->param.modes==AC_MODES_COOL||ir_dev.ac_dev->param.modes==AC_MODES_HEAT) {
                    ir_dev.ac_dev->param.fan_mode = AC_FAN_MODE_AUTO;
                    blog_error("ac mode=%d,fan_mode only =%d", ir_dev.ac_dev->param.modes, ir_dev.ac_dev->param.fan_mode);
                }
                uint8_t fam_data = ir_dev.ac_dev->param.fan_mode_data[(uint8_t)(ir_dev.ac_dev->param.fan_mode)];
                fam_data <<= 5;
                ir_dev.ac_dev->ir_data[2] = ir_dev.ac_dev->ir_data[2]& 0x1f;
                ir_dev.ac_dev->ir_data[2] += fam_data;
                ir_dev.ac_dev->ir_data[3] = ir_dev.ac_dev->ir_data[2]^0xff;
                blog_debug_hexdump("Midea CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
            }
            break;
            case IR_DEVICE_TYPE_AC_BRAND_TCL:
            {

                blog_debug_hexdump("TCL CODE", (uint8_t*)ir_dev.ac_dev->ir_data, ir_dev.ac_dev->ir_data_len);
            }
            break;
            default:
                break;
        }

        //创建出波形数据

        ir_dev.ac_dev->cmd_data.cmd_date_len = ir_data_encode(ir_dev.ac_dev);
        dev_cmd_t* ir_cmd_data = create_ir_cmd(ac_brand_type, IR_DEVICE_CMD_SEND_MIDEA_CODE);
        HAL_at_uart_send(ir_cmd_data->data, ir_cmd_data->cmd_date_len);
    }
    ir_dev.ac_dev = NULL;
}