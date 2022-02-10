
/**
 * @file    minbasecli_linux.cpp
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

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_linux.h"

// Device/Framework Libraries
#include <pthread.h>
#include <sys/select.h> // async stdin-stdout interface

// Standard Libraries
#include <string.h>
#include <stdio.h>      // getchar(), printf()
#include <time.h>       // millis()
#include <unistd.h>     // async stdin-stdout interface

/*****************************************************************************/

/* Read STDIN Strem Thread Prototype */

void* th_read_stdin(void* arg);

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_LINUX::MINBASECLI_LINUX()
{
    this->iface = NULL;
    this->th_rx_read_head = 0;
    this->th_rx_read_tail = 0;
    this->th_rx_read[0] = '\0';

    // Initialize millis count
    hal_millis_init();
}

/*****************************************************************************/

/* Specific Device/Framework HAL Methods */

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI_LINUX::hal_setup(const uint32_t baud_rate)
{
    return launch_stdin_read_thread();
}

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI_LINUX::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    return hal_setup(baud_rate);
}

size_t MINBASECLI_LINUX::hal_iface_available()
{
    return (this->th_rx_read_head - this->th_rx_read_tail);
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_LINUX::hal_iface_read()
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
  * @brief  Print a byte with ASCII encode to CLI HAL interface.
  * @param  data_byte Byte of data to write.
  */
void MINBASECLI_LINUX::hal_iface_print(const uint8_t data_byte)
{
    printf("%c", (char)(data_byte));
}

/**
  * @brief  Initialize System-Tick count.
  */
void MINBASECLI_LINUX::hal_millis_init()
{
    hal_millis();
}

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI_LINUX::hal_millis()
{
    static bool first_execution = true;
    static struct timespec t_start;
    static struct timespec t_now;
    uint32_t ms = 0;

    if (first_execution)
    {
        first_execution = false;
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_start) != 0)
            return 0;
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_now) != 0)
        return (t_start.tv_nsec / 1000000);

    ms = (uint32_t) ((((t_now.tv_sec - t_start.tv_sec) * 1000000000) +
            (t_now.tv_nsec - t_start.tv_nsec)) / 1000000);

    return ms;
}

/*****************************************************************************/

/* Private Methods */

/**
  * @brief  Launch the Thread to read from STDIN Stream.
  * @return If thread has been created (true/false).
  */
bool MINBASECLI_LINUX::launch_stdin_read_thread()
{
    pthread_t th_id;
    int rc = 0;

    rc = pthread_create(&th_id, NULL, th_read_stdin, (void*)(this));
    if (rc != 0)
    {
        printf("Fail to create STDIN read thread: %s\n", strerror(rc));
        return false;
    }

    return true;
}

/*****************************************************************************/

/* STDIN Read Thread */

/**
  * @brief  Thread to read from STDIN Stream.
  */
void* th_read_stdin(void* arg)
{
    MINBASECLI_LINUX* _this = (MINBASECLI_LINUX*) arg;
    uint8_t new_head = 0;

    while (true)
    {
        new_head = (_this->th_rx_read_head + 1) % SIMPLECLI_MAX_READ_SIZE;
        _this->th_rx_read[new_head] = getc(stdin);
        _this->th_rx_read_head = new_head;
    }
}

/*****************************************************************************/

#endif /* __linux__ */
