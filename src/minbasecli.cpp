
/**
 * @file    minbasecli.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    02-04-2022
 * @version 1.1.1
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

/* Libraries */

// Header Interface
#include "minbasecli.h"

// Standard Libraries
#include <string.h>
#include <stdarg.h>

/*****************************************************************************/

/* Constants & Defines */

// Maximum string length to store a 64 bit signed/unsigned number
static const uint8_t MAX_64_BIT_NUM_STR_LENGTH = 21;

/*****************************************************************************/

/* In-Scope Function Prototypes */

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI::MINBASECLI()
{
    this->initialized = false;
    this->received_bytes = 0;
    this->rx_read[0] = '\0';
}

/*****************************************************************************/

/* Public Methods */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI::setup(void* iface, const uint32_t baud_rate)
{
    hal_setup(iface, baud_rate);
    this->initialized = true;
    return true;
}

/**
  * @brief  Manage Command Line Interface by checking and reading commands.
  * @param  cli_result Pointer to check result.
  * @return If any command was received (true/false).
  */
bool MINBASECLI::manage(t_cli_result* cli_result)
{
    uint32_t received_bytes = 0;

    // Set default null result
    set_default_result(cli_result);

    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return false;

    // Check if any command has been received
    if (iface_read_data(this->rx_read, MINBASECLI_MAX_READ_SIZE) == false)
        return false;
    received_bytes = get_received_bytes();
    this->received_bytes = 0;

    // Get the command
    str_read_until_char(this->rx_read, received_bytes, ' ',
            cli_result->cmd, MINBASECLI_MAX_CMD_LEN);

    // Shows the received command
    this->printf("# %s\n", this->rx_read);

    // Check number of command arguments
    cli_result->argc = str_count_words(this->rx_read, received_bytes);
    if (cli_result->argc == 0)
    {
        // Empty command (just and EOL character)
        return true;
    }
    cli_result->argc = cli_result->argc - 1;

    // Limit number of arguments to check
    if (cli_result->argc > MINBASECLI_MAX_ARGV)
        cli_result->argc = MINBASECLI_MAX_ARGV;

    // Get Arguments
    char* ptr_data = this->rx_read;
    char* ptr_argv = NULL;
    for (uint8_t i = 0; i < cli_result->argc; i++)
    {
        // Point to next argument
        ptr_argv = strstr(ptr_data, " ");
        if (ptr_argv == NULL)
        {
            // No ' ' character found, so it is last command, lets get it
            strncpy(cli_result->argv[i], ptr_data, strlen(ptr_data));
            cli_result->argv[i][MINBASECLI_MAX_ARGV_LEN-1] = '\0';
            break;
        }
        ptr_data = ptr_data + (ptr_argv - ptr_data) + 1;
        ptr_argv = ptr_argv + 1;

        // Get the argument
        str_read_until_char(ptr_argv, strlen(ptr_argv), ' ',
                cli_result->argv[i], MINBASECLI_MAX_ARGV_LEN);
    }

    return true;
}

/**
  * @brief  Print a format string through the CLI.
  * @param  fstr String format to print.
  */
void MINBASECLI::printf(const char* fstr, ...)
{
    va_list lst;

    va_start(lst, fstr);

    while (*fstr != '\0')
    {
        // Just print the string if start format symbol has not been reach
        if (*fstr != '%')
        {
            hal_iface_print((uint8_t)(*fstr));
            fstr = fstr + 1;
            continue;
        }

        // Increase format string pointer to next char, and check end of string
        fstr = fstr + 1;
        if (*fstr == '\0')
            break;

        /* Check for format to apply */

        // Format String
        if (*fstr == 's')
            printstr(va_arg(lst, char*));

        // Format Character
        else if (*fstr == 'c')
            hal_iface_print((uint8_t)(va_arg(lst, int)));

        // Format Unsigned integer
        else if (*fstr == 'u')
        {
            // Convert unsigned type argument of variadic list into string
            if (u64toa((uint64_t)(va_arg(lst, unsigned)), print_array,
                    MINBASECLI_MAX_PRINT_SIZE, 10) == false)
            {
                // Increase format string pointer to next character
                fstr = fstr + 1;
                continue;
            }

            // Print the converted value string
            printstr(print_array);
        }

        // Format Signed Integer
        else if ((*fstr == 'i') || (*fstr == 'd'))
        {
            // Convert integer type argument of variadic list into string
            if (i64toa((int64_t)(va_arg(lst, int)), print_array,
                    MINBASECLI_MAX_PRINT_SIZE, 10) == false)
            {
                // Increase format string pointer to next character
                fstr = fstr + 1;
                continue;
            }

            // Print the converted value string
            printstr(print_array);
        }

        // Format Hexadecimal
        // Note: specify leading zeroes is not supported
        else if ((*fstr == 'x') || (*fstr == 'X'))
        {
            // Convert unsigned type argument of variadic list into string
            if (u64toa((uint64_t)(va_arg(lst, unsigned)), print_array,
                    MINBASECLI_MAX_PRINT_SIZE, 16) == false)
            {
                // Increase format string pointer to next character
                fstr = fstr + 1;
                continue;
            }

            // Print a leading zero if hexadecimal string length is odd
            if ((strlen(print_array) % 2) != 0)
                this->hal_iface_print('0');

            // Print the converted value string
            printstr(print_array);
        }

        // Unssuported format
        else
        {
            // Do nothing
        }

        // Increase format string pointer to next character
        fstr = fstr + 1;
    }

    va_end(lst);
}

