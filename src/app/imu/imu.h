/***
 * @Author: yjwang@kunluntech.com.cn
 * @Date: 2024-04-17 13:50:21
 * @LastEditTime: 2024-04-17 13:50:21
 * @LastEditors: yjwang@kunluntech.com.cn
 * @Description:
 * @FilePath: /temperature-control/pneumatic-control-device/sensor/sensor.h
 * @
 */
#pragma once

#define IMU_STACK_SIZE 768

typedef struct imu_data {
	double acce_x;
	double acce_y;
	double acce_z;
	double gyro_x;
	double gyro_y;
	double gyro_z;
	double temp;
} imu_data;

void imu_init();