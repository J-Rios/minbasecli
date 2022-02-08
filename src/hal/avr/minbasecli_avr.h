
/**
 * @file    minbasecli_avr.h
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

#if defined(__AVR) && !defined(ARDUINO)

#ifndef MINBASECLI_AVR_H_
#define MINBASECLI_AVR_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Constants & Defines */

#if !defined(SIMPLECLI_UART)
    #define SIMPLECLI_UART 0
#endif

/*****************************************************************************/

/* CLass Interface */

class MINBASECLI_AVR
{
    public:
        MINBASECLI_AVR();

    protected:
        bool hal_setup(const uint32_t baud_rate);
        bool hal_setup(void* iface, const uint32_t baud_rate);
        void hal_iface_print(const char* str);
        void hal_iface_println(const char* str);
        size_t hal_iface_available();
        uint8_t hal_iface_read();
        void hal_millis_init();
        uint32_t hal_millis();

    private:
        void* iface;
};

/*****************************************************************************/

#endif /* MINBASECLI_AVR_H_ */

#endif /* defined(__AVR) && !defined(ARDUINO) */
