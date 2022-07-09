
/**
 * @file    avr_uart.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    26-01-2022
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * UART driver interface for AVR devices.
 *
 * @section LICENSE
 *
 * Copyright (c) 2020 Jose Miguel Rios Rubio. All right reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*****************************************************************************/

/* Include Guard */

#ifndef AVR_UART_H_
#define AVR_UART_H_

/*****************************************************************************/

/* Hardware Supported Devices Selector */

#if defined(ARDUINO)

    #warning "[AVR-UART Driver] Arduino Framework can't be used"

#elif \
defined(__AVR_ATmega48__)    || defined(__AVR_ATmega48A__)   || \
defined(__AVR_ATmega48PA__)  || defined(__AVR_ATmega48PB__)  || \
defined(__AVR_ATmega48P__)   || defined(__AVR_ATmega88__)    || \
defined(__AVR_ATmega88A__)   || defined(__AVR_ATmega88P__)   || \
defined(__AVR_ATmega88PA__)  || defined(__AVR_ATmega88PB__)  || \
defined(__AVR_ATmega168__)   || defined(__AVR_ATmega168A__)  || \
defined(__AVR_ATmega168P__)  || defined(__AVR_ATmega168PA__) || \
defined(__AVR_ATmega168PB__) || defined(__AVR_ATmega328__)   || \
defined(__AVR_ATmega328P__)

    #include "hal/atmega328/atmega328_uart.h"

#elif \
defined(__AVR_ATmega640__)   || defined(__AVR_ATmega1280__)  || \
defined(__AVR_ATmega1281__)  || defined(__AVR_ATmega2560__)  || \
defined(__AVR_ATmega2561__)

    #include "hal/atmega2560/atmega2560_uart.h"

#else

    #error "[AVR-UART Driver] Unsupported Device"

#endif

/*****************************************************************************/

#endif /* AVR_UART_H_ */
