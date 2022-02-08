
/**
 * @file    minbasecli_windows.cpp
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

#if defined(_WIN32) || defined(_WIN64)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_windows.h"

// Device/Framework Libraries
#include <windows.h>
#include <sys/timeb.h>  // millis()

// Standard Libraries
#include <iostream>
#include <string.h>
#include <stdio.h>      // getchar(), printf()

/*****************************************************************************/

/* Read STDIN Strem Thread Prototype */

DWORD WINAPI th_read_stdin(LPVOID lpParam);

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_WINDOWS::MINBASECLI_WINDOWS()
{
    this->iface = NULL;
    this->th_rx_read_head = 0;
    this->th_rx_read_tail = 0;
    this->th_rx_read[0] = '\0';

    // Initialize millis count
    hal_millis_init();
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  */
bool MINBASECLI_WINDOWS::hal_setup(const uint32_t baud_rate)
{
    if (launch_stdin_read_thread() == false)
        return false;
    return true;
}

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI_WINDOWS::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    return hal_setup(baud_rate);
}

/**
  * @brief  Print a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI_WINDOWS::hal_iface_print(const char* str)
{
    printf("%s", str);
}

/**
  * @brief  Print line a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI_WINDOWS::hal_iface_println(const char* str)
{
    printf("%s\r\n", str);
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI_WINDOWS::hal_iface_available()
{
    return (this->th_rx_read_head - this->th_rx_read_tail);
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_WINDOWS::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    this->th_rx_read_tail = (this->th_rx_read_tail + 1) %
            SIMPLECLI_MAX_READ_SIZE;
    return th_rx_read[this->th_rx_read_tail];
}

/**
  * @brief  Initialize System-Tick count.
  */
void MINBASECLI_WINDOWS::hal_millis_init()
{
    hal_millis();
}

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI_WINDOWS::hal_millis()
{
    static bool first_execution = true;
    static struct timeb t_start;
    static struct timeb t_now;
    uint32_t ms = 0;

    if (first_execution)
    {
        ftime(&t_start);
        first_execution = false;
    }

    ftime(&t_now);

    ms = (uint32_t) (1000.0 * (t_now.time - t_start.time) +
            (t_now.millitm - t_start.millitm));

    return ms;
}

/*****************************************************************************/

/* Private Methods */

/**
  * @brief  Launch the Thread to read from STDIN Stream.
  * @return If thread has been created (true/false).
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
  * @brief  Thread to read from STDIN Stream.
  */
DWORD WINAPI th_read_stdin(LPVOID lpParam)
{
    MINBASECLI_WINDOWS* _this = (MINBASECLI_WINDOWS*) lpParam;
    uint8_t new_head = 0;

    while (true)
    {
        new_head = (_this->th_rx_read_head + 1) % SIMPLECLI_MAX_READ_SIZE;
        _this->th_rx_read[new_head] = getc(stdin);
        _this->th_rx_read_head = new_head;
    }
}

/*****************************************************************************/

#endif /* defined(_WIN32) || defined(_WIN64) */
