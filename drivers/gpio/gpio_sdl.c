/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <gpio.h>
#include <errno.h>
#include "gpio_utils.h"
#include <SDL.h>
#include "irq_ctrl.h"

typedef void (*gpio_sdl_config_irq_t)(void);

enum gpio_sdl_switch_type {
	gpio_sdl_toggle_switch = 0,
	gpio_sdl_momentary_no_switch,
	gpio_sdl_momentary_nc_switch,
	gpio_sdl_impulse_switch,
};

struct gpio_sdl_data {
	sys_slist_t callbacks;
	u32_t enabled_callbacks;
	int flags[32];
	u32_t pin_state;
	u32_t pending_irqs;
};

struct gpio_sdl_config {
	u8_t scan_codes[32];
	enum gpio_sdl_switch_type switch_types[32];
	gpio_sdl_config_irq_t cfg_irq_func;
	u32_t irq_id;
};

static int gpio_sdl_init(struct device *dev);
static int gpio_sdl_config(struct device *port, int access_op, u32_t pin,
			int flags);
static int gpio_sdl_write(struct device *port, int access_op, u32_t pin,
			u32_t value);
static int gpio_sdl_read(struct device *port, int access_op, u32_t pin,
			u32_t *value);
static int gpio_sdl_manage_callback(struct device *port,
			struct gpio_callback *callback, bool set);
static int gpio_sdl_enable_callback(struct device *port, int access_op,
			u32_t pin);
static int gpio_sdl_disable_callback(struct device *port, int access_op,
			u32_t pin);
static u32_t gpio_sdl_get_pending_int(struct device *dev);
static void gpio_sdl_write_pin(struct gpio_sdl_data *data,  u32_t pin,
		bool enabled);
static void gpio_sdl_0_cfg_irq(void);
static void gpio_sdl_isr(void *arg);

static void gpio_sdl_handle_pin_event(u32_t pin, struct gpio_sdl_data *data,
	const struct gpio_sdl_config *cfg, SDL_Event *event);
static bool gpio_sdl_handle_toggle_switch(u32_t pin,
		struct gpio_sdl_data *data, SDL_Event *event);
static bool gpio_sdl_handle_momentary_switch(u32_t pin,
		struct gpio_sdl_data *data, SDL_Event *event,
		bool normal_closed);
static bool gpio_sdl_handle_impulse_switch(u32_t pin,
		struct gpio_sdl_data *data, SDL_Event *event);
static void gpio_sdl_handle_irq_generation(u32_t pin,
		struct gpio_sdl_data *data, const struct gpio_sdl_config *cfg,
		bool prev_pin_state, bool new_pin_state);

#define DEV_GPIO_CFG(dev) \
	((const struct gpio_sdl_config * const)dev->config->config_info)
#define DEV_GPIO_DATA(dev) \
	((struct gpio_sdl_data * const)dev->driver_data)

struct gpio_driver_api gpio_sdl_driver_api = {
	.config = gpio_sdl_config,
	.write = gpio_sdl_write,
	.read = gpio_sdl_read,
	.manage_callback = gpio_sdl_manage_callback,
	.enable_callback = gpio_sdl_enable_callback,
	.disable_callback = gpio_sdl_disable_callback,
	.get_pending_int = gpio_sdl_get_pending_int};

int gpio_sdl_config(struct device *port, int access_op, u32_t pin, int flags)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);
	u32_t start_pin;
	u32_t end_pin;
	u32_t cur_pin;

	if (access_op == GPIO_ACCESS_BY_PIN) {
		__ASSERT_NO_MSG(pin < 32);
		if (pin < 32) {
			start_pin = pin;
			end_pin = pin;
		} else {
			return -ENOTSUP;
		}
	} else if (access_op == GPIO_ACCESS_BY_PORT) {
		start_pin = 0;
		end_pin = 31;
	} else {
		return -ENOTSUP;
	}

	for (cur_pin = start_pin; cur_pin <= end_pin; ++cur_pin) {
		data->flags[cur_pin] = flags;
	}

	return 0;
}

void gpio_sdl_write_pin(struct gpio_sdl_data *data,  u32_t pin, bool enabled)
{

	if ((data->flags[pin] & GPIO_DIR_MASK) == GPIO_DIR_OUT) {
		if (enabled) {
			data->pin_state |= BIT(pin);
		} else {
			data->pin_state &= ~BIT(pin);
		}
	}
}

int gpio_sdl_write(struct device *port, int access_op, u32_t pin, u32_t value)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);

	if (access_op == GPIO_ACCESS_BY_PIN) {
		__ASSERT_NO_MSG(pin < 32);
		if (pin < 32) {
			gpio_sdl_write_pin(data, pin, (value != 0));
		} else {
			return -ENOTSUP;
		}
	} else if (access_op == GPIO_ACCESS_BY_PORT) {
		u32_t cur_pin;
		bool enabled;

		for (cur_pin = 0; cur_pin < 32; ++cur_pin) {
			enabled = ((value & BIT(cur_pin)) != 0);
			gpio_sdl_write_pin(data, cur_pin, enabled);
		}
	} else {
		return -ENOTSUP;
	}
	return 0;
}

