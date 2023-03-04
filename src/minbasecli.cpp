
/**
 * @file    minbasecli.cpp
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

/* Constructor */

/**
 * @details
 * This constructor initializes all attributes of the CLI class.
 */
MINBASECLI::MINBASECLI()
{
    this->initialized = false;
    this->received_bytes = 0;
    this->use_builtin_help_cmd = false;
    this->num_added_commands = 0;
    this->cli_result.argc = 0U;
    for (int i = 0; i < MINBASECLI_MAX_ARGV; i++)
    {
        memset(this->cli_result.argv[i], (int)('\0'),
                MINBASECLI_MAX_ARGV_LEN - 1U);
    }
    memset(this->cli_result.cmd, (int)('\0'), MINBASECLI_MAX_CMD_LEN - 1U);
    for (int i = 0; i < MINBASECLI_MAX_CMD_TO_ADD; i++)
    {
        this->added_commands[i].command[0] = '\0';
        this->added_commands[i].description[0] = '\0';
        this->added_commands[i].callback = NULL;
    }
    memset(this->rx_read, (int)('\0'), MINBASECLI_MAX_READ_SIZE - 1U);
    memset(this->print_array, (int)('\0'), MINBASECLI_MAX_PRINT_SIZE - 1U);
}

/*****************************************************************************/

/* Public Methods */

/**
 * @details
 * This function call to specific Hardware Abstraction Layer CLI interface
 * setup function to initialize the interface, and set the CLI initialized
 * flag.
 */
bool MINBASECLI::setup(void* iface, const uint32_t baud_rate)
{
    if (hal_setup(iface, baud_rate) == true)
        { this->initialized = true; }
    return this->initialized;
}

/**
 * @details
 * This function check if provided arguments are valid and if they there is
 * enough space in the added commands array to store a new command callback
 * info, and add a new command callback element to the list according to
 * provided arguments.
 */
bool MINBASECLI::add_cmd(const char* command, t_command_callback callback,
        const char* description)
{
    t_cmd_cb_info cmd_cb_info;
    size_t cmd_len = 0U;
    size_t cmd_description_len = 0U;

    // Check if there is enough space to add a new command
    if (num_added_commands >= MINBASECLI_MAX_CMD_TO_ADD)
        return false;

    // Check if provided argument are valid
    if ( (command == NULL) || (callback == NULL) || (description == NULL) )
        return false;

    // Check and limit provided arguments lengths
    cmd_len = strlen(command);
    cmd_description_len = strlen(description);
    if (cmd_len >= MINBASECLI_MAX_CMD_LEN)
        cmd_len = MINBASECLI_MAX_CMD_LEN - 1U;
    if (cmd_description_len >= MINBASECLI_MAX_CMD_DESCRIPTION)
        cmd_description_len = MINBASECLI_MAX_CMD_DESCRIPTION - 1U;

    // Create a new t_cmd_cb_info element with provided command data
    strncpy(cmd_cb_info.command, command, cmd_len);
    cmd_cb_info.command[cmd_len] = '\0';
    strncpy(cmd_cb_info.description, description, cmd_description_len);
    cmd_cb_info.description[cmd_description_len] = '\0';
    cmd_cb_info.callback = callback;

    // Set to use the internal "help" command if it is the first command added
    if (num_added_commands == 0U)
        use_builtin_help_cmd = true;

    // If requested to add a custom "help" command, don't use the builtin one
    if (strcmp(command, CMD_HELP) == 0)
        use_builtin_help_cmd = false;

    // Add the new command to the list of binded commands and increase the
    // number of added commands
    added_commands[num_added_commands] = cmd_cb_info;
    num_added_commands = num_added_commands + 1U;

    return true;
}

/**
 * @details
 * This function calls to manage the CLI to check if there is any new command
 * received available to be handled, then check if the received command is one
 * of the added inside CLI component to be handle through a callback, and call
 * to the corresponding callback for it.
 */
bool MINBASECLI::run()
{
    bool cmd_found = false;

    // Do nothing if there is no added commands
    if (num_added_commands == 0U)
        return false;

    // Check if there is any new command received by the CLI
    if (manage(&cli_result) == false)
        return false;

    // Compose array of pointer for arguments
    char* ptr_argv[MINBASECLI_MAX_ARGV];
    for (int i = 0; i < MINBASECLI_MAX_ARGV; i++)
        ptr_argv[i] = cli_result.argv[i];

    // If no custom "help" command is set
    if (use_builtin_help_cmd)
    {
        // For "help" command, call the builtin "help" function
        if (strcmp(cli_result.cmd, CMD_HELP) == 0U)
        {
            cmd_help(cli_result.argc, ptr_argv);
            return true;
        }
    }

    // Check if the command is added in the callback handle list
    for (uint8_t i = 0U; i < num_added_commands; i++)
    {
        // If command is found in the callbacks list, call to the callback
        if (strcmp(cli_result.cmd, added_commands[i].command) == 0U)
        {
            // Call to command callback
            added_commands[i].callback(this, cli_result.argc, ptr_argv);
            cmd_found = true;
            break;
        }
    }

    return cmd_found;
}

