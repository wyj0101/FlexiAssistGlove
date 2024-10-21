#include <stdio.h>

#include <zephyr/kernel.h>

#include "air_sensor.h"
#include "imu.h"
#include "pwm.h"
#include "stm32_ll_gpio.h"

int main(void)
{
	printf("main start !\n");
	__HAL_AFIO_REMAP_SWJ_NOJTAG();

	pwm_device_init();
	// imu_init();
	//air_sensor_init();

	return 0;
}