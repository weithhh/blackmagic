/*
 * This file is pat of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2018  Uwe Bonnes (bon@elektron.ikp.physik.tu-darmstadt.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file implements the platform specific functions for the STM32
 * implementation.
 */
#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <libopencm3/stm32/usart.h>

#include "gpio.h"
#include "timing.h"
#include "timing_stm32.h"
#include "version.h"

#ifdef ENABLE_DEBUG
# define PLATFORM_HAS_DEBUG
# define USBUART_DEBUG
#endif

#define BOARD_IDENT			"Black Magic Probe (Bugoed), (Firmware " FIRMWARE_VERSION ")"
#define BOARD_IDENT_DFU		"Black Magic (Upgrade), Bugoed, (Firmware " FIRMWARE_VERSION ")"
#define BOARD_IDENT_UPD		"Black Magic (DFU Upgrade), Bugoed, (Firmware " FIRMWARE_VERSION ")"
#define DFU_IDENT			"Black Magic Firmware Upgrade (Bugoed)"
#define UPD_IFACE_STRING	"@Internal Flash   /0x08000000/8*001Kg"

/* Hardware definitions... */
#define TMS_PORT	GPIOB
#define TCK_PORT	GPIOB
#define TDI_PORT	GPIOA
#define TDO_PORT	GPIOA
#define JRST_PORT	GPIOA
#define TMS_PIN		GPIO15
#define TCK_PIN		GPIO14
#define TDI_PIN		GPIO6
#define TDO_PIN		GPIO7
#define JRST_PIN	GPIO8

#define SWDIO_PORT 	TMS_PORT
#define SWCLK_PORT 	TCK_PORT
#define SWDIO_PIN	TMS_PIN
#define SWCLK_PIN	TCK_PIN

#define HOST_USART USART3
#define HOST_USART_RCC RCC_USART3
#define HOST_USART_TX_PORT GPIOB
#define HOST_USART_TX_PIN GPIO10
#define HOST_USART_RX_PORT GPIOB
#define HOST_USART_RX_PIN GPIO11

#define TMS_SET_MODE() \
	gpio_set_mode(TMS_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, TMS_PIN);
#define SWDIO_MODE_FLOAT() \
	gpio_set_mode(SWDIO_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, SWDIO_PIN);
#define SWDIO_MODE_DRIVE() \
	gpio_set_mode(SWDIO_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, SWDIO_PIN);

#ifdef ENABLE_DEBUG
extern bool debug_bmp;
int usbuart_debug_write(const char *buf, size_t len);
# define DEBUG printf
#else
# define DEBUG(...)
#endif

#define LED_PORT GPIOC
#define LED_IDLE_RUN GPIO13
#define SET_RUN_STATE(state)
#define SET_ERROR_STATE(state)
void set_idle_state(int state);
#define SET_IDLE_STATE(state) set_idle_state(state)

/* Use newlib provided integer only stdio functions */
#define sscanf siscanf
#define sprintf siprintf
#define vasprintf vasiprintf
#define snprintf sniprintf

#endif

