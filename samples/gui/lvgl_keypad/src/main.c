/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <gpio.h>
#include <misc/util.h>
#include <stdbool.h>

#define LOG_LEVEL LOG_LEVEL_DEBUG
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

static struct gpio_callback key_callback;
static lv_style_t led_style;
static lv_obj_t *leds[4];
static lv_obj_t *switches[4];
static lv_group_t *group;

static u32_t last_key;
static lv_indev_state_t last_key_state = LV_INDEV_STATE_REL;

static lv_res_t switch_action(lv_obj_t *sw)
{
	u32_t idx = lv_obj_get_free_num(sw);

	if (lv_sw_get_state(sw)) {
		lv_led_on(leds[idx]);
	} else {
		lv_led_off(leds[idx]);
	}

	return LV_RES_OK;
}

static void create_leds(void)
{
	int idx;

	lv_style_copy(&led_style, &lv_style_pretty_color);
	led_style.body.radius = LV_RADIUS_CIRCLE;
	led_style.body.main_color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
	led_style.body.grad_color = LV_COLOR_MAKE(0x50, 0x07, 0x02);
	led_style.body.border.color = LV_COLOR_MAKE(0xfa, 0x0f, 0x00);
	led_style.body.border.width = 3;
	led_style.body.border.opa = LV_OPA_30;
	led_style.body.shadow.color = LV_COLOR_MAKE(0xb5, 0x0f, 0x04);
	led_style.body.shadow.width = 10;

	for (idx = 0; idx < ARRAY_SIZE(leds); ++idx) {
		leds[idx]  = lv_led_create(lv_scr_act(), NULL);
		lv_obj_set_style(leds[idx], &led_style);
		lv_obj_align(leds[idx], NULL, LV_ALIGN_IN_TOP_RIGHT, -40,
				20 + 55 * idx);
		lv_led_off(leds[idx]);
	}

}

static void create_switches(void)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE(switches); ++idx) {
		switches[idx] = lv_sw_create(lv_scr_act(), NULL);
		lv_obj_set_free_num(switches[idx], idx);
		lv_sw_set_action(switches[idx], switch_action);
		lv_group_add_obj(group, switches[idx]);
		lv_obj_align(switches[idx], NULL, LV_ALIGN_IN_TOP_MID, 0,
				20 + 55 * idx);
	}

}

static bool lv_key_read(lv_indev_data_t *data)
{
	data->key = last_key;
	data->state = last_key_state;
	if (data->state == LV_INDEV_STATE_PR) {
		last_key_state = LV_INDEV_STATE_REL;
	}
	return false;
}

static void key_callback_handler(struct device *port, struct gpio_callback *cb,
		u32_t pins)
{
	if ((pins & BIT(0)) != 0) {
		last_key = LV_GROUP_KEY_PREV;
		last_key_state = LV_INDEV_STATE_PR;
	}
	if ((pins & BIT(1)) != 0) {
		last_key = LV_GROUP_KEY_NEXT;
		last_key_state = LV_INDEV_STATE_PR;
	}
	if ((pins & BIT(2)) != 0) {
		last_key = LV_GROUP_KEY_LEFT;
		last_key_state = LV_INDEV_STATE_PR;
	}
	if ((pins & BIT(3)) != 0) {
		last_key = LV_GROUP_KEY_RIGHT;
		last_key_state = LV_INDEV_STATE_PR;
	}
}

void main(void)
{
	lv_indev_drv_t indev_drv;
	lv_indev_t *indev;
	struct device *display_dev;
	static struct device *gpio_dev;

	display_dev = device_get_binding("DISPLAY");

	if (display_dev == NULL) {
		LOG_ERR("display device not found.  Aborting test.");
		return;
	}

	gpio_dev = device_get_binding("GPIO_0");

	if (display_dev == NULL) {
		LOG_ERR("GPIO device not found.  Aborting test.");
		return;
	}

	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv.read = lv_key_read;
	indev = lv_indev_drv_register(&indev_drv);

	group = lv_group_create();
	lv_indev_set_group(indev, group);

	create_leds();
	create_switches();

	display_blanking_off(display_dev);

	gpio_pin_configure(gpio_dev,  0,
			GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE |
			GPIO_INT_ACTIVE_HIGH);
	gpio_pin_configure(gpio_dev,  1,
			GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE |
			GPIO_INT_ACTIVE_HIGH);
	gpio_pin_configure(gpio_dev,  2,
			GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE |
			GPIO_INT_ACTIVE_HIGH);
	gpio_pin_configure(gpio_dev,  3,
			GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE |
			GPIO_INT_ACTIVE_HIGH);
	gpio_init_callback(&key_callback, key_callback_handler,
			BIT(0) | BIT(1) | BIT(2) | BIT(3));
	gpio_add_callback(gpio_dev, &key_callback);
	gpio_pin_enable_callback(gpio_dev, 0);
	gpio_pin_enable_callback(gpio_dev, 1);
	gpio_pin_enable_callback(gpio_dev, 2);
	gpio_pin_enable_callback(gpio_dev, 3);

	while (1) {
		lv_task_handler();
		k_sleep(10);
	}
}