/*****************************************************************************/

/* Private Methods */

/**
 * @brief Print a string.
 * @param str The string to print.
 */
void MINBASECLI::printstr(const char* str)
{
    while (*str != '\0')
    {
        hal_iface_print((uint8_t)(*str));
        str = str + 1;
    }
}

/**
 * @brief Reverse string characters ("ABCD" -> "DCBA").
 * @param str Pointer to string to reverse (also reversed string result).
 * @param length Number of characters of the string.
 * @return Operation result success/fail (true/false).
 */
bool MINBASECLI::str_reverse(char* str, uint8_t length)
{
    char tmp[MAX_64_BIT_NUM_STR_LENGTH];
    int8_t start = 0;
    int8_t end = length - 1;

    if (length == 0)
        return false;

    memcpy(tmp, str, length);
    while (start < end)
    {
        *(str + start) = *(tmp + end);
        *(str + end) = *(tmp + start);
        start = start + 1;
        end = end - 1;
    }

    return true;
}

/**
  * @brief  Convert a unsigned integer of 64 bits (uint64_t) into a string.
  * @param  num Unsigned integer to be converted.
  * @param  str Pointer to array that gonna store the converted result string.
  * @param  str_size Size of converted string array.
  * @param  base Provided number base (binary, decimal, hexadecimal, etc.).
  * @return Conversion result (false - fail; true - success).
  */
bool MINBASECLI::u64toa(uint64_t num, char* str,
        const uint8_t str_size, const uint8_t base)
{
    uint64_t tmp;
    uint8_t i = 0;

    // Check if string buffer max size is large enough for 64 bits num
    if (str_size < MAX_64_BIT_NUM_STR_LENGTH)
        return false;

    // Check for number 0
    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return true;
    }

    // Process individual digits
    while (num != 0)
    {
        tmp = num % base;
        if (tmp > 9)
            str[i] = (tmp - 10) + 'a';
        else
            str[i] = tmp + '0';
        num = num / base;
        i = i + 1;
    }

    // Null terminate string
    str[i] = '\0';

    // Reverse the string
    str_reverse(str, i);

    return true;
}

/**
  * @brief  Convert a signed integer of 64 bits (int64_t) into a string.
  * @param  num Signed integer to be converted.
  * @param  str Pointer to array that gonna store the converted result string.
  * @param  str_size Size of converted string array.
  * @param  base Provided number base (binary, decimal, hexadecimal, etc.).
  * @return Conversion result (false - fail; true - success).
  */
bool MINBASECLI::i64toa(int64_t num, char* str,
        const uint8_t str_size, const uint8_t base)
{
    uint64_t tmp;
    uint8_t i = 0;
    bool negative_num = false;

    // Check if string buffer max size is large enough for 64 bits num
    if (str_size < MAX_64_BIT_NUM_STR_LENGTH)
        return false;

    // Check for number 0
    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return true;
    }

    // Check for negative number when decimal base
    if (num < 0 && base == 10)
    {
        negative_num = true;
        num = (-num);
    }

    // Process individual digits
    while (num != 0)
    {
        tmp = num % base;
        if (tmp > 9)
            str[i] = (tmp - 10) + 'a';
        else
            str[i] = tmp + '0';
        num = num / base;
        i = i + 1;
    }

    // If number is negative, append '-'
    if (negative_num)
        str[i++] = '-';

    // Null terminate string
    str[i] = '\0';

    // Reverse the string
    str_reverse(str, i);

    return true;
}

