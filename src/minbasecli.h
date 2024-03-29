
/**
 * @file    minbasecli.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    04-03-2023
 * @version 1.2.0
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

// Interface HAL Selection and configuration
#include "minbasecli_hal_select.h"

/*****************************************************************************/

/* Constants */

/**
 * @brief Builtin command "help" text.
 */
static const char CMD_HELP[] = "help";

/**
 * @brief Builtin command "help" description text.
 */
static const char CMD_HELP_DESCRIPTION[] = "Shows current info.";

/*****************************************************************************/

/* Data Types */

// Forward Declaration of current class
class MINBASECLI;

// Command callbacks type
typedef void (*t_command_callback)(MINBASECLI* Cli, int argc, char* argv[]);

// Command function callback information
typedef struct t_cmd_cb_info
{
    char command[MINBASECLI_MAX_CMD_LEN];
    char description[MINBASECLI_MAX_CMD_DESCRIPTION];
    t_command_callback callback;
} t_cmd_cb_info;

// CLI manage result data
typedef struct t_cli_result
{
    char cmd[MINBASECLI_MAX_CMD_LEN];
    char argv[MINBASECLI_MAX_ARGV][MINBASECLI_MAX_ARGV_LEN];
    uint8_t argc;
} t_cli_result;

/*****************************************************************************/

/* MinBaseCLI Class Interface */

/**
 * @brief MINBASECLI Class.
 * Inherit from corresponding HAL CLI interface class.
 */
class MINBASECLI : public MINBASECLI_HAL
{
    /*************************************************************************/

    /* Public Methods */

    public:

        /**
         * @brief Construct a new MINBASECLI object.
         */
        MINBASECLI();

        /**
         * @brief Configure the MINBASECLI object specifying the interface
         * element to use and the communication speed.
         * @param iface Pointer to Interface element to be used by the CLI.
         * @param baud_rate Communication speed for the CLI.
         * @return Setup result success/fail (true/false).
         */
        bool setup(
            void* iface=MINBASECLI_DEFAULT_IFACE,
            const uint32_t baud_rate=MINBASECLI_DEFAULT_BAUDS
        );

        /**
         * @brief Add and bind a new command to a callback function.
         * @param command Command text that fires the callback.
         * @param callback Pointer to function that must be executed when the
         * command text is received through the CLI.
         * @param description Command description text that will be shown on
         * help command execution.
         * @return true if the command has been successfully added/bind.
         * @return false if the command can't be added/bind (the command
         * already exists or there is no more memory space for a new command).
         */
        bool add_cmd(
            const char* command,
            t_command_callback callback,
            const char* description
        );

        /**
         * @brief Let the Command Line Interface run an execution iteration to
         * check if an added command has been received and then call the
         * corresponding command function callback.
         * @return true if an added command has been detected and handled by
         * callback.
         * @return false if no added command has been detected.
         */
        bool run();

        /**
         * @brief Let the Command Line Interface run an execution iteration to
         * check for any incoming command from the CLI and get it.
         * @param cli_result Pointer to get the last received CLI command and
         * arguments.
         * @return true if nay command has been received.
         * @return false If no command has been received.
         */
        bool manage(t_cli_result* cli_result);

        /**
         * @brief CLI print a text with format support.
         * @param str The text to be printed.
         * @param ... Format arguments variables.
         */
        void printf(const char* str, ...);

        /**
         * @brief Internal builtin "help" command callback.
         * @param argc Number of arguments.
         * @param argv Pointers array of arguments.
         */
        void cmd_help(int argc, char* argv[]);

    /*************************************************************************/

    /* Private Attributes */

    private:

        /**
         * @brief CLI initialized flag.
         */
        bool initialized;

        /**
         * @brief Number of bytes received through the CLI interface.
         */
        uint32_t received_bytes;

        /**
         * @brief Store if the builtin "help" command has been setup.
         */
        bool use_builtin_help_cmd;

