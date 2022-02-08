
/**
 * @file    minbasecli_none.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    08-02-2022
 * @version 1.0.1
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
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_NONE::MINBASECLI_NONE()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL Methods */

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI::hal_setup()
{
    return true;
}

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI_LINUX::hal_setup(void* iface)
{
    this->iface = iface;
    return launch_stdin_read_thread();
}

/**
  * @brief  Print a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI_NONE::hal_iface_print(const char* str)
{
    return;
}

/**
  * @brief  Print line a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI_NONE::hal_iface_println(const char* str)
{
    return;
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI_NONE::hal_iface_available()
{
    return 0;
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_NONE::hal_iface_read()
{
    return 0;
}

/**
  * @brief  Initialize System-Tick count.
  */
void MINBASECLI_NONE::hal_millis_init()
{
  return;
}

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI_NONE::hal_millis()
{
    return 0;
}

/*****************************************************************************/

#endif /* !ARDUINO !ESP_PLATFORM !__linux__ !_WIN32 ... */
