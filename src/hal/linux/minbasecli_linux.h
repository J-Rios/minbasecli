
/**
 * @file    minbasecli_linux.h
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

#if defined(__linux__)

#ifndef MINBASECLI_LINUX_H_
#define MINBASECLI_LINUX_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Constants & Defines */

// Maximum number of bytes that can be stored in thread data read buffer
#define MINBASECLI_MAX_READ_SIZE 64

/*****************************************************************************/

/* Class Interface */

/**
 * @brief MINBASECLI_ESPIDF Class.
 */
class MINBASECLI_LINUX
{
    /*************************************************************************/

    /* Public Attributes */

    public:

        /**
         * @brief Read buffer to store data from STDIN read thread.
         */
        char th_rx_read[MINBASECLI_MAX_READ_SIZE];

        /**
         * @brief th_rx_read head index for circular buffer behaviour. 
         */
        uint32_t th_rx_read_head;

        /**
         * @brief th_rx_read tail index for circular buffer behaviour. 
         */
        uint32_t th_rx_read_tail;

    /*************************************************************************/

    /* Public Methods */

    public:

        /**
         * @brief Construct a new minbasecli linux object.
         */
        MINBASECLI_LINUX();

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
         * @brief Get the number of bytes that the interface has recevived and
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
         * @brief Pointer to interfce used.
         */
        void* iface;

    /*************************************************************************/

    /* Private Methods */

    private:

        /**
         * @brief Specific function to launch STDIN data read thread.
         * @return true if thread start result success.
         * @return false if thread start result fail.
         */
        bool launch_stdin_read_thread();
};

/*****************************************************************************/

#endif /* MINBASECLI_LINUX_H_ */

#endif /* defined(__linux__) */
