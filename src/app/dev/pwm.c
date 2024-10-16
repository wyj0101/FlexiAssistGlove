/*
 * @Author: wang,yongjing
 * @Date: 2024-10-16 16:46:58
 * @LastEditTime: 2024-10-16 19:31:55
 * @LastEditors: wang,yongjing
 * @Description:
 * @FilePath: /temperature-control/FlexiAssistGlove/src/app/dev/pwm.c
 *
 */
/*
 * @Author: wang,yongjing
 * @Date: 2024-05-31 09:26:25
 * @LastEditTime: 2024-05-31 15:19:11
 * @LastEditors: wang,yongjing
 * @Description: motor control file
 * @FilePath: /temperature-control/pneumatic-control-device/src/lib/motor/motor.c
 *
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread_stack.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include "pwm.h"

/*	1- error
 *	2- warning
 *	3- info	(default)
 *	4- debug
 */
LOG_MODULE_REGISTER(motor, 4);

const struct device *pwm_dev1 = DEVICE_DT_GET(DT_NODELABEL(pwm1));
const struct device *pwm_dev2 = DEVICE_DT_GET(DT_NODELABEL(pwm2));
const struct device *pwm_dev3 = DEVICE_DT_GET(DT_NODELABEL(pwm3));
const struct device *pwm_dev4 = DEVICE_DT_GET(DT_NODELABEL(pwm4));

// const struct device *pwm_dev2 = DEVICE_DT_GET(DT_NODELABEL(pwm2));
// const struct device *pwm_dev3 = DEVICE_DT_GET(DT_NODELABEL(pwm3));
// const struct device *pwm_dev7 = DEVICE_DT_GET(DT_NODELABEL(pwm7));

int pwm_device_init()
{
	if (!device_is_ready(pwm_dev1)) {
		LOG_ERR("pwm0 device is not ready\n");
		return -1;
	}

	pwm_set_cycles(pwm_dev1, 1, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev2, 1, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev3, 1, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev3, 2, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev4, 1, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev4, 2, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev4, 3, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);
	pwm_set_cycles(pwm_dev4, 4, FAN_PERIOD, 0.2 * FAN_PERIOD, 0);

	return 0;
}