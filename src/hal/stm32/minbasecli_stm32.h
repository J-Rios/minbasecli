
/**
 * @file    minbasecli_stm32.h
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

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F2)  || \
    defined(STM32F3) || defined(STM32F4) || defined(STM32F7)  || \
    defined(STM32L0) || defined(STM32L1) || defined(STM32L4)  || \
    defined(STM32L5) || defined(STM32G0) || defined(STM32G4)  || \
    defined(STM32H7) || defined(STM32U5) || defined(STM32MP1) || \
    defined(STM32WB) || defined(STM32WL)

#ifndef MINBASECLI_STM32_H_
#define MINBASECLI_STM32_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Constants & Defines */

/*****************************************************************************/

/* Class Interface */

class MINBASECLI_STM32
{
    public:
        MINBASECLI_STM32();

    protected:
        bool hal_setup(const uint32_t baud_rate);
        bool hal_setup(void* iface, const uint32_t baud_rate);
        size_t hal_iface_available();
        uint8_t hal_iface_read();
        void hal_iface_print(const uint8_t data_byte);
        void hal_millis_init();
        uint32_t hal_millis();

    private:
        void* iface;

        bool uart_setup(const uint32_t baud_rate,
                const bool self_initialization=true);
};

/*****************************************************************************/

#endif /* MINBASECLI_STM32_H_ */

#endif /* STM32 */
