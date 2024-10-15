#include <stdio.h>

#include <zephyr/kernel.h>

#include "air_sensor.h"
#include "imu.h"

int main(void)
{
	printf("main start !\n");

	imu_init();
	air_sensor_init();

	return 0;
}