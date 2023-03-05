
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

/**
 * @brief MINBASECLI_STM32 Class.
 */
class MINBASECLI_STM32
{
    /*************************************************************************/

    /* Public Methods */

    public:

        /**
         * @brief Construct a new minbasecli stm32 object.
         */
        MINBASECLI_STM32();

    /*************************************************************************/

    /* Protected Methods */

    protected:

        /**
         * @brief Configure the interface and communication speed of the CLI.
         * @param iface Pointer to interface element that will be used by the
         * CLI.
         * @param baud_rate Communication speed for the CLI.
         * @return true Interface configuration success.
         * @return false Interface configuration fail.
         */
        bool hal_setup(void* iface, const uint32_t baud_rate);

        /**
         * @brief Get the number of bytes that the interface has received and
         * are available in the current interface buffer to be read.
         * @return size_t The number of bytes available to be read.
         */
        size_t hal_iface_available();

        /**
         * @brief Get/read a byte from the interface.
         * @return uint8_t The byte read.
         */
        uint8_t hal_iface_read();

        /**
         * @brief Write a byte to the interface.
         * @param data_byte The byte to be written.
         */
        void hal_iface_print(const uint8_t data_byte);

    /*************************************************************************/

    /* Private Attributes */

    private:

        /**
         * @brief Pointer to interface used.
         */
        void* iface;

    /*************************************************************************/

    /* Private Methods */

    private:

        /**
         * @brief Specific function to setup a UART as interface.
         * @param baud_rate UART baud rate speed communication.
         * @param self_initialization Select if you want the CLI initialize
         * the UART with default settings or if the UART is already
         * initialized outside of this component.
         * @return true if UART setup result success.
         * @return false if UART setup result fail.
         */
        bool uart_setup(const uint32_t baud_rate,
                const bool self_initialization=true);
};

/*****************************************************************************/

#endif /* MINBASECLI_STM32_H_ */

#endif /* STM32 */
