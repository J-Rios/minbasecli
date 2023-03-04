
/**
 * @file    minbasecli_arduino.cpp
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

#if defined(ARDUINO)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_arduino.h"

// Device/Framework Libraries
#include <Arduino.h>

// Standard Libraries
#include <string.h>

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#if defined(USBCON) // Arduinos: Leonardo, Micro, MKR, etc
    #define _IFACE Serial_
#else // Arduinos: UNO, MEGA, Nano, etc
    #define _IFACE HardwareSerial
#endif

/*****************************************************************************/

/* Constructor */

/**
 * @details
 * This constructor initializes all attributes of the CLI class.
 */
MINBASECLI_ARDUINO::MINBASECLI_ARDUINO()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
 * @details
 * This function should get and initialize the interface element that is going
 * to be used by the CLI.
 */
bool MINBASECLI_ARDUINO::hal_setup(void* iface, const uint32_t baud_rate)
{
    if (iface == NULL)
        { return false; }

    this->iface = iface;
    _IFACE* _Serial = (_IFACE*) this->iface;
    _Serial->begin(baud_rate);

    return true;
}

/**
 * @details
 * This function return the number of bytes received by the interface that are
 * available to be read.
 */
size_t MINBASECLI_ARDUINO::hal_iface_available()
{
    _IFACE* _Serial = (_IFACE*) this->iface;
    return ((size_t) (_Serial->available()));
}

/**
 * @details
 * This function returns a received byte from the interface.
 */
uint8_t MINBASECLI_ARDUINO::hal_iface_read()
{
    _IFACE* _Serial = (_IFACE*) this->iface;
    return _Serial->read();
}

/**
 * @details
 * This function send a byte through the interface.
 */
void MINBASECLI_ARDUINO::hal_iface_print(const uint8_t data_byte)
{
    _IFACE* _Serial = (_IFACE*) this->iface;

    _Serial->write(data_byte);
}

/*****************************************************************************/

#endif /* defined(ARDUINO) */
