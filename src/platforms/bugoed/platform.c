/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
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

#include "general.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/scs.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

char serial_no[9] = "none";

int platform_hwversion(void)
{
	return 0;
}

void host_usart_init(void) {
	rcc_periph_clock_enable(HOST_USART_RCC);

	nvic_enable_irq(NVIC_USART3_IRQ);

	gpio_set_mode(HOST_USART_TX_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, HOST_USART_TX_PIN);
	gpio_set_mode(HOST_USART_RX_PORT, GPIO_MODE_INPUT,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, HOST_USART_RX_PIN);

	usart_set_baudrate(HOST_USART, 460800);
	usart_set_databits(HOST_USART, 8);
	usart_set_stopbits(HOST_USART, USART_STOPBITS_1);
	usart_set_parity(HOST_USART, USART_PARITY_NONE);
	usart_set_flow_control(HOST_USART, USART_FLOWCONTROL_NONE);
	usart_set_mode(HOST_USART, USART_MODE_TX_RX);

	usart_enable_rx_interrupt(HOST_USART);
	usart_enable(HOST_USART);
}

void platform_init(void)
{
	SCS_DEMCR |= SCS_DEMCR_VC_MON_EN;
#ifdef ENABLE_DEBUG
	void initialise_monitor_handles(void);
	initialise_monitor_handles();
#endif
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* Enable peripherals */
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_CRC);

	/* Setup JTAG GPIO ports */
	gpio_set_mode(TMS_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_INPUT_FLOAT, TMS_PIN);
	gpio_set_mode(TCK_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, TCK_PIN);
	gpio_set_mode(TDI_PORT, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, TDI_PIN);
	gpio_set_mode(TDO_PORT, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_FLOAT, TDO_PIN);

	platform_srst_set_val(false);

	/* Relocate interrupt vector table here */
	extern int vector_table;
	SCB_VTOR = (uint32_t)&vector_table;

	platform_timing_init();
	host_usart_init();
}

void platform_srst_set_val(bool assert)
{
	/* We reuse JSRST as SRST.*/
	if (assert) {
		gpio_set_mode(JRST_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		              GPIO_CNF_OUTPUT_OPENDRAIN, JRST_PIN);
		/* Wait until requested value is active.*/
		while (gpio_get(JRST_PORT, JRST_PIN))
			gpio_clear(JRST_PORT, JRST_PIN);
	} else {
		gpio_set_mode(JRST_PORT, GPIO_MODE_INPUT,
					  GPIO_CNF_INPUT_PULL_UPDOWN, JRST_PIN);
		/* Wait until requested value is active.*/
		while (!gpio_get(JRST_PORT, JRST_PIN))
			gpio_set(JRST_PORT, JRST_PIN);
	}
}

bool platform_srst_get_val(void)
{
	return gpio_get(JRST_PORT, JRST_PIN) == 0;
}

const char *platform_target_voltage(void)
{
	return "not supported";
}

void set_idle_state(int state)
{
	gpio_set_val(LED_PORT, LED_IDLE_RUN, (!state));
}


