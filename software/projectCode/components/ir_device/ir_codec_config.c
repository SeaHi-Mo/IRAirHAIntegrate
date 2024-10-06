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
#include "ir_device.h"
#define IR_DATA_SIZE_MAX 512

ac_dev_t ac_dev[] = {
    {
         .name = "Midea",
        .codec_fig = {4500,4500,550,1700,550,560,6,550,5200,1},//编码特性
         .ir_data = {0xb2,0x4d,0xbf,0x40,0x40,0xbf}, //默认开机码
        .ir_data_off = {0xb2,0x4d,0x7b,0x84,0xe0,0x1f},//关机代码
        .param.temp_data = {0,1,3,2,6,7,5,4,12,13,9,8,10,11,14},//
        .param.fan_mode = AC_FAN_MODE_AUTO,
        .param.fan_mode_data = {0B101,0B100,0B010,0B001,0b000}, //自动风，弱风，中风，强风,固定风
        .param.modes_data = {0B10,0B00,0B11,0B01,0B01},//自动模式、制冷、制热，抽湿、送风
        .param.modes = 0,//默认模式为自动
        .param.temperature = 24.0, //默认温度
        .ir_data_len = 6,   //红外数据长度
        .max_temp = 30.0,  //最大温度
        .min_temp = 17.0  //最小温度
    },
    {
        .name = "TCL",
        .codec_fig = {3100,1600,480,1000,480,374,14,0,0,0},
        .ir_data = {0xc4,0xd3,0x64,0x80,0x00,0x24,0xc0,0x10,0x1c,0x0,0x0,0x0,0x0,0x3e}, //默认开机码,制冷模式 风速自动 风向自动 温度23
        .ir_data_len = 14,
        .ir_data_off = {0xc4,0xd3,0x64,0x80,0x00,0x04,0x80,0xb0,0x1c,0x0,0x0,0x0,0x0,0xde}, //固定的关机代码
        .param.temperature = 23.0,
        .param.modes = 1,
        .max_temp = 30.0,
        .min_temp = 16.0,
    },
    { //格力空调码
        .name = "Gree",
        .codec_fig = {9000,4500,526,1660,526,572,4,580,20000,1},

        .ir_data = {0x90,0x90,0x04,0x0A,0x00,0x02,0x00,0x00},//默认开机码（不包括3位固定码），制冷模式， 风速自动 风向自动 温度25
        .ir_data_len = 8,
        .ir_data_off = {0x80,0x89,0x04,0x0A,0x00,0x02,0x00,0x01},//默认关机码
        .param.temp_data = {0b0,0b1000,0b0100,0b1100,0b0010,0b1010,0b0110,0b1110,0b0001,0b1001,0b0101,0b1101,0b0011,0b1011,0b0111},//温度数据码
        .param.fan_mode = AC_FAN_MODE_AUTO,
        .param.fan_mode_data = {0B00,0B10,0B01,0B11},
        .param.temperature = 25.0,
        .param.modes = 1,
        .param.modes_data = {0B00,0B100,0B010,0B110,0B001},
        .max_temp = 30.0,
        .max_temp = 16.0
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
# if 0
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
#endif
    return data_buff;

    }
/**
 * @brief
 *
 * @param ac_brand_type
 * @param ac_device
 * @return ir_uint16_t
*/
ir_uint16_t ir_data_encode(ir_int8_t _acType, ac_dev_t* ac_device)
{
    if (ac_device==NULL) {
        printf("[%s:%d] params is NULL\r\n", __func__, __LINE__);
        return NULL;
    }
    if (ac_device->cmd_data.data==NULL) ac_device->cmd_data.data = data_buff;
    memset(ac_device->cmd_data.data, 0, IR_DATA_SIZE_MAX);
    ac_device->cmd_data.cmd_date_len = 0;
    //引导码
    char* ir_data_buff = ac_device->ac_state?ac_device->ir_data:ac_device->ir_data_off;
    {
        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_low_time/8)%128)+128;
        ac_device->cmd_data.cmd_date_len++;
        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_low_time/8)/128;
        ac_device->cmd_data.cmd_date_len++;
        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_high_time/8)%128)+128;
        ac_device->cmd_data.cmd_date_len++;
        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_high_time/8)/128;
    }

    switch (_acType)
    {
        case IR_DEVICE_TYPE_AC_BRAND_MIDEA:
        case IR_DEVICE_TYPE_AC_BRAND_TCL:
        {
            for (size_t i = 0;i<ac_device->ir_data_len; i++)
            {
                //从高位开始识别0和1
                for (size_t j = 8; j >0; j--)
                {
                    //识别1
                    ac_device->cmd_data.cmd_date_len++;
                    //识别为1时
                    if ((ir_data_buff[i]>>(j-1)&0x01)) {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;//数据1时的电平时间
                        ac_device->cmd_data.cmd_date_len++;

                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)%128+128;//数据1时的高电平时间

                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)/128;
                    }
                    else
                    {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;
                    }
                }
            }

            if (ac_dev->codec_fig.apart_code_numble>0) {
                for (size_t i = 0; i < ac_dev->codec_fig.apart_code_numble; i++)
                {
                    ac_device->cmd_data.cmd_date_len++;

                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.apart_code_low_time/8;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.apart_code_high_time/8)%128)+128;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.apart_code_high_time/8)/128;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_low_time/8)%128)+128;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_low_time/8)/128;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_high_time/8)%128)+128;
                    ac_device->cmd_data.cmd_date_len++;
                    ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_high_time/8)/128;
                    //生成最后数据码的波形数据
                    for (size_t i = 0; i<ac_device->ir_data_len; i++)
                    {
                        for (size_t j = 8; j >0; j--)
                        {
                            //识别1 
                            ac_device->cmd_data.cmd_date_len++;
                            if ((ir_data_buff[i]>>(j-1)&0x01)) {

                                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;
                                ac_device->cmd_data.cmd_date_len++;
                                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (((ac_device->codec_fig.data_1_code_high_time)/8)%128)+128;
                                ac_device->cmd_data.cmd_date_len++;
                                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.data_1_code_high_time)/8)/128;
                            }
                            else
                            {
                                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;

                                ac_device->cmd_data.cmd_date_len++;
                                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;

                            }

                        }

                    }
                }

            }
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
            ac_device->cmd_data.cmd_date_len++;
        }
        /* code */
        break;
        case IR_DEVICE_TYPE_AC_BRAND_GREE:
        {
            //开始配置前32位数据码
            for (size_t i = 0; i < ac_device->ir_data_len/2; i++)
            {
                //从高位开始识别0和1
                for (size_t j = 8; j >0; j--)
                {
                    //识别1
                    ac_device->cmd_data.cmd_date_len++;
                    //识别为1时
                    if ((ir_data_buff[i]>>(j-1)&0x01)) {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;//数据1时的电平时间
                        ac_device->cmd_data.cmd_date_len++;

                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)%128+128;//数据1时的高电平时间

                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)/128;
                    }
                    else
                    {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;
                    }
                }
            }
            //添加后三位数据
            /* 低电平*/
            {
                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;
            }
            /* 高电平*/
            {
                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;//数据1时的电平时间
                ac_device->cmd_data.cmd_date_len++;

                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)%128+128;//数据1时的高电平时间

                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)/128;
            }
            /*低电平*/
            {
                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
                ac_device->cmd_data.cmd_date_len++;
                ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;
            }
            //连接码
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.apart_code_low_time/8;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.apart_code_high_time/8)%128)+128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.apart_code_high_time/8)/128;

            //剩下的32位数据码
            for (size_t i = 4; i < ac_device->ir_data_len; i++)
            {
                //从高位开始识别0和1
                for (size_t j = 8; j >0; j--)
                {
                    //识别1
                    ac_device->cmd_data.cmd_date_len++;
                    //识别为1时
                    if ((ir_data_buff[i]>>(j-1)&0x01)) {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;//数据1时的电平时间
                        ac_device->cmd_data.cmd_date_len++;

                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)%128+128;//数据1时的高电平时间

                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.data_1_code_high_time/8)/128;
                    }
                    else
                    {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;
                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;
                    }
                }
            }
            //尾部加上连接码
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.apart_code_low_time/8;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.apart_code_high_time/8)%128)+128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.apart_code_high_time/8)/128;
            ac_device->cmd_data.cmd_date_len++;
        }
        break;
        default:
            break;
    }

    //配置分隔码
