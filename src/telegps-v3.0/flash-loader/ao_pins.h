/*
 * Copyright © 2023 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef _AO_PINS_H_
#define _AO_PINS_H_

#include <ao_flash_lpc_pins.h>

/* Debug pin 6, rx_ext */
#define AO_BOOT_PIN		1
#define AO_BOOT_APPLICATION_GPIO	1
#define AO_BOOT_APPLICATION_PIN		16
#define AO_BOOT_APPLICATION_VALUE	1
#define AO_BOOT_APPLICATION_MODE	AO_EXTI_MODE_PULL_UP

#define HAS_USB_PULLUP		1
#define AO_USB_PULLUP_PORT	1
#define AO_USB_PULLUP_PIN	23

#endif /* _AO_PINS_H_ */
