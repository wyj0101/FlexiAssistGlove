/*
 * @Author: wang,yongjing
 * @Date: 2024-10-15 09:20:29
 * @LastEditTime: 2024-10-15 14:34:39
 * @LastEditors: wang,yongjing
 * @Description:
 * @FilePath: /temperature-control/FlexiAssistGlove/src/app/sensor/air_sensor.c
 *
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel/thread_stack.h>
#include "air_sensor.h"
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#ifdef CONFIG_LOG
#include <zephyr/logging/log.h>

/*	1- error
 *	2- warning
 *	3- info	(default)
 *	4- debug
 */
LOG_MODULE_REGISTER(sensor, 4);
#endif

static struct k_thread sensor_handle_thread;
static K_KERNEL_STACK_MEMBER(sensor_handle_stack, SENSOR_STACK_SIZE);

const struct device *const spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi2));
const struct device *const usart_dev = DEVICE_DT_GET(DT_NODELABEL(usart1));

struct gpio_dt_spec sensor_cs[SENSOR_COUNT] = {
	SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(sensor0)),
	SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(sensor1)),
	SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(sensor2)),
	SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(sensor3)),
};
static struct spi_config sensor_cfg[SENSOR_COUNT] = {0};

static uint8_t start_cmd[] = {0xaa, 0x00, 0x00};
static uint8_t read_cmd[] = {0x00, 0x00, 0x00, 0x00};

static uint8_t value_buff[4] = {0};

static const struct spi_buf start_cmd_buff = {
	.buf = start_cmd,
	.len = sizeof(start_cmd),
};

static const struct spi_buf read_cmd_buff = {
	.buf = read_cmd,
	.len = sizeof(read_cmd),
};

static struct spi_buf read_value_buff = {
	.buf = value_buff,
	.len = sizeof(value_buff),
};

const static struct spi_buf_set start_cmd_set = {
	.buffers = &start_cmd_buff,
	.count = 1,
};

const static struct spi_buf_set read_cmd_set = {
	.buffers = &read_cmd_buff,
	.count = 1,
};

const static struct spi_buf_set read_value_set = {
	.buffers = &read_value_buff,
	.count = 1,
};

static float value = 0;
float sensor_value[SENSOR_COUNT] = {0};
/* Please manually calibrate the sensor here */
/* sensor_real_value = raw_value - sensor_adjust*/
static const float sensor_adjust[SENSOR_COUNT] = {
	0,
	0,
	0,
	0,
};
static int sensor_config_init(void)
{
	int i;
	for (i = 0; i < SENSOR_COUNT; i++) {
		sensor_cfg[i].frequency = SPI_FREQUENCY;
		sensor_cfg[i].operation = SPI_OPERTION;
		sensor_cfg[i].cs.gpio = sensor_cs[i];
	}
	return 0;
}

void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(usart_dev, buf[i]);
	}
	uart_poll_out(usart_dev, '\r');
	uart_poll_out(usart_dev, '\n');
}
static void sensor_handle(void *arug0, void *arug1, void *arug2)
{
	int i;
	uint8_t data_buff[150] = {0};
#ifdef CONFIG_LOG
	LOG_INF("sensor handle start!");
#else
	printf("air sensor handle start!\n");
#endif
	sensor_config_init();

	if (!device_is_ready(spi_dev)) {
		printf("spi device not ready\n");
		return;
	}

	while (1) {
		for (i = 0; i < SENSOR_COUNT; i++) {
			if (spi_write(spi_dev, &sensor_cfg[i], &start_cmd_set) != 0) {
#ifdef CONFIG_LOG
				LOG_ERR("Spi Write Failed! device:%d\n", i);
#else
				printf("Spi Write Failed! device:%d\n", i);
#endif
				continue;
			}
		}

		k_usleep(7500);

		for (i = 0; i < SENSOR_COUNT; i++) {
			if (spi_transceive(spi_dev, &sensor_cfg[i], &read_cmd_set,
					   &read_value_set) != 0) {
#ifdef CONFIG_LOG
				LOG_ERR("Spi Read Failed! device:%d\n", i);
#else
				printf("Spi Read Failed! device:%d\n", i);
#endif
				continue;
			}
			value = (value_buff[1] << 16) | (value_buff[2] << 8) | value_buff[3];
			sensor_value[i] = ((((value - 0x800000) * 0xc8) / 0xb33333) * 1000) -
					  sensor_adjust[i];
		}
		memset(data_buff, 0, sizeof(data_buff));
		sprintf(data_buff, "P:%.2f,%.2f,%.2f,%.2f", sensor_value[0], sensor_value[1],
			sensor_value[2], sensor_value[3]);
		print_uart(data_buff);
	}
}
void air_sensor_init()
{
	k_thread_create(&sensor_handle_thread, sensor_handle_stack,
			K_THREAD_STACK_SIZEOF(sensor_handle_stack), sensor_handle, NULL, NULL, NULL,
			CONFIG_MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
}