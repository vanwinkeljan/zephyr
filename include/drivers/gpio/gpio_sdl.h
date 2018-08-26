/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __GPIO_SDL_H__
#define __GPIO_SDL_H__

#include <SDL.h>

/**
 * @brief Handle SDL keyboard event as GPIO input
 *
 * @param event SDL event to be handled as GPIO input
 */
void gpio_sdl_handle_keyboard_event(SDL_Event *event);

#endif /* __GPIO_SDL_H__ */
