
/**
 * @file    constants.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    26-01-2022
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * General Project constants.
 *
 * @section LICENSE
 *
 * Copyright (c) 2021 Jose Miguel Rios Rubio. All right reserved.
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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/*****************************************************************************/

/* Constants & Defines */

// Default CPU Frequency to use if has not been provided
#if !defined(F_CPU)
    #define F_CPU 16000000UL
#endif

// Firmware Application Version
#define FW_APP_VERSION "1.0.0"

// USART UART number to use (i.e. UART0)
#define UART_NUM UART0

// UART Serial Speed Baud Rate
#define UART_BAUD_RATE 9600

/*****************************************************************************/

#endif /* CONSTANTS_H_ */
