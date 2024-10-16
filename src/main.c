#include <stdio.h>

#include <zephyr/kernel.h>

#include "air_sensor.h"
#include "imu.h"
#include "pwm.h"

int main(void)
{
	printf("main start !\n");

	pwm_device_init();
	// imu_init();
	// air_sensor_init();

	return 0;
}