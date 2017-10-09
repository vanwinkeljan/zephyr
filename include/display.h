/*
 * Copyright (c) 2017 dXplore
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for display drivers and applications
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/**
 * @brief Display Interface
 * @defgroup display_interface display Interface
 * @ingroup io_interfaces
 * @{
 */

#include <device.h>
#include <stddef.h>
#include <zephyr/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef display_on_api
 * @brief Callback API to turn display on
 * See display_on() for argument description
 */
typedef int (*display_on_api)(struct device *dev);

/**
 * @typedef display_off_api
 * @brief Callback API to tunrn display off
 * See display_off() for argument description
 */
typedef int (*display_off_api)(struct device *dev);

/**
 * @typedef display_write_bitmap_api
 * @brief Callback API for writing bitmap
 * See display_write_bitmap() for argument description
 */
typedef int (*display_write_bitmap_api)(const struct device *dev, const u16_t x,
					const u16_t y, const u16_t w,
					const u16_t h, const u8_t *rgb_data);

/**
 * @brief Display driver API
 * API which a display driver should expose
 */
struct display_driver_api {
	display_on_api display_on;
	display_off_api display_off;
	display_write_bitmap_api write_bitmap;
};

/**
 * @brief Write bitmap
 *
 * @param dev Pointer to device structure
 * @param x x coordinate of the upper left corner
 * @param y y coordinate of the upper left corner
 * @param w width of the bitmap
 * @param h height of the bitmap
 * @param rgb_data pointer to the RGB data array, the data array should be at
 * minimum w * h * 3 bytes
 *
 * @retval 0 on success else negative errno code.
 */
inline int display_write_bitmap(const struct device *dev, const u16_t x,
				const u16_t y, const u16_t w, const u16_t h,
				const u8_t *rgb_data)
{
	struct display_driver_api *api =
	    (struct display_driver_api *)dev->driver_api;
	return api->write_bitmap(dev, x, y, w, h, rgb_data);
}

/**
 * @brief Turn display on
 *
 * @param dev Pointer to device structure
 */
inline int display_on(struct device *dev)
{
	struct display_driver_api *api =
	    (struct display_driver_api *)dev->driver_api;
	return api->display_on(dev);
}

/**
 * @brief Turn display off
 *
 * @param dev Pointer to device structure
 */
inline int display_off(struct device *dev)
{
	struct display_driver_api *api =
	    (struct display_driver_api *)dev->driver_api;
	return api->display_off(dev);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __DISPLAY_H__*/