int gpio_sdl_read(struct device *port, int access_op, u32_t pin, u32_t *value)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);

	if (access_op == GPIO_ACCESS_BY_PIN) {
		__ASSERT_NO_MSG(pin < 32);
		if (pin < 32) {
			*value = ((data->pin_state & BIT(pin)) != 0);
		} else {
			return -ENOTSUP;
		}
	} else if (access_op == GPIO_ACCESS_BY_PORT) {
		*value = data->pin_state;
	} else {
		return -ENOTSUP;
	}
	return 0;
}

int gpio_sdl_manage_callback(struct device *port,
			struct gpio_callback *callback, bool set)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);

	_gpio_manage_callback(&data->callbacks, callback, set);

	return 0;
}

int gpio_sdl_enable_callback(struct device *port, int access_op, u32_t pin)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);

	if (access_op == GPIO_ACCESS_BY_PIN) {
		__ASSERT_NO_MSG(pin < 32);
		if (pin < 32) {
			data->enabled_callbacks |= BIT(pin);
		} else {
			return -ENOTSUP;
		}
	} else if (access_op == GPIO_ACCESS_BY_PORT) {
		data->enabled_callbacks = 0xFFFFFFFF;
	} else {
		return -ENOTSUP;
	}
	return 0;
}

int gpio_sdl_disable_callback(struct device *port, int access_op, u32_t pin)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(port);

	if (access_op == GPIO_ACCESS_BY_PIN) {
		__ASSERT_NO_MSG(pin < 32);
		if (pin < 32) {
			data->enabled_callbacks &= ~BIT(pin);
		} else {
			return -ENOTSUP;
		}
	} else if (access_op == GPIO_ACCESS_BY_PORT) {
		data->enabled_callbacks = 0x00000000;
	} else {
		return -ENOTSUP;
	}
	return 0;
}

u32_t gpio_sdl_get_pending_int(struct device *dev)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(dev);

	return data->pending_irqs;
}

void gpio_sdl_isr(void *arg)
{
	u32_t callbacks;
	struct device *dev = (struct device *)arg;
	struct gpio_sdl_data *data = DEV_GPIO_DATA(dev);

	callbacks = data->pending_irqs & data->enabled_callbacks;
	data->pending_irqs = 0;

	_gpio_fire_callbacks(&data->callbacks, dev, callbacks);
}

int gpio_sdl_init(struct device *dev)
{
	struct gpio_sdl_data *data = DEV_GPIO_DATA(dev);
	const struct gpio_sdl_config *cfg = DEV_GPIO_CFG(dev);

	data->enabled_callbacks = 0;
	data->pin_state = 0;
	memset(data->flags, 0, 32 * sizeof(u32_t));
	data->pending_irqs = 0;

	cfg->cfg_irq_func();

	return 0;
}
static struct gpio_sdl_data gpio_sdl_0_data;
static const struct gpio_sdl_config gpio_sdl_0_cfg = {
	.scan_codes = {
		CONFIG_GPIO_SDL_0_PIN_0_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_1_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_2_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_3_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_4_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_5_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_6_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_7_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_8_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_9_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_10_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_11_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_12_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_13_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_14_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_15_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_16_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_17_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_18_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_19_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_20_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_21_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_22_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_23_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_24_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_25_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_26_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_27_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_28_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_29_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_30_SCAN_CODE,
		CONFIG_GPIO_SDL_0_PIN_31_SCAN_CODE,
	},
	.switch_types = {
		CONFIG_GPIO_SDL_0_PIN_0_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_1_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_2_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_3_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_4_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_5_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_6_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_7_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_8_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_9_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_10_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_11_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_12_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_13_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_14_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_15_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_16_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_17_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_18_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_19_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_20_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_21_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_22_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_23_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_24_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_25_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_26_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_27_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_28_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_29_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_30_SWITCH_TYPE,
		CONFIG_GPIO_SDL_0_PIN_31_SWITCH_TYPE,
	},
	.cfg_irq_func = gpio_sdl_0_cfg_irq,
	.irq_id = CONFIG_GPIO_SDL_0_IRQ
};

DEVICE_AND_API_INIT(gpio_sdl_0, CONFIG_GPIO_SDL_0_DEV_NAME, gpio_sdl_init,
		    &gpio_sdl_0_data, &gpio_sdl_0_cfg,
		    POST_KERNEL, CONFIG_GPIO_SDL_0_INIT_PRIORITY,
		    &gpio_sdl_driver_api);