#if 0
    if (ac_dev->codec_fig.apart_code_numble>0) {
        for (size_t i = 0; i < ac_dev->codec_fig.apart_code_numble; i++)
        {
            ac_device->cmd_data.cmd_date_len++;

            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.apart_code_low_time/8;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.apart_code_high_time/8)%128)+128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.apart_code_high_time/8)/128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_low_time/8)%128)+128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_low_time/8)/128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.guided_code_high_time/8)%128)+128;
            ac_device->cmd_data.cmd_date_len++;
            ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (ac_device->codec_fig.guided_code_high_time/8)/128;
            //生成最后数据码的波形数据
            for (size_t i = 0; i<ac_device->ir_data_len; i++)
            {
                for (size_t j = 8; j >0; j--)
                {
                    //识别1 
                    ac_device->cmd_data.cmd_date_len++;
                    if ((ir_data_buff[i]>>(j-1)&0x01)) {

                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_1_code_low_time/8;
                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = (((ac_device->codec_fig.data_1_code_high_time)/8)%128)+128;
                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ((ac_device->codec_fig.data_1_code_high_time)/8)/128;
                    }
                    else
                    {
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_low_time/8;

                        ac_device->cmd_data.cmd_date_len++;
                        ac_device->cmd_data.data[ac_device->cmd_data.cmd_date_len] = ac_device->codec_fig.data_0_code_high_time/8;

                    }

                }

            }
}

    }
#endif
    //  ac_device->cmd_data.cmd_date_len++;

    return  ac_device->cmd_data.cmd_date_len;
}


