/**
 * @file at_receive.h
 * @author seahi-mo (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef __AIIO_AT_RECEIVE_H_
#define __AIIO_AT_RECEIVE_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define AT_OK "\r\nATCMD:OK\r\n"
#define AT_ERR "\r\nATCMD:ERR\r\n"

typedef void (*cmd_callback_t)(char* cmd, uint16_t cmd_len);

typedef struct
{
    char* cmd;
    cmd_callback_t set_func;
    cmd_callback_t check_func;
    cmd_callback_t  test_func;
} at_cmd_info_t;


#define AT_RESPONSE(x)              at_cmd_response(x)
/**
 * @brief
 *
 * @param baud_rate
*/
void atUartInit(uint32_t baud_rate);

/**
 * @brief   This function is mainly processing AT command
 *
 * @param[in]   buff     Receiving AT command
 *
 */
void atCommandExecute(char* buff);

/**
 * @brief   Response AT command
 *
 * @param[in]   cmd    AT command
 *
 */
void at_cmd_response(char* cmd);



#endif
