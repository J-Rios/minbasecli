
/**
 * @file    minbasecli.h
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

#define SIMPLECLI_READ_TIMEOUT_MS 100
#define SIMPLECLI_READ_INTERCHAR_TIMEOUT_MS 10
#define SIMPLECLI_MAX_READ_SIZE 64
#define SIMPLECLI_MAX_CMD_LEN 24
#define SIMPLECLI_MAX_ARGV_LEN 32
#define SIMPLECLI_MAX_ARGV 4

#if !defined(SIMPLECLI_BAUD_RATE)
    #define SIMPLECLI_BAUD_RATE 115200
#endif

/*****************************************************************************/

/* Data Types */

typedef struct t_cli_result
{
    char cmd[SIMPLECLI_MAX_CMD_LEN];
    char argv[SIMPLECLI_MAX_ARGV][SIMPLECLI_MAX_ARGV_LEN];
    uint8_t argc;
} t_cli_result;

/*****************************************************************************/

class MINBASECLI : public MINBASECLI_HAL
{
    public:
        MINBASECLI();

        bool setup(const uint32_t baud_rate=SIMPLECLI_BAUD_RATE);
        bool setup(void* iface, const uint32_t baud_rate=SIMPLECLI_BAUD_RATE);
        bool manage(t_cli_result* cli_result);
        uint32_t get_received_bytes();
        void print(const char* str);
        void println(const char* str);

    private:
        bool initialized;
        uint32_t received_bytes;
        char rx_read[SIMPLECLI_MAX_READ_SIZE];

        void set_default_result(t_cli_result* cli_result);
        bool iface_is_not_initialized();
        bool iface_read_data(char* rx_read, const size_t rx_read_size);
        size_t iface_read_data_t(char* rx_read, const size_t rx_read_size);
        uint32_t str_count_words(const char* str_in, const size_t str_in_len);
        bool str_read_until_char(char* str, const size_t str_len,
                const char until_c, char* str_read,
                const size_t str_read_size);
};

/*****************************************************************************/

#endif /* MINBASECLI_H_ */
