/* Driver for TSL2591 light-to-digital converter
 *
 * Copyright (c) 2018, dXplore
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tsl2591.h"
#include <init.h>
#include <misc/byteorder.h>
#include <sensor.h>

static int tsl2591_sample_fetch(struct device *dev, enum sensor_channel channel)
{
	struct tsl2591_data *data = dev->driver_data;
	u16_t saturation = data->atime == 0 ? 2^15 : 2^16;
	int err;
	u8_t cmd;
	u16_t sensor_data[2];
	u32_t vis;
	u32_t ir;

	__ASSERT_NO_MSG(channel == SENSOR_CHAN_ALL);

	cmd = TSL2591_CMD | TSL2591_CMD_NORMAL | TSL2591_CMD_C0DATAL;
	err =
	    i2c_burst_read(data->i2c_master, TSL2591_ADDR, cmd,
			    (u8_t *) sensor_data, 4);
	if (err < 0) {
		return err;
	}

	vis = sys_le16_to_cpu(sensor_data[0]);

	if (vis >= saturation) {
		data->lux_vis = ~0;
	} else {
		/* Store result value in uW/m2
		 *
		 * 264 counts per uW/cm2 @ 850nm
		 * uW/cm2 = 0.01W/m2 = 1/100 W/m2
		 *
		 * Steps for calculation:
		 * 1) scale with 2^16
		 * 2) divide by 26400 (1/(264*100)
		 * 3) multiply by 15 ((1/10e-6) / (2^16))
		 * 4) compensate for gain and measurement window
		 *    100ms / ((integration time) * gain)
		 *    or 1 / ( (1+ATIME) * gain
		 */
		vis <<= 16;
		vis /= 26400;
		vis *= 15;
		vis /= ((data->atime + 1) * data->gain);
		data->lux_vis = vis;
	}


	ir = sys_le16_to_cpu(sensor_data[1]);

	if (ir >= saturation) {
		data->lux_ir = ~0;
	} else {
		/* Store result value in uW/m2
		 *
		 * 155 counts per uW/cm2 @ 850nm
		 * uW/cm2 = 0.01W/m2 = 1/100 W/m2
		 *
		 * Steps for calculation:
		 * 1) scale with 2^16
		 * 2) divide by 15500 (1/(155*100)
		 * 3) multiply by 15 ((1/10e-6) / (2^16))
		 * 4) compensate for gain and measurement window
		 *    100ms / ((integration time) * gain)
		 *    or 1 / ( (1+ATIME) * gain
		 */
		ir <<= 16;
		ir /= 15500;
		ir *= 15;
		ir /= ((data->atime + 1) * data->gain);
		data->lux_ir = ir;
	}
	return err;
}

static int tsl2591_channel_get(struct device *dev, enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct tsl2591_data *data = dev->driver_data;

	switch (chan) {
	case SENSOR_CHAN_LIGHT:
		val->val1 = data->lux_vis / 1000000;
		val->val2 = data->lux_vis % 1000000;
		break;
	case SENSOR_CHAN_IR:
		val->val1 = data->lux_ir / 1000000;
		val->val2 = data->lux_ir % 1000000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static u32_t tsl2591_write_byte(struct tsl2591_data *data, u8_t reg, u8_t val)
{
	u8_t cmd;

	cmd = TSL2591_CMD | TSL2591_CMD_NORMAL | reg;
	return i2c_reg_write_byte(data->i2c_master, TSL2591_ADDR, cmd, val);
}

int tsl2591_init(struct device *dev)
{
	struct tsl2591_data *data = dev->driver_data;
	int err;

	data->lux_vis = 0;
	data->lux_ir = 0;
	data->atime = 0;
	data->gain = 1;

	data->i2c_master =
	    device_get_binding(CONFIG_TSL2591_I2C_MASTER_DEV_NAME);
	if (!data->i2c_master) {
		SYS_LOG_DBG("i2c master %s not found",
			    CONFIG_TSL2591_I2C_MASTER_DEV_NAME);
		return -EINVAL;
	}

	err = tsl2591_write_byte(data, TSL2591_CMD_CONTROL, TSL2591_CTL_SRESET);
	if (err < 0) {
		return err;
	}

	err = tsl2591_write_byte(data, TSL2591_CMD_ENABLE,
				 TSL2591_ENA_AIEN | TSL2591_ENA_PON);
	if (err < 0) {
		return err;
	}

	return 0;
}

static const struct sensor_driver_api tsl2591_api_funcs = {
	.sample_fetch = tsl2591_sample_fetch,
	.channel_get = tsl2591_channel_get,
};

static struct tsl2591_data tsl2591_data;

DEVICE_AND_API_INIT(tsl2591, CONFIG_TSL2591_DEV_NAME, tsl2591_init,
		    &tsl2591_data, NULL, POST_KERNEL,
		    CONFIG_SENSOR_INIT_PRIORITY, &tsl2591_api_funcs);
