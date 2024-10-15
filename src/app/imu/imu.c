/*
 * @Author: wang,yongjing
 * @Date: 2024-10-15 09:20:29
 * @LastEditTime: 2024-10-15 14:39:15
 * @LastEditors: wang,yongjing
 * @Description:
 * @FilePath: /temperature-control/FlexiAssistGlove/src/app/imu/imu.c
 *
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread_stack.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include "imu.h"

static struct k_thread imu_handle_thread;
static K_KERNEL_STACK_MEMBER(imu_handle_stack, IMU_STACK_SIZE);
static const struct device *imu_arm = DEVICE_DT_GET(DT_NODELABEL(imu_arm));
static const struct device *imu_palm = DEVICE_DT_GET(DT_NODELABEL(imu_palm));

int get_imu_value(const struct device *dev, imu_data *value)
{
	struct sensor_value temperature;
	struct sensor_value accel[3];
	struct sensor_value gyro[3];
	int rc = sensor_sample_fetch(dev);

	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
	}
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro);
	}
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP, &temperature);
	}
	if (rc == 0) {
		value->acce_x = sensor_value_to_double(&accel[0]);
		value->acce_y = sensor_value_to_double(&accel[1]);
		value->acce_z = sensor_value_to_double(&accel[2]);
		value->gyro_x = sensor_value_to_double(&gyro[0]);
		value->gyro_y = sensor_value_to_double(&gyro[1]);
		value->gyro_z = sensor_value_to_double(&gyro[2]);
		value->temp = sensor_value_to_double(&temperature);
	} else {
		printf("sample fetch/get failed: %d\n", rc);
	}
	return 0;
}
int imu_arm_filter(imu_data arm_raw_value, imu_data *arm_value)
{
	*arm_value = arm_raw_value;
	return 0;
}
int imu_palm_filter(imu_data palm_raw_value, imu_data *palm_value)
{
	*palm_value = palm_raw_value;
	return 0;
}
static void imu_handle(void *arug0, void *arug1, void *arug2)
{
	printf("imu handle start!\n");

	if (!device_is_ready(imu_arm) || !device_is_ready(imu_palm)) {
		printf("imu device not ready\n");
		return;
	}

	imu_data imu_arm_raw_value = {0}, imu_palm_raw_value = {0};
	imu_data imu_arm_value = {0}, imu_palm_value = {0};
	int ret;

	while (1) {
		ret = get_imu_value(imu_arm, &imu_arm_raw_value);
		ret = get_imu_value(imu_palm, &imu_palm_raw_value);
		if (ret) {
			printf("get imu value failed!\n");
			continue;
		}

		/* user custom fileter here, If the filtering algorithms of the two IMUs are
		 * consistent, the same function can be used */
		imu_arm_filter(imu_arm_raw_value, &imu_arm_value);
		imu_palm_filter(imu_palm_raw_value, &imu_palm_value);

		printf("arm: acce: %f %f %f gyro: %f %f %f temp: %f\n", imu_arm_value.acce_x,
		       imu_arm_value.acce_y, imu_arm_value.acce_z, imu_arm_value.gyro_x,
		       imu_arm_value.gyro_y, imu_arm_value.gyro_z, imu_arm_value.temp);
		printf("palm: acce: %f %f %f gyro: %f %f %f temp: %f\n", imu_palm_value.acce_x,
		       imu_palm_value.acce_y, imu_palm_value.acce_z, imu_palm_value.gyro_x,
		       imu_palm_value.gyro_y, imu_palm_value.gyro_z, imu_palm_value.temp);
	}
}
void imu_init()
{
	k_thread_create(&imu_handle_thread, imu_handle_stack,
			K_THREAD_STACK_SIZEOF(imu_handle_stack), imu_handle, NULL, NULL, NULL,
			CONFIG_MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
}