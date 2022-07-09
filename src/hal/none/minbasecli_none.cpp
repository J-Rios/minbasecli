
/**
 * @file    minbasecli_none.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    10-02-2022
 * @version 1.0.2
 *
 * @section DESCRIPTION
 *
 * A simple Command Line Interface C++ library implementation with HAL
 * emphasis to be used in different kind of devices and frameworks.
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

#if !defined(__linux__) && !defined(_WIN32) && !defined(_WIN64) \
&& !defined(__AVR) && !defined(ARDUINO) && !defined(ESP_PLATFORM) \
&& !defined(STM32F0) && !defined(STM32F1) && !defined(STM32F2) \
&& !defined(STM32G0) && !defined(STM32G4) && !defined(STM32H7) \
&& !defined(STM32F3) && !defined(STM32F4) && !defined(STM32F7) \
&& !defined(STM32L0) && !defined(STM32L1) && !defined(STM32L4) \
&& !defined(STM32L5) && !defined(STM32MP1) && !defined(STM32U5) \
&& !defined(STM32WB) && !defined(STM32WL)

#if !defined(HAL_NONE)
  #define HAL_NONE
#endif

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_none.h"

// Device/Framework Libraries
// None

// Standard Libraries
#include <stdio.h>
#include <string.h>

/*****************************************************************************/

/* Constructor */

/**
 * @details
 * This constructor initializes all attributtes of the CLI class.
 */
MINBASECLI_NONE::MINBASECLI_NONE()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL Methods */

/**
 * @details
 * This function should get and initialize the interface element that is going
 * to be used by the CLI.
 */
bool MINBASECLI_LINUX::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    return launch_stdin_read_thread();
}

/**
 * @details
 * This function return the number of bytes received by the interface that are
 * available to be read.
 */
size_t MINBASECLI_NONE::hal_iface_available()
{
    return 0;
}

/**
 * @details
 * This function returns a received byte from the interface.
 */
uint8_t MINBASECLI_NONE::hal_iface_read()
{
    return 0;
}

/**
 * @details
 * This function send a byte through the interface.
 */
void MINBASECLI_NONE::hal_iface_print(const uint8_t data_byte)
{
    return;
}

/*****************************************************************************/

#endif /* !ARDUINO !ESP_PLATFORM !__linux__ !_WIN32 ... */
