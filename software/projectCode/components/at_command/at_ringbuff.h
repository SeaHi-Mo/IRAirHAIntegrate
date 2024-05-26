/*
 * Tencent is pleased to support the open source community by making IoT Hub
 available.
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file
 except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software
 distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND,
 * either express or implied. See the License for the specific language
 governing permissions and
 * limitations under the License.
 *
 */

#ifndef __AT_RING_BUFF_H__
#define __AT_RING_BUFF_H__

#include "stdbool.h"
#include "stdint.h"

#define RINGBUFF_OK        0  /* No error, everything OK. */
#define RINGBUFF_ERR       -1 /* Out of memory error.     */
#define RINGBUFF_EMPTY     -3 /* Timeout.                	    */
#define RINGBUFF_FULL      -4 /* Routing problem.          */
#define RINGBUFF_TOO_SHORT -5

typedef struct _ring_buff_ {
    uint32_t size;
    uint32_t readpoint;
    uint32_t writepoint;
    char* buffer;
    bool     full;
} sRingbuff;

typedef sRingbuff* ring_buff_t;
/**
 * @brief 环形缓冲区初始化
 *
 * @param ring_buff 缓冲区句柄
 * @param buff 缓冲区地址
 * @param size 缓冲区大小
 * @return int
*/
int ring_buff_init(sRingbuff* ring_buff, char* buff, uint32_t size);
/**
 * @brief 清空缓冲区
 *
 * @param ring_buff 缓冲区句柄
 * @return int
*/
int ring_buff_flush(sRingbuff* ring_buff);
/**
 * @brief 输出写入缓冲区
 *
 * @param ring_buff 缓冲区句柄
 * @param pData 要写入的数据
 * @param len  数据长度
 * @return int
*/
int ring_buff_push_data(sRingbuff* ring_buff, uint8_t* pData, int len);
/**
 * @brief 读取缓冲区数据
 *
 * @param ring_buff 缓冲区句柄
 * @param pData 读取的数据存放地址
 * @param len 需要读取的长度
 * @return int
*/
int ring_buff_pop_data(sRingbuff* ring_buff, uint8_t* pData, int len);
/**
 * @brief 读取缓冲区内的数据大小
 *
 * @param ring_buff 缓冲区句柄
 * @return unsigned int
*/
unsigned int ring_buff_get_size(sRingbuff* ring_buff);
#endif  // __ringbuff_h__