        /**
         * @brief Current number of commands added to the CLI through add()
         * function.
         */
        uint8_t num_added_commands;

        /**
         * @brief Array of commands that are added to be handle through
         * callbacks by the add() function.
         */
        t_cmd_cb_info added_commands[MINBASECLI_MAX_CMD_TO_ADD];

        /**
         * @brief Last received command result.
         */
        t_cli_result cli_result;

        /**
         * @brief CLI data reception buffer.
         */
        char rx_read[MINBASECLI_MAX_READ_SIZE];

        /**
         * @brief Format conversion buffer for printf.
         */
        char print_array[MINBASECLI_MAX_PRINT_SIZE];

    /*************************************************************************/

    /* Private Methods */

    private:

        /**
         * @brief  Set attributes of a t_cli_result element to default null
         * values.
         * @param  cli_result The t_cli_result element to setup.
         */
        void set_default_result(t_cli_result* cli_result);

        /**
         * @brief  Check if needed CLI interface is initialized.
         * @return If interface is initialized (true or false).
         */
        bool iface_is_not_initialized();

        /**
         * @brief  Return the current number of bytes received by
         * iface_read_data().
         * @return The number of bytes read.
         */
        uint32_t get_received_bytes();

        /**
         * @brief  Check and read data from the CLI internal interface until
         * EOL.
         * @param  rx_read Read buffer to store the read data.
         * @param  rx_read_size Max size of read buffer to be filled by read
         * data.
         * @return If end of line was received (true/false).
         */
        bool iface_read_data(char* rx_read, const size_t rx_read_size);

        /**
         * @brief  Count the number of words inside a string.
         * @param  str_in Input string from where to count words.
         * @param  str_in_len Number of characters in "str_in".
         * @return The number of words in the string.
         */
        uint32_t str_count_words(const char* str_in, const size_t str_in_len);

        /**
         * @brief  Get substring from array until a specific character or end
         * of string.
         * @param  str Input string from where to get the substring.
         * @param  str_len Number of characters in "str".
         * @param  until_c Get substring until this character.
         * @param  str_read Buffer to store the read substring.
         * @param  str_read_size Max size of read buffer.
         * @return If character "until_c" was found (true/false).
         */
        bool str_read_until_char(
            char* str,
            const size_t str_len,
            const char until_c,
            char* str_read,
            const size_t str_read_size
        );

        /**
         * @brief Print a string.
         * @param str The string to print.
         */
        void printstr(const char* str);

        /**
         * @brief  Convert a unsigned integer of 64 bits (uint64_t) into a
         * string array.
         * @param  num Unsigned integer to be converted.
         * @param  str Pointer to array that gonna store the converted result
         * string.
         * @param  str_size Size of converted string array.
         * @param  base Provided number base (binary, decimal, hexadecimal,
         * etc.).
         * @return Conversion result (false - fail; true - success).
         */
        bool u64toa(
            uint64_t number,
            char* str,
            const uint8_t str_max_size,
            const uint8_t base
        );

        /**
         * @brief  Convert a signed integer of 64 bits (int64_t) into a string
         * array.
         * @param  num Signed integer to be converted.
         * @param  str Pointer to array that gonna store the converted result
         * string.
         * @param  str_size Size of converted string array.
         * @param  base Provided number base (binary, decimal, hexadecimal,
         * etc.).
         * @return Conversion result (false - fail; true - success).
         */
        bool i64toa(
            int64_t number,
            char* str,
            const uint8_t str_max_size,
            const uint8_t base
        );

        /**
         * @brief Reverse string characters ("ABCD" -> "DCBA").
         * @param str Pointer to string to reverse and reversed string result.
         * @param length Number of characters of the string.
         * @return Operation result success/fail (true/false).
         */
        bool str_reverse(char* str, uint8_t length);
};

/*****************************************************************************/

#endif /* MINBASECLI_H_ */
