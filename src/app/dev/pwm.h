/***
 * @Author: wang,yongjing
 * @Date: 2024-05-31 09:26:25
 * @LastEditTime: 2024-05-31 15:16:21
 * @LastEditors: wang,yongjing
 * @Description:
 * @FilePath:
 * @
 */
#pragma once

#define PUMP_PERIOD 7200

#define VALVE_PERIOD 1000

enum pump_channel {
	PUMP_V1,
	PUMP_V2
};
enum valve_channel {
	VALVE_V3,
	VALVE_V4,
	VALVE_V5,
	VALVE_V6,
	VALVE_P1,
	VALVE_P2,
};

int pwm_device_init(void);
int pwm_set_period(enum pump_channel channel, uint32_t period);
int valve_set_period(enum valve_channel channel, uint8_t period);