/**
 * @details
 * This function checks and get any received data from the CLI interface and
 * parse to handle it as a command with arguments, populating the t_cli_result
 * element to be returned as reference at the end of the function.
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
 * @details
 * This function implements a basic reduced version of the standard C STDIO
 * printf() function. It loops over each provided fstr characters sending it
 * to the CLI interface and checking for some format pattern in the string to
 * apply a data conversion of provided variables and writing it then. It uses
 * variadic function parameters and API to get the undefined number of
 * arguments that the function can get.
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

        // Increase format string pointer to next char and check end of string
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

        // Unsupported format
        else
        {
            // Do nothing
        }

        // Increase format string pointer to next character
        fstr = fstr + 1;
    }

    va_end(lst);
}

/**
 * @details
 * This function is called when a "help" command is received through the CLI
 * if any command has been added to be handled through a callback and the
 * "help" command has not been added. It shows the list of commands that are
 * added and the descriptions of them.
 */
void MINBASECLI::cmd_help(int argc, char* argv[])
{
    // Do nothing if there is no added commands
    if (num_added_commands == 0U)
        return;

    // Shows each added command descriptions
    this->printf("\nAvailable commands:\n\n");

    // Shows help description info
    if (use_builtin_help_cmd)
        this->printf("%s - %s\n", CMD_HELP, CMD_HELP_DESCRIPTION);

    // Shows all added command descriptions
    for (uint8_t i = 0U; i < num_added_commands; i++)
    {
        this->printf("%s - %s\n", added_commands[i].command,
                added_commands[i].description);
    }

    this->printf("\n");
}

/*****************************************************************************/

/* Private Methods */

/**
 * @details
 * This function loop and print each character of the provided string until an
 * end of string null character is found ('\0').
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
 * @details
 * This function copy the provided string in a temporary array element to then
 * start reversing each character and update the provided str array.
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
 * @details
 * This function checks if provided string buffer has enough size to store the
 * maximum unsigned integer value on it, and convert each individual digit of
 * the number to characters that are store in the str array. The conversion is
 * done from most significant digit to less one to allow easily append at the
 * end the minus signal in case the number was negative, then the string is
 * reversed to get the correct string number in the array.
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
 * @details
 * This function checks if provided string buffer has enough size to store the
 * maximum signed integer value on it, differentiate between positive or
 * negative numbers, and convert each individual digit of the number to
 * characters that are store in the str array. The conversion is done from
 * most significant digit to less one to allow easily append at the end the
 * minus signal in case the number was negative, then the string is reversed
 * to get the correct string number in the array.
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
 * @details
 * This function set t_cli_result attributes to default clear/zero values.
 */
void MINBASECLI::set_default_result(t_cli_result* cli_result)
{
    cli_result->cmd[0] = '\0';
    for (uint8_t i = 0; i < MINBASECLI_MAX_ARGV; i++)
        cli_result->argv[i][0] = '\0';
    cli_result->argc = 0;
}

/**
 * @details
 * This function check if CLI interface initialized flag is off.
 */
bool MINBASECLI::iface_is_not_initialized()
{
    return (this->initialized == false);
}

/**
 * @details
 * This function just return the current number of bytes received by
 * iface_read_data().
 */
uint32_t MINBASECLI::get_received_bytes()
{
    return (this->received_bytes);
}

/**
 * @details
 * This function get each received byte from the CLI interface, counting the
 * number of received bytes and storing them into the reception buffer array
 * until an End-Of-Line character is detected. It differentiates between CR,
 * LF and CRLF, and get rid off this characters from the read buffer.
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
 * @details
 * This function loop through provided str_in string characters searching for
 * "X Y" pattern to increase the counter of words until the end of the string.
 */
uint32_t MINBASECLI::str_count_words(const char* str_in,
        const size_t str_in_len)
{
    uint32_t n = 1;

    // Check if string is empty of just has 1 character
    if (str_in_len == 0)
        return 0;
    if (str_in[0] == '\0')
        return 0;
    if (str_in_len == 1)
        return 1;

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
 * @details
 * This function loop for each character of the provided string checking for
 * the requested "until" character while copying each character into the
 * str_read array.
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
