/**
 * @file ir_uart.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef IR_UART_H
#define IR_UART_H
#include "bl_uart.h"

#define IR_UART_TX 22
#define IR_UART_RX 20
#define IR_UART_NUM 1
typedef void(*ir_recv_cb_t)(unsigned char);
void ir_uart_dvice_init(ir_recv_cb_t _ir_recv_cb);
#endif