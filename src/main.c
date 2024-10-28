#include <stdio.h>

#include <zephyr/kernel.h>

#include "air_sensor.h"
#include "imu.h"
#include "pwm.h"

int main(void)
{

	pwm_device_init();
	air_sensor_init();
	// imu_init();

	return 0;
}