/**
  * @brief  Set attributes of a t_cli_result element to default null values.
  * @param  cli_result The t_cli_result element to setup.
  */
void MINBASECLI::set_default_result(t_cli_result* cli_result)
{
    cli_result->cmd[0] = '\0';
    for (uint8_t i = 0; i < MINBASECLI_MAX_ARGV; i++)
        cli_result->argv[i][0] = '\0';
    cli_result->argc = 0;
}

/**
  * @brief  Check if needed CLI interface is initialized.
  * @return If interface is initialized (true or false).
  */
bool MINBASECLI::iface_is_not_initialized()
{
    return (this->initialized == false);
}

/**
  * @brief  Return the current number of bytes received by iface_read_data().
  * @return The number of bytes readed.
  */
uint32_t MINBASECLI::get_received_bytes()
{
    return (this->received_bytes);
}

/**
  * @brief  Check and read data from the CLI internal interface until EOL.
  * @param  rx_read Read buffer to store the read data.
  * @param  rx_read_size Max size of read buffer to be filled by read data.
  * @return If end of line was received (true/false).
  */
bool MINBASECLI::iface_read_data(char* rx_read, const size_t rx_read_size)
{
    // While there is any data incoming from CLI interface
    while (hal_iface_available())
    {
        // Read a byte
        rx_read[this->received_bytes] = hal_iface_read();
        this->received_bytes = this->received_bytes + 1;

        // Check for read buffer full
        if (this->received_bytes >= rx_read_size-1)
        {
            rx_read[rx_read_size-1] = '\0';
            return true;
        }

        // Check for LF
        if (rx_read[this->received_bytes-1] == '\n')
        {
            rx_read[this->received_bytes-1] = '\0';
            this->received_bytes = this->received_bytes - 1;
            return true;
        }

        // Check for CR or CRLF
        if (rx_read[this->received_bytes-1] == '\r')
        {
            // Check for CRLF
            if (hal_iface_available())
            {
                // Read next byte
                rx_read[this->received_bytes] = hal_iface_read();
                this->received_bytes = this->received_bytes + 1;

                if (rx_read[this->received_bytes-1] == '\n')
                {
                    rx_read[this->received_bytes-2] = '\0';
                    this->received_bytes = this->received_bytes - 2;
                    return true;
                }
            }

            // CR
            rx_read[this->received_bytes-1] = '\0';
            this->received_bytes = this->received_bytes - 1;
            return true;
        }
    }

    return false;
}

/**
  * @brief  Count the number of words inside a string.
  * @param  str_in Input string from where to count words.
  * @param  str_in_len Number of characters in "str_in".
  * @return The number of words in the string.
  */
uint32_t MINBASECLI::str_count_words(const char* str_in,
        const size_t str_in_len)
{
    uint32_t n = 1;

    // Check if string is empty
    if (str_in[0] == '\0')
        return 0;

    // Check for character occurrences
    for (size_t i = 1; i < str_in_len; i++)
    {
        // Check if end of string detected
        if (str_in[i] == '\0')
            break;

        // Check if pattern "X Y", "X\rY" or "X\nY" does not meet
        if ((str_in[i] != ' ') && (str_in[i] != '\r') && (str_in[i] != '\n'))
            continue;
        if ((str_in[i-1] == ' ') || (str_in[i-1] == '\r') ||
                (str_in[i-1] == '\n'))
            continue;
        if ((str_in[i+1] == ' ') || (str_in[i+1] == '\r') ||
                (str_in[i+1] == '\n'))
            continue;
        if (str_in[i+1] == '\0')
            continue;

        // Pattern detected, increase word count
        n = n + 1;
    }

    return n;
}

/**
  * @brief  Get substring from array until a specific character or end of
  * string.
  * @param  str Input string from where to get the substring.
  * @param  str_len Number of characters in "str".
  * @param  until_c Get substring until this character.
  * @param  str_read Buffer to store the read substring.
  * @param  str_read_size Max size of read buffer.
  * @return If character "until_c" was found (true/false).
  */
bool MINBASECLI::str_read_until_char(char* str, const size_t str_len,
        const char until_c, char* str_read, const size_t str_read_size)
{
    size_t i = 0;
    bool found = false;

    str_read[0] = '\0';
    while (i < str_len)
    {
        if (str[i] == until_c)
        {
            found = true;
            break;
        }
        if (i < str_read_size)
            str_read[i] = str[i];
        i = i + 1;
    }
    str_read[str_read_size-1] = '\0';
    if (i < str_read_size)
        str_read[i] = '\0';

    return found;
}

/*****************************************************************************/
