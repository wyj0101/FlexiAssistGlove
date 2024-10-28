/*
 * @Author: wang,yongjing
 * @Date: 2024-10-16 16:46:58
 * @LastEditTime: 2024-10-28 11:07:49
 * @LastEditors: wang,yongjing
 * @Description:
 * @FilePath: /temperature-control/FlexiAssistGlove/src/app/dev/pwm.c
 *
 */
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread_stack.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

#include "stm32_ll_gpio.h"
#include "pwm.h"

const struct device *pump_dev = DEVICE_DT_GET(DT_NODELABEL(pump_pwm));
static const struct device *gpioa_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa));
static const struct device *gpiob_dev = DEVICE_DT_GET(DT_NODELABEL(gpiob));

static int count = 0;
static uint8_t valve_v3, valve_v4, valve_v5, valve_v6, valve_p1, valve_p2;

int valve_set_period(enum valve_channel channel, uint8_t period)
{
	if (period > 100) {
		period = 100;
	}

	switch (channel) {
	case VALVE_P1:
		valve_p1 = period;
		break;
	case VALVE_P2:
		valve_p2 = period;
		break;
	case VALVE_V3:
		valve_v3 = period;
		break;
	case VALVE_V4:
		valve_v4 = period;
		break;
	case VALVE_V5:
		valve_v5 = period;
		break;
	case VALVE_V6:
		valve_v6 = period;
		break;
	default:
		printf("Invalid valve channel\n");
		return -1;
	}
	return 0;
}
static void valve_timer_handler(struct k_timer *timer)
{
	count++;

	gpio_pin_set(gpioa_dev, 6, (count >= valve_p1 ? 1 : 0));
	gpio_pin_set(gpioa_dev, 7, (count >= valve_p2 ? 1 : 0));
	gpio_pin_set(gpiob_dev, 7, (count >= valve_v3 ? 1 : 0));
	gpio_pin_set(gpiob_dev, 6, (count >= valve_v4 ? 1 : 0));
	gpio_pin_set(gpiob_dev, 5, (count >= valve_v5 ? 1 : 0));
	gpio_pin_set(gpioa_dev, 15, (count >= valve_v6 ? 1 : 0));

	if (count >= 100) {
		count = 0;
	}
}
K_TIMER_DEFINE(valve_timer, valve_timer_handler, NULL);

static void valve_gpio_init(void)
{
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	gpio_pin_configure(gpioa_dev, 7, GPIO_OUTPUT);
	gpio_pin_configure(gpioa_dev, 6, GPIO_OUTPUT);
	gpio_pin_configure(gpioa_dev, 15, GPIO_OUTPUT);
	gpio_pin_configure(gpiob_dev, 5, GPIO_OUTPUT);
	gpio_pin_configure(gpiob_dev, 6, GPIO_OUTPUT);
	gpio_pin_configure(gpiob_dev, 7, GPIO_OUTPUT);
}
int pwm_device_init()
{
	if (!device_is_ready(pump_dev)) {
		return -1;
	}
	valve_gpio_init();
	k_timer_start(&valve_timer, K_MSEC(VALVE_PERIOD / 100), K_MSEC(VALVE_PERIOD / 100));
	return 0;
}
int pwm_set_period(enum pump_channel channel, uint32_t period)
{
	switch (channel) {
	case PUMP_V1:
		pwm_set_cycles(pump_dev, 4, PUMP_PERIOD, period * PUMP_PERIOD, 0);
		break;
	case PUMP_V2:
		pwm_set_cycles(pump_dev, 3, PUMP_PERIOD, period * PUMP_PERIOD, 0);
		break;
	}
	return 0;
}