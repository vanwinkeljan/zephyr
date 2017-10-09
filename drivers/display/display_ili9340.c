/* Copyright (c) 2017 dXplore
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "display_ili9340.h"
#include <display.h>

#define SYS_LOG_DOMAIN "ILI9340"
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_ILI9340_LEVEL
#include <logging/sys_log.h>

#include <gpio.h>
#include <misc/byteorder.h>
#include <spi.h>

struct ili9340_data {
	struct device *reset_gpio;
	struct device *command_data_gpio;
	struct spi_config spi_config;
};

#define ILI9340_CMD_DATA_PIN_COMMAND 0
#define ILI9340_CMD_DATA_PIN_DATA 1

static void ili9340_exit_sleep(struct ili9340_data *data);
static void ili9340_set_mem_area(struct ili9340_data *data, const u16_t x,
				 const u16_t y, const u16_t w, const u16_t h);
static int ili9340_init(struct device *dev);
static int ili9340_write_bitmap(const struct device *dev, const u16_t x,
				const u16_t y, const u16_t w, const u16_t h,
				const u8_t *rgb_data);
static int ili9340_display_on(struct device *dev);
static int ili9340_display_off(struct device *dev);

static const struct display_driver_api ili9340_api = {
	.display_on = ili9340_display_on,
	.display_off = ili9340_display_off,
	.write_bitmap = ili9340_write_bitmap
};

int ili9340_init(struct device *dev)
{
	struct ili9340_data *data = (struct ili9340_data *)dev->driver_data;

	SYS_LOG_DBG("Initializing display driver");

	data->spi_config.dev = device_get_binding(CONFIG_ILI9340_SPI_DEV_NAME);
	if (data->spi_config.dev == NULL) {
		SYS_LOG_ERR("Could not get SPI device for ILI9340");
		return -EPERM;
	}

	/* max (write) speed 15MHz (66ns) */
	data->spi_config.frequency = 15151515;
	data->spi_config.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8);
	data->spi_config.slave = CONFIG_ILI9340_SPI_SLAVE_NUMBER;
	data->spi_config.cs = NULL;

	data->reset_gpio =
	    device_get_binding(CONFIG_ILI9340_RESET_GPIO_PORT_NAME);
	if (data->reset_gpio == NULL) {
		SYS_LOG_ERR("Could not get GPIO port for ILI9340 reset");
		return -EPERM;
	}

	gpio_pin_configure(data->reset_gpio, CONFIG_ILI9340_RESET_PIN,
			   GPIO_DIR_OUT);

	data->command_data_gpio =
	    device_get_binding(CONFIG_ILI9340_CMD_DATA_GPIO_PORT_NAME);
	if (data->command_data_gpio == NULL) {
		SYS_LOG_ERR("Could not get GPIO port for ILI9340 command/data");
		return -EPERM;
	}

	gpio_pin_configure(data->command_data_gpio, CONFIG_ILI9340_CMD_DATA_PIN,
			   GPIO_DIR_OUT);

	SYS_LOG_DBG("Resetting display driver");
	gpio_pin_write(data->reset_gpio, CONFIG_ILI9340_RESET_PIN, 1);
	k_sleep(1);
	gpio_pin_write(data->reset_gpio, CONFIG_ILI9340_RESET_PIN, 0);
	k_sleep(1);
	gpio_pin_write(data->reset_gpio, CONFIG_ILI9340_RESET_PIN, 1);
	k_sleep(5);

	SYS_LOG_DBG("Initializing LCD");
	ili9340_lcd_init(data);

	SYS_LOG_DBG("Exiting sleep mode");
	ili9340_exit_sleep(data);

	return 0;
}

int ili9340_write_bitmap(const struct device *dev, const u16_t x,
			  const u16_t y, const u16_t w, const u16_t h,
			  const u8_t *rgb_data)
{
	struct ili9340_data *data = (struct ili9340_data *)dev->driver_data;

	SYS_LOG_DBG("Writing %dx%d (w,h) bitmap @ %dx%d (x,y)", w, h, x, y);
	ili9340_set_mem_area(data, x, y, w, h);
	ili9340_transmit(data, ILI9340_CMD_MEM_WRITE, (void *)rgb_data,
			 3 * w * h);
	return 0;
}

int ili9340_display_on(struct device *dev)
{
	struct ili9340_data *data = (struct ili9340_data *)dev->driver_data;

	SYS_LOG_DBG("Turning display on");
	ili9340_transmit(data, ILI9340_CMD_DISPLAY_ON, NULL, 0);
	return 0;
}

int ili9340_display_off(struct device *dev)
{
	struct ili9340_data *data = (struct ili9340_data *)dev->driver_data;

	SYS_LOG_DBG("Turning display off");
	ili9340_transmit(data, ILI9340_CMD_DISPLAY_OFF, NULL, 0);
	return 0;
}

void ili9340_transmit(struct ili9340_data *data, u8_t cmd, void *tx_data,
		      size_t tx_len)
{
	struct spi_buf tx_buf = {.buf = &cmd, .len = 1};

	gpio_pin_write(data->command_data_gpio, CONFIG_ILI9340_CMD_DATA_PIN,
		       ILI9340_CMD_DATA_PIN_COMMAND);
	spi_write(&data->spi_config, &tx_buf, 1);

	if (tx_data != NULL) {
		tx_buf.buf = tx_data;
		tx_buf.len = tx_len;
		gpio_pin_write(data->command_data_gpio,
			       CONFIG_ILI9340_CMD_DATA_PIN,
			       ILI9340_CMD_DATA_PIN_DATA);
		spi_transceive(&data->spi_config, &tx_buf, 1, NULL, 0);
	}
}

void ili9340_exit_sleep(struct ili9340_data *data)
{
	ili9340_transmit(data, ILI9340_CMD_EXIT_SLEEP, NULL, 0);
	k_sleep(120);
}

void ili9340_set_mem_area(struct ili9340_data *data, const u16_t x,
			  const u16_t y, const u16_t w, const u16_t h)
{
	u16_t spi_data[2];

	spi_data[0] = sys_cpu_to_be16(x);
	spi_data[1] = sys_cpu_to_be16(x + w - 1);
	ili9340_transmit(data, ILI9340_CMD_COLUMN_ADDR, &spi_data[0], 4);

	spi_data[0] = sys_cpu_to_be16(y);
	spi_data[1] = sys_cpu_to_be16(y + h - 1);
	ili9340_transmit(data, ILI9340_CMD_PAGE_ADDR, &spi_data[0], 4);
}

static struct ili9340_data ili9340_data;

DEVICE_AND_API_INIT(ili9340, CONFIG_ILI9340_DEV_NAME, &ili9340_init,
		    &ili9340_data, NULL, APPLICATION,
		    CONFIG_APPLICATION_INIT_PRIORITY, &ili9340_api);
