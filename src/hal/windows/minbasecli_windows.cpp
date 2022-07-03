
/**
 * @file    minbasecli_windows.cpp
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

#if defined(_WIN32) || defined(_WIN64)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_windows.h"

// Device/Framework Libraries
#include <windows.h>

// Standard Libraries
#include <iostream>
#include <string.h>
#include <stdio.h>      // getchar(), printf()

/*****************************************************************************/

/* Read STDIN Strem Thread Prototype */

/**
 * @brief Windows Thread handler function to read STDIN data from the interface.
 * @param arg Windows Thread arguments.
 */
DWORD WINAPI th_read_stdin(LPVOID lpParam);

/*****************************************************************************/

/* Constructor */

/**
 * @details
 * This constructor initializes all attributtes of the CLI class.
 */
MINBASECLI_WINDOWS::MINBASECLI_WINDOWS()
{
    this->iface = NULL;
    this->th_rx_read_head = 0;
    this->th_rx_read_tail = 0;
    this->th_rx_read[0] = '\0';
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
 * @details
 * This function should get and initialize the interface element that is going
 * to be used by the CLI and it also start the STDIN data read thread.
 */
bool MINBASECLI_WINDOWS::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    if (launch_stdin_read_thread() == false)
        return false;
    return true;
}

/**
 * @details
 * This function return the number of bytes received by the interface that are
 * available to be read. 
 */
size_t MINBASECLI_WINDOWS::hal_iface_available()
{
    return (this->th_rx_read_head - this->th_rx_read_tail);
}

/**
 * @details
 * This function returns a received byte from the interface. It checks if there
 * is any byte avaliable to be read and increase the read circular buffer tail
 * index to "pop" this element from the buffer and return it.
 */
uint8_t MINBASECLI_WINDOWS::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    this->th_rx_read_tail = (this->th_rx_read_tail + 1) %
            MINBASECLI_MAX_READ_SIZE;
    return th_rx_read[this->th_rx_read_tail];
}

/**
 * @details
 * This function send a byte through the interface.
 */
void MINBASECLI_WINDOWS::hal_iface_print(const uint8_t data_byte)
{
    printf("%c", (char)(data_byte));
}

/*****************************************************************************/

/* Private Methods */

/**
 * @details
 * This function create a Windows Thread to handle the STDIN data read from the
 * interface.
 */
bool MINBASECLI_WINDOWS::launch_stdin_read_thread()
{
    HANDLE thread = CreateThread(NULL, 0, th_read_stdin, (PVOID)(this),
            0, NULL);
    if (!thread)
    {
        printf("Fail to create STDIN read thread\n");
        return false;
    }

    return true;
}

/*****************************************************************************/

/* STDIN Read Thread */

/**
 * @details
 * This function is the Windows Thread that manages the STDIN data read. It gets
 * each new byte received from the interface and store them in the read buffer
 * (increasing the circular buffer head index).
 */
DWORD WINAPI th_read_stdin(LPVOID lpParam)
{
    MINBASECLI_WINDOWS* _this = (MINBASECLI_WINDOWS*) lpParam;
    uint8_t new_head = 0;

    while (true)
    {
        new_head = (_this->th_rx_read_head + 1) % MINBASECLI_MAX_READ_SIZE;
        _this->th_rx_read[new_head] = getc(stdin);
        _this->th_rx_read_head = new_head;
    }
}

/*****************************************************************************/

#endif /* defined(_WIN32) || defined(_WIN64) */
