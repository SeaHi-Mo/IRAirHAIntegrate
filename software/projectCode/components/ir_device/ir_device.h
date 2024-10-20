/**
 * @file ir_device.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-03
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef IR_DEVICE_H
#define IR_DEVICE_H


typedef struct __hxd039b_ir_msg {
    char* manufacturer;
    char ir_code_group[2];
}hxd039b_msg_t;


void ir_dvice_init(void);
void ir_codec_start_learn(void);
void hxd_039b_uart_recv_cb(unsigned char uart_data);
#endif
