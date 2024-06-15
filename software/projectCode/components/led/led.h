/**
 * @file led.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
*/
#ifndef LED_H
#define LED_H

#define NET_WORKER_LED 12

typedef enum {
    LED_STATE_NONE = -1,
    LED_STATE_BLINK_0_5,
    LED_STATE_BLINK_1_0,
    LED_STATE_BLINK_1_5,
    LED_STATE_BLINK_2_0,
    LED_STATE_BLINK_2_0_S_0_5,
}led_state_t;
void device_led_init(void);
void device_led_update_state(led_state_t led_ste);
#endif