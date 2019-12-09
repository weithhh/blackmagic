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

/* This file implements a transparent channel over which the GDB Remote
 * Serial Debugging protocol is implemented.
 */


#include <libopencm3/stm32/usart.h>

#include "general.h"
#include "cdcacm.h"
#include "gdb_if.h"

//volatile size_t count_tx;
//static uint8_t buffer_tx[CDCACM_PACKET_SIZE];

volatile size_t count_rx = 0;
static uint8_t buffer_rx[CDCACM_PACKET_SIZE];

void gdb_if_putchar(unsigned char c, int flush)
{
	flush = flush;
	usart_send_blocking(HOST_USART, c);

//	buffer_tx[count_tx] = c;
//	count_tx++;
//
//	if (flush || (count_tx == CDCACM_PACKET_SIZE)) {
//		while (count_tx) {
//			count_tx--;
//			usart_send_blocking(HOST_USART, buffer_tx[count_tx]);
//		}
//	}
}

static char nonblocking_getchar(void) {
	usart_disable_rx_interrupt(HOST_USART);

	count_rx--;
	uint8_t byte = buffer_rx[count_rx];

	usart_enable_rx_interrupt(HOST_USART);

	return byte;
}

unsigned char gdb_if_getchar(void)
{
	while (!count_rx);
	return nonblocking_getchar();
}

unsigned char gdb_if_getchar_to(int timeout)
{
	platform_timeout t;
	platform_timeout_set(&t, timeout);

	while (!platform_timeout_is_expired(&t)) {
		if (count_rx) {
			return nonblocking_getchar();
		}
	}

	return -1;
}

void usart3_isr(void)
{
	if (((USART_CR1(HOST_USART) & USART_CR1_RXNEIE) != 0) &&
		    ((USART_SR(HOST_USART) & USART_SR_RXNE) != 0))
	{
		uint16_t byte = usart_recv(HOST_USART);

		if (count_rx < CDCACM_PACKET_SIZE) {
			buffer_rx[count_rx] = byte;
			count_rx++;
		}
	}
}
