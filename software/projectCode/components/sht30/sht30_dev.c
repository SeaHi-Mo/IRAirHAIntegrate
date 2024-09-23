/**
 * @file sht30_dev.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <bl_gpio.h>
#include <blog.h>
#include <device_state.h>
#include "sht30_dev.h"
#include <hosal_i2c.h>


static bool sht3x_is_single_sample = false;
static xTimerHandle sht30_timer;
static sht3x_data_t sht30_data;
static sht03_sample_t sht30_sample;
static sht30_get_data_cb_t sht30_cb;
#pragma pack(1)
struct sht3x_data
{
    uint8_t st_high;
    uint8_t st_low;
    uint8_t st_crc8;
    uint8_t srh_high;
    uint8_t srh_low;
    uint8_t srh_crc8;
};
static  struct sht3x_data data;
#pragma pack()

static hosal_i2c_dev_t sht_i2c0 = {
        .config = {
            .address_width = HOSAL_I2C_ADDRESS_WIDTH_7BIT,
            .freq = 400000,
            .mode = HOSAL_I2C_MODE_MASTER,
            .scl = SHT30_I2C_SCL,
            .sda = SHT30_I2C_SDA,
        },
        .port = 0,
};


static uint8_t crc8(uint8_t* data, int len)
{
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;
    for (int j = len; j; --j)
    {
        crc ^= *data++;
        for (int i = 8; i; --i)
        {
            crc = (crc & 0x80)
                ? (crc << 1) ^ POLYNOMIAL
                : (crc << 1);
        }
    }
    return crc;
}
static  void vTimerCallback_sht30(TimerHandle_t xTimer)
{
    hosal_i2c_master_recv(&sht_i2c0, SHT30_I2C_ADDR, (uint8_t*)&data, sizeof data, 100);
    if (crc8(&data.st_high, 2) == data.st_crc8) {
        uint16_t st = data.st_high;
        st <<= 8;
        st |= data.st_low;
        sht30_data.temperature = -45.0+175*(st/(0xffff-1.0));
    }

    if (crc8(&data.srh_high, 2) == data.srh_crc8) {
        uint16_t srh = data.srh_high;
        srh <<= 8;
        srh |= data.srh_low;
        double humi = 100.0*(srh/(0xffff-1.0));
        sht30_data.humidity = (int)humi;

    }
    blog_debug("temperature: %.2f humidity: %d\r\n", sht30_data.temperature, sht30_data.humidity);
    sht30_cb(&sht30_data);
}

void sht30_device_init(sht03_sample_t sample_config, sht30_get_data_cb_t sht30_get_data_cb)
{
    hosal_i2c_init(&sht_i2c0);
    sht30_sample = sample_config;
    sht30_cb = sht30_get_data_cb;
    uint16_t _timers = 0;
    switch (sample_config)
    {
        case SHT30_PERIODIC_SAMPLE_0_5_HIGH:
        case SHT30_PERIODIC_SAMPLE_0_5_MEDIUM:
        case SHT30_PERIODIC_SAMPLE_0_5_LOW:
            _timers = 500;
            sht3x_is_single_sample = false;
            break;
        case SHT30_PERIODIC_SAMPLE_1_HIGH:
        case SHT30_PERIODIC_SAMPLE_1_MEDIUM:
        case SHT30_PERIODIC_SAMPLE_1_LOW:
            sht3x_is_single_sample = false;
            _timers = 1000;
            break;
        case SHT30_PERIODIC_SAMPLE_2_HIGH:
        case SHT30_PERIODIC_SAMPLE_2_MEDIUM:
        case SHT30_PERIODIC_SAMPLE_2_LOW:
            sht3x_is_single_sample = false;
            _timers = 2000;
            break;
        case SHT30_PERIODIC_SAMPLE_4_HIGH:
        case SHT30_PERIODIC_SAMPLE_4_MEDIUM:
        case SHT30_PERIODIC_SAMPLE_4_LOW:
            sht3x_is_single_sample = false;
            _timers = 4000;
            break;
        case SHT30_PERIODIC_SAMPLE_10_HIGH:
        case SHT30_PERIODIC_SAMPLE_10_MEDIUM:
        case SHT30_PERIODIC_SAMPLE_10_LOW:
            sht3x_is_single_sample = false;
            _timers = 10000;
            break;

        default:
            sht3x_is_single_sample = true;
            return;
            break;
    }
    sht30_timer = xTimerCreate("sht30 getdata", pdMS_TO_TICKS(_timers), sht3x_is_single_sample?pdFALSE:pdTRUE, 0, vTimerCallback_sht30);
    uint8_t command[2] = { sample_config >>8, sample_config&0xff };
    hosal_i2c_master_send(&sht_i2c0, SHT30_I2C_ADDR, command, sizeof command, 100);
    vTaskDelay(pdMS_TO_TICKS(50));
    xTimerStart(sht30_timer, pdMS_TO_TICKS(100));

}

sht3x_data_t* sht30_get_data(void)
{
    if (sht3x_is_single_sample)
    {
        uint8_t command[2] = { sht30_sample >> 8,  sht30_sample & 0xff };
        blog_debug("i2c cmd=0x%02x%02x", command[0], command[1]);
        hosal_i2c_master_send(&sht_i2c0, SHT30_I2C_ADDR, command, sizeof command, 100);
        memset(&data, 0, sizeof data);
        vTaskDelay(pdMS_TO_TICKS(50));
        hosal_i2c_master_recv(&sht_i2c0, SHT30_I2C_ADDR, (uint8_t*)&data, sizeof data, 100);
        if (crc8(&data.st_high, 2) == data.st_crc8) {
            uint16_t st = data.st_high;
            st <<= 8;
            st |= data.st_low;

            sht30_data.temperature = -45.0+175*(st/(0xffff-1.0));
        }
        if (crc8(&data.srh_high, 2) == data.srh_crc8) {
            uint16_t srh = data.srh_high;
            srh <<= 8;
            srh |= data.srh_low;
            double humi = 100.0*(srh/(0xffff-1.0));
            sht30_data.humidity = (int)humi;
        }
        blog_debug("temperature: %.2f humidity: %d\r\n", sht30_data.temperature, sht30_data.humidity);
    }
    else {
        return &sht30_data;
    }
    return &sht30_data;
}