void gpio_sdl_0_cfg_irq(void)
{
	IRQ_CONNECT(CONFIG_GPIO_SDL_0_IRQ,
			CONFIG_GPIO_SDL_0_IRQ_PRIORITY,
			gpio_sdl_isr,
			DEVICE_GET(gpio_sdl_0),
			0);

	irq_enable(CONFIG_GPIO_SDL_0_IRQ);
}

void gpio_sdl_handle_pin_event(u32_t pin, struct gpio_sdl_data *data,
	const struct gpio_sdl_config *cfg, SDL_Event *event)
{
	bool new_pin_state = false;
	bool prev_pin_state = false;

	switch (cfg->switch_types[pin]) {
	case gpio_sdl_toggle_switch:
		new_pin_state = gpio_sdl_handle_toggle_switch(pin, data,
				event);
		break;
	case gpio_sdl_momentary_no_switch:
		new_pin_state = gpio_sdl_handle_momentary_switch(pin,
				data, event, false);
		break;
	case gpio_sdl_momentary_nc_switch:
		new_pin_state = gpio_sdl_handle_momentary_switch(pin,
				data, event, true);
		break;
	case gpio_sdl_impulse_switch:
		new_pin_state = gpio_sdl_handle_impulse_switch(pin,
				data, event);
		break;
	default:
		return;
	}

	gpio_sdl_handle_irq_generation(pin, data, cfg, prev_pin_state,
			new_pin_state);
}

bool gpio_sdl_handle_toggle_switch(u32_t pin, struct gpio_sdl_data *data,
		SDL_Event *event)
{
	bool new_pin_state = data->pin_state & BIT(pin);
	if (event->type == SDL_KEYUP) {
		return new_pin_state;
	}

	if (event->key.repeat != 0) {
		return new_pin_state;
	}

	if (new_pin_state) {
		data->pin_state &= ~BIT(pin);
		new_pin_state = false;
	} else {
		data->pin_state |= BIT(pin);
		new_pin_state = true;
	}

	return new_pin_state;
}

bool gpio_sdl_handle_momentary_switch(u32_t pin, struct gpio_sdl_data *data,
		SDL_Event *event, bool normal_closed)
{
	bool new_pin_state;

	if (event->type == SDL_KEYDOWN) {
		new_pin_state = !normal_closed;
	} else {
		new_pin_state = normal_closed;
	}

	if (new_pin_state) {
		data->pin_state &= ~BIT(pin);
	} else {
		data->pin_state |= BIT(pin);
	}

	return new_pin_state;
}

bool gpio_sdl_handle_impulse_switch(u32_t pin, struct gpio_sdl_data *data,
		SDL_Event *event)
{
	bool new_pin_state = false;

	if (event->key.repeat != 0) {
		return new_pin_state;
	}

	if (event->type == SDL_KEYDOWN) {
		new_pin_state = true;
	} else {
		new_pin_state = false;
	}

	if (new_pin_state) {
		data->pin_state &= ~BIT(pin);
	} else {
		data->pin_state |= BIT(pin);
	}

	return new_pin_state;
}

void gpio_sdl_handle_irq_generation(u32_t pin, struct gpio_sdl_data *data,
		const struct gpio_sdl_config *cfg, bool prev_pin_state,
		bool new_pin_state)
{
	bool trigger_irq = false;
	u32_t flags = data->flags[pin];

	if ((flags & GPIO_INT) != GPIO_INT) {
		return;
	}

	if ((flags & GPIO_INT_LEVEL) == GPIO_INT_LEVEL) {

		if ((flags & GPIO_INT_ACTIVE_LOW) == GPIO_INT_ACTIVE_LOW) {
			trigger_irq = (new_pin_state == 0);
		} else {
			trigger_irq = (new_pin_state == 1);
		}

	} else {
		if ((flags & GPIO_INT_DOUBLE_EDGE) == GPIO_INT_DOUBLE_EDGE) {
			trigger_irq = prev_pin_state ^ new_pin_state;
		} else if ((flags & GPIO_INT_ACTIVE_LOW) ==
				GPIO_INT_ACTIVE_LOW) {
			trigger_irq = prev_pin_state && !new_pin_state;
		} else {
			trigger_irq = !prev_pin_state && new_pin_state;
		}
	}

	if (trigger_irq) {
		data->pending_irqs |= BIT(pin);
		hw_irq_ctrl_set_irq(cfg->irq_id);
	}
}

void gpio_sdl_handle_keyboard_event(SDL_Event *event)
{
	int cur_pin;

	if ((event->type != SDL_KEYUP) && (event->type != SDL_KEYDOWN)) {
		return;
	}

	for (cur_pin = 0; cur_pin < 32; ++cur_pin) {
		if (event->key.keysym.scancode ==
				gpio_sdl_0_cfg.scan_codes[cur_pin]) {
			gpio_sdl_handle_pin_event(cur_pin, &gpio_sdl_0_data,
					&gpio_sdl_0_cfg, event);
		}
	}
}
