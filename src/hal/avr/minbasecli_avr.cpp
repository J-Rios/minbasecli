
/**
 * @file    minbasecli_avr.cpp
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

#if defined(__AVR) && !defined(ARDUINO)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_avr.h"

// (UART Driver)
#include "avr_uart.h"

// Standard Libraries
#include <string.h>

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#define _IFACE AvrUart

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_AVR::MINBASECLI_AVR()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL Methods */

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI_AVR::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    _IFACE* _Serial = (_IFACE*) this->iface;
    _Serial->setup(baud_rate);
    return true;
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI_AVR::hal_iface_available()
{
    _IFACE* _Serial = (_IFACE*) this->iface;

    return ((size_t) (_Serial->num_rx_data_available()));
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_AVR::hal_iface_read()
{
    uint8_t read_byte = 0;

    _IFACE* _Serial = (_IFACE*) this->iface;

    if (!_Serial->read(&read_byte))
        return 0;

    return read_byte;
}

/**
  * @brief  Print a byte with ASCII encode to CLI HAL interface.
  * @param  data_byte Byte of data to write.
  */
void MINBASECLI_AVR::hal_iface_print(const uint8_t data_byte)
{
    _IFACE* _Serial = (_IFACE*) this->iface;

    _Serial->write(data_byte);
}

/*****************************************************************************/

#endif /* defined(__AVR) && !defined(ARDUINO) */
