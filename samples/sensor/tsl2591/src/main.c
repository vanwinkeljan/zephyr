/*
 * Copyright (c) 2018 dXplore
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <sensor.h>
#include <stdio.h>
#include <zephyr.h>

#define SYS_LOG_DOMAIN "main"
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG
#include <logging/sys_log.h>

void main(void)
{
	struct device *dev = device_get_binding("TSL2591");

	if (dev == NULL) {
		SYS_LOG_ERR(
		    "Could not find TSL2591_CMD device, aborting test.");
		return;
	}

	while (1) {
		struct sensor_value light;
		struct sensor_value ir;

		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &light);
		sensor_channel_get(dev, SENSOR_CHAN_IR, &ir);

		printf("Irradiance Full Spectrum: %d.%06d W/m2", light.val1,
		       light.val2);
		printf("Irradiance Infrared Spectrum: %d.%06d W/m2", ir.val1,
		       ir.val2);

		k_sleep(5000);
	}
}
