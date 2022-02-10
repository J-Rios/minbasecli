
/**
 * @file    minbasecli.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    10-02-2022
 * @version 1.1.0
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

#ifndef MAIN_MINBASECLI_H_
#define MAIN_MINBASECLI_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Use Specific HAL for build system */

#if defined(__linux__)
    #include "hal/linux/minbasecli_linux.h"
    #define MINBASECLI_HAL MINBASECLI_LINUX
#elif defined(_WIN32) || defined(_WIN64)
    #include "hal/windows/minbasecli_windows.h"
    #define MINBASECLI_HAL MINBASECLI_WINDOWS
#elif defined(ARDUINO)
    #include "hal/arduino/minbasecli_arduino.h"
    #define MINBASECLI_HAL MINBASECLI_ARDUINO
#elif defined(__AVR)
    #include "hal/avr/minbasecli_avr.h"
    #define MINBASECLI_HAL MINBASECLI_AVR
#elif defined(ESP_PLATFORM)
    #include "hal/espidf/minbasecli_espidf.h"
    #define MINBASECLI_HAL MINBASECLI_ESPIDF
#elif defined(STM32F0) || defined(STM32F1) || defined(STM32F2) \
|| defined(STM32G0) || defined(STM32G4) || defined(STM32H7) \
|| defined(STM32F3) || defined(STM32F4) || defined(STM32F7) \
|| defined(STM32L0) || defined(STM32L1) || defined(STM32L4) \
|| defined(STM32L5) || defined(STM32MP1) || defined(STM32U5) \
|| defined(STM32WB) || defined(STM32WL)
    #include "hal/stm32/minbasecli_stm32.h"
    #define MINBASECLI_HAL MINBASECLI_STM32
#else
    #warning "minbasecli - Unsupported device/system."
    #define HAL_NONE
    #include "hal/none/minbasecli_none.h"
    #define MINBASECLI_HAL MINBASECLI_NONE
#endif

/*****************************************************************************/

/* Constants & Defines */

// Default CLI Interface to use if not provided
#if !defined(MINBASECLI_DEFAULT_IFACE)
    #define MINBASECLI_DEFAULT_IFACE 0
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

/*****************************************************************************/

/* Data Types */

// CLI manage result data
typedef struct t_cli_result
{
    char cmd[MINBASECLI_MAX_CMD_LEN];
    char argv[MINBASECLI_MAX_ARGV][MINBASECLI_MAX_ARGV_LEN];
    uint8_t argc;
} t_cli_result;

/*****************************************************************************/

/* MinBaseCLI Class Interface */

// Inherit from corresponding HAL
class MINBASECLI : public MINBASECLI_HAL
{
    public:
        MINBASECLI();

        bool setup(void* iface,
                const uint32_t baud_rate=MINBASECLI_DEFAULT_BAUDS);
        bool manage(t_cli_result* cli_result);
        uint32_t get_received_bytes();
        void print(const char* str);
        void println(const char* str);

    private:
        bool initialized;
        uint32_t received_bytes;
        char rx_read[MINBASECLI_MAX_READ_SIZE];

        void set_default_result(t_cli_result* cli_result);
        bool iface_is_not_initialized();
        bool iface_read_data(char* rx_read, const size_t rx_read_size);
        uint32_t str_count_words(const char* str_in, const size_t str_in_len);
        bool str_read_until_char(char* str, const size_t str_len,
                const char until_c, char* str_read,
                const size_t str_read_size);
};

/*****************************************************************************/

#endif /* MINBASECLI_H_ */
