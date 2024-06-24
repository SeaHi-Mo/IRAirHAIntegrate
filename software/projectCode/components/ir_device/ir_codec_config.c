/**
 * @file ir_code_config.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ir_codec_config.h"

#define IR_DATA_SIZE_MAX 256

ac_dev_t ac_dev[] = {
    {
        .codec_fig = {4500,4500,550,1700,550,560,6,550,5200,1},
        .param.temp_data = {0,1,3,2,6,7,5,4,12,13,9,8,10,11,14},
        .name = "Midea",
        .ir_data = {0xb2,0x4d,0xbf,0x40,0x40,0xbf},
        .ir_data_len = 6,
        .max_temp = 30.0,
        .min_temp = 17.0
    },
};

static volatile  char data_buff[IR_DATA_SIZE_MAX] = { 0 };
/**
 * @brief 解码函数
 *         把红外波形数据解析成红外数据码
 * @param ir_data
 * @param decode_config
 * @return char*
*/
char* ir_data_decode(char* ir_data, ir_uint16_t ir_data_len, ir_codec_cfg_t* decode_config)
{
    if (ir_data==NULL || decode_config==NULL) {
        printf("%s %dparams is NULL\r\n", __func__, __LINE__);
        return NULL;
    }
    char* data_temp = data_buff;
    memset(data_temp, 0, IR_DATA_SIZE_MAX);

    ir_uint16_t data_1_timer = 0;
    ir_uint16_t data_0_timer = 0;
    ir_uint16_t data_low_tiemr = 0;
    decode_config->guided_code_low_time = (ir_data[1]*128+(ir_data[0]-128))*8;//引导码的低电平
    decode_config->guided_code_high_time = (ir_data[3]*128+(ir_data[2]-128))*8;//引导码的高电平
    // ir_uint8_t data = 0;
    ir_uint8_t j = 0;

    for (size_t i = 4; i < ir_data_len; i++)
    {
        //识别一个周期

        data_0_timer = 0;
        //如果大小小于128，证明这是低电平
        if (ir_data[i]<128) {
            //转到下一个数据
            data_low_tiemr = ir_data[i]*8;//计算出低电平时间
            i++;
            //如果大于128证明这是数据 1
            if (ir_data[i]>128) {
                data_1_timer = 0;
                data_1_timer += ir_data[i]-128;//求出余数
                //数据下跳到商
                i++;
                data_1_timer += (ir_data[i]*128);
                data_1_timer *= 8;//最后*8
                //判断是否是分隔码或者是引导码
                if (data_1_timer >1850) {
                    break;
                }
                else {
                    j++;
                    *data_temp += 1;
                    if (j==8) {
                        j = 0;
                        data_temp++;
                        *data_temp = 0;
                    }
                    *data_temp <<= 1;
                }
            }
            else {
                //数
                data_0_timer = ir_data[i]*128*8;
                j++;
                *data_temp += 0;
                if (j==8) {
                    j = 0;
                    data_temp++;
                    *data_temp = 0;
                }
                *data_temp <<= 1;
            }
        }
    }
    return data_buff;
}
/**
 * @brief
 *
 * @param ac_brand_type
 * @param ac_device
 * @return ir_uint16_t
*/
ir_uint16_t ir_data_encode(ac_dev_t* ac_device)
{
    if (ac_device==NULL) {
        printf("[%s:%d] params is NULL\r\n", __func__, __LINE__);
        return NULL;
    }
    if (ac_device->cmd_data.data==NULL) ac_device->cmd_data.data = data_buff;
    memset(ac_device->cmd_data.data, 0, IR_DATA_SIZE_MAX);
    ir_uint16_t data_buff_len = 0;
    //引导码
    ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.guided_code_low_time/8)%128)+128;

    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.guided_code_low_time/8)/128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.guided_code_high_time/8)%128)+128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.guided_code_high_time/8)/128;
    for (size_t i = 0;i<ac_device->ir_data_len; i++)
    {

        for (size_t j = 8; j >0; j--)
        {
            //识别1
            data_buff_len++;
            if ((ac_device->ir_data[i]>>(j-1)&0x01)) {
                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_1_code_low_time/8;
                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.data_1_code_high_time/8)%128+128;

                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.data_1_code_high_time/8)/128;
            }
            else
            {
                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_0_code_low_time/8;
                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_0_code_high_time/8;
            }
        }
    }
    //配置分隔码
    data_buff_len++;

    ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.apart_code_low_time/8;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.apart_code_high_time/8)%128)+128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.apart_code_high_time/8)/128;
    //再次生成引导码波形数据
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.guided_code_low_time/8)%128)+128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.guided_code_low_time/8)/128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.guided_code_high_time/8)%128)+128;
    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = (ac_device->codec_fig.guided_code_high_time/8)/128;
    // data_buff_len++;
    //生成最后数据码的波形数据
    for (size_t i = 0; i<ac_device->ir_data_len; i++)
    {

        for (size_t j = 8; j >0; j--)
        {
            //识别1
            data_buff_len++;
            if ((ac_device->ir_data[i]>>(j-1)&0x01)) {

                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_1_code_low_time/8;
                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = (((ac_device->codec_fig.data_1_code_high_time)/8)%128)+128;
                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = ((ac_device->codec_fig.data_1_code_high_time)/8)/128;
            }
            else
            {

                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_0_code_low_time/8;

                data_buff_len++;
                ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_0_code_high_time/8;

            }

        }

    }

    data_buff_len++;
    ac_device->cmd_data.data[data_buff_len] = ac_device->codec_fig.data_0_code_low_time/8;
    data_buff_len++;

    return data_buff_len;
}


