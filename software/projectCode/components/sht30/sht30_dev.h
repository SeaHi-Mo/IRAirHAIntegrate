/**
 * @file sht30_dev.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef SHT30_DEV_H
#define SHT30_DEV_H


#define SHT30_I2C_SCL 14
#define SHT30_I2C_SDA 17
#define SHT30_I2C_ADDR 0x0044
/**
 * @brief SHT30 采样参数设置
 *
*/
typedef enum {
    SHT30_SINGLE_SAMPLE_CLOK_HIGH = 0x2C06,
    SHT30_SINGLE_SAMPLE_CLOK_MEDIUM = 0x2C0D,
    SHT30_SINGLE_SAMPLE_CLOK_LOW = 0x2C0D,

    SHT30_SINGLE_SAMPLE_NOCLOK_HIGH = 0x2400,
    SHT30_SINGLE_SAMPLE_NOCLOK_MEDIUM = 0x240B,
    SHT30_SINGLE_SAMPLE_NOCLOK_LOW = 0x2416,

    SHT30_PERIODIC_SAMPLE_0_5_HIGH = 0x2032,
    SHT30_PERIODIC_SAMPLE_0_5_MEDIUM = 0x2024,
    SHT30_PERIODIC_SAMPLE_0_5_LOW = 0x202F,

    SHT30_PERIODIC_SAMPLE_1_HIGH = 0x2130,
    SHT30_PERIODIC_SAMPLE_1_MEDIUM = 0x2126,
    SHT30_PERIODIC_SAMPLE_1_LOW = 0x212D,

    SHT30_PERIODIC_SAMPLE_2_HIGH = 0x2236,
    SHT30_PERIODIC_SAMPLE_2_MEDIUM = 0x2220,
    SHT30_PERIODIC_SAMPLE_2_LOW = 0x222B,

    SHT30_PERIODIC_SAMPLE_4_HIGH = 0x2334,
    SHT30_PERIODIC_SAMPLE_4_MEDIUM = 0x2322,
    SHT30_PERIODIC_SAMPLE_4_LOW = 0x2329,

    SHT30_PERIODIC_SAMPLE_10_HIGH = 0x2737,
    SHT30_PERIODIC_SAMPLE_10_MEDIUM = 0x2721,
    SHT30_PERIODIC_SAMPLE_10_LOW = 0x272A,
}sht03_sample_t;

typedef struct
{
    double temperature;
    int humidity;
}sht3x_data_t;

typedef void(*sht30_get_data_cb_t)(sht3x_data_t*);

void sht30_device_init(sht03_sample_t sample_config, sht30_get_data_cb_t sht30_get_data_cb);
sht3x_data_t* sht30_get_data(void);
#endif