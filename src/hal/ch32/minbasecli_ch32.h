
/**
 * @file    minbasecli_ch32.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    13-04-2025
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * A simple Command Line Interface C++ library implementation with HAL
 * emphasis to be used in different kind of devices and frameworks.
 *
 * @section LICENSE
 *
 * Copyright (c) 2025 Jose Miguel Rios Rubio. All right reserved.
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

#if defined(CH32V00X) /* || defined(CH32V10X) || defined(CH32V20X) || \
    defined(CH32V30X) || defined(CH32L10X) || defined(CH32X03X) */

#ifndef MINBASECLI_CH32_H_
#define MINBASECLI_CH32_H_

/*****************************************************************************/

/* Libraries */

// Standard C++ libraries
#include <cstdint>
#include <cstddef>

/*****************************************************************************/

/* Constants & Defines */

#define MINBASECLI_HAL MINBASECLI_CH32

// Default CLI Interface to use if not provided
#if !defined(MINBASECLI_DEFAULT_IFACE)
    #define MINBASECLI_DEFAULT_IFACE NULL
#endif

// Default CLI Baud Rate Speed to use if not provided
#if !defined(MINBASECLI_DEFAULT_BAUDS)
    #define MINBASECLI_DEFAULT_BAUDS 115200
#endif

// Maximum CLI read buffer size
#if !defined(MINBASECLI_MAX_READ_SIZE)
    #define MINBASECLI_MAX_READ_SIZE 64
#endif

// Maximum CLI Command length
#if !defined(MINBASECLI_MAX_CMD_LEN)
    #define MINBASECLI_MAX_CMD_LEN 24
#endif

// Maximum CLI Command Argument length
#if !defined(MINBASECLI_MAX_ARGV_LEN)
    #define MINBASECLI_MAX_ARGV_LEN 32
#endif

// Maximum number of arguments to check on a received CLI command
#if !defined(MINBASECLI_MAX_ARGV)
    #define MINBASECLI_MAX_ARGV 4
#endif

// Maximum Print formatted number array size
#if !defined(MINBASECLI_MAX_PRINT_SIZE)
    #define MINBASECLI_MAX_PRINT_SIZE 22
#endif

// Maximum number of commands that can be added to the CLI
#if !defined(MINBASECLI_MAX_CMD_TO_ADD)
    #define MINBASECLI_MAX_CMD_TO_ADD 16
#endif

// Maximum length of command description text
#if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
    #define MINBASECLI_MAX_CMD_DESCRIPTION 64
#endif

/*****************************************************************************/

/* Class Interface */

/**
 * @brief MINBASECLI_CH32 Class.
 */
class MINBASECLI_CH32
{
    /*************************************************************************/

    /* Public Methods */

    public:

        /**
         * @brief Construct a new minbasecli stm32 object.
         */
        MINBASECLI_CH32();

    /*************************************************************************/

    /* Protected Methods */

    protected:

        /**
         * @brief Configure the interface and communication speed of the CLI.
         * @param iface_ Pointer to interface element that will be used by the
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

#endif /* MINBASECLI_CH32_H_ */

#endif /* STM32 */
