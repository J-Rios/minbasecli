
/**
 * @file    minbasecli.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    26-05-2021
 * @version 1.0.0
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

#ifdef __linux__

#ifndef MINBASECLI_H_
#define MINBASECLI_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Constants & Defines */

#define IFACE_STDIO NULL
#define SIMPLECLI_READ_TIMEOUT_MS 100
#define SIMPLECLI_READ_INTERCHAR_TIMEOUT_MS 10
#define SIMPLECLI_MAX_READ_SIZE 64
#define SIMPLECLI_MAX_CMD_LEN 24
#define SIMPLECLI_MAX_ARGV_LEN 32
#define SIMPLECLI_MAX_ARGV 4

/*****************************************************************************/

/* Data Types */

typedef struct t_cli_result
{
    char cmd[SIMPLECLI_MAX_CMD_LEN];
    char argv[SIMPLECLI_MAX_ARGV][SIMPLECLI_MAX_ARGV_LEN];
    uint8_t argc;
} t_cli_result;

/*****************************************************************************/

class MINBASECLI
{
    public:
        uint32_t th_rx_read_head, th_rx_read_tail;
        char th_rx_read[SIMPLECLI_MAX_READ_SIZE];

        MINBASECLI();

        bool setup();
        bool setup(void* iface);
        bool manage(t_cli_result* cli_result);
        uint32_t get_received_bytes();

    private:
        void* iface;
        bool initialized;
        uint32_t received_bytes;
        char rx_read[SIMPLECLI_MAX_READ_SIZE];

        bool launch_stdin_read_thread();

        void set_default_result(t_cli_result* cli_result);
        bool iface_is_not_initialized();
        bool iface_read_data(char* rx_read, const size_t rx_read_size);
        size_t iface_read_data_t(char* rx_read, const size_t rx_read_size);
        uint32_t str_count_words(const char* str_in, const size_t str_in_len);
        bool str_read_until_char(char* str, const size_t str_len,
                const char until_c, char* str_read,
                const size_t str_read_size);

        uint32_t hal_millis();
        void hal_iface_print(const char* str);
        void hal_iface_println(const char* str);
        size_t hal_iface_available();
        uint8_t hal_iface_read();
};

/*****************************************************************************/

#endif /* MINBASECLI_H_ */

#endif /* __linux__ */
