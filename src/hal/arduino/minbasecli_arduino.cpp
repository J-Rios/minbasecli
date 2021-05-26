
/**
 * @file    minbasecli.cpp
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

#ifdef ARDUINO

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_arduino.h"

// Device/Framework Libraries
#include <Arduino.h>

// Standard Libraries
#include <string.h>

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#if defined(USBCON) // Arduinos: Leonardo, Micro, MKR, etc
    #define _IFACE (Serial)
#else // Arduinos: UNO, MEGA, Nano, etc
    #define _IFACE (HardwareSerial)
#endif

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI::MINBASECLI()
{
    this->iface = NULL;
    this->initialized = false;
    this->received_bytes = 0;
    this->rx_read[0] = '\0';
}

/*****************************************************************************/

/* Public Methods */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  */
bool MINBASECLI::setup()
{
    hal_uart_setup();
    this->iface = &(_IFACE);
    this->initialized = true;
    return true;
}

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI::setup(void* iface)
{
    this->iface = iface;
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

    // Check if any command has been received
    if (iface_read_data(this->rx_read, SIMPLECLI_MAX_READ_SIZE) == false)
        return false;
    received_bytes = get_received_bytes();
    this->received_bytes = 0;

    // Get the command
    str_read_until_char(this->rx_read, received_bytes, ' ',
            cli_result->cmd, SIMPLECLI_MAX_CMD_LEN);

    // Shows the received command
    hal_iface_print("# ");
    hal_iface_println(this->rx_read);

    // Check number of command arguments
    cli_result->argc = str_count_words(this->rx_read, received_bytes);
    if (cli_result->argc == 0)
    {
        // Empty command (just and EOL character)
        return true;
    }
    cli_result->argc = cli_result->argc - 1;

    // Limit number of arguments to check
    if (cli_result->argc > SIMPLECLI_MAX_ARGV)
        cli_result->argc = SIMPLECLI_MAX_ARGV;

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
            snprintf(cli_result->argv[i], SIMPLECLI_MAX_ARGV_LEN,
                    "%s", ptr_data);
            break;
        }
        ptr_data = ptr_data + (ptr_argv - ptr_data) + 1;
        ptr_argv = ptr_argv + 1;

        // Get the argument
        str_read_until_char(ptr_argv, strlen(ptr_argv), ' ',
                cli_result->argv[i], SIMPLECLI_MAX_ARGV_LEN);
    }

    return true;
}

/*****************************************************************************/

/* Private Methods */

/**
  * @brief  Set attributes of a t_cli_result element to default null values.
  * @param  cli_result The t_cli_result element to setup.
  */
void MINBASECLI::set_default_result(t_cli_result* cli_result)
{
    cli_result->cmd[0] = '\0';
    for (uint8_t i = 0; i < SIMPLECLI_MAX_ARGV; i++)
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
  * @brief  Check and read data from the CLI internal interface with timeout.
  * @param  rx_read Read buffer to store the read data.
  * @param  rx_read_size Max size of read buffer to be filled by read data.
  * @return The number of bytes readed.
  */
size_t MINBASECLI::iface_read_data_t(char* rx_read, const size_t rx_read_size)
{
    size_t num_read_bytes = 0;
    unsigned long t0 = 0;
    unsigned long t1 = 0;

    // Read until timeout
    t0 = hal_millis();
    t1 = hal_millis();
    while (hal_millis() - t0 < SIMPLECLI_READ_TIMEOUT_MS)
    {
        // Break if no character received in 100ms
        if (hal_iface_available() == 0)
        {
            if (hal_millis() - t1 >= SIMPLECLI_READ_INTERCHAR_TIMEOUT_MS)
                break;
            continue;
        }

        // Break if read buffer is full
        if (num_read_bytes >= rx_read_size)
            break;

        rx_read[num_read_bytes] = hal_iface_read();
        num_read_bytes = num_read_bytes + 1;
        t1 = hal_millis();
    }

    // Check and ignore end of line characters
    if (num_read_bytes == 0)
        return 0;
    if (rx_read[num_read_bytes-1] == '\n')
        num_read_bytes = num_read_bytes - 1;
    if (num_read_bytes == 0)
        return 0;
    if (rx_read[num_read_bytes-1] == '\r')
        num_read_bytes = num_read_bytes - 1;

    // Close the read buffer
    if (num_read_bytes < rx_read_size)
        rx_read[num_read_bytes] = '\0';
    else if (num_read_bytes >= rx_read_size)
        rx_read[num_read_bytes-1] = '\0';

    return num_read_bytes;
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

/* Specific Device/Framework HAL functions */

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI::hal_millis()
{
    return ((uint32_t) (millis()));
}

/**
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI::hal_uart_setup()
{
    _IFACE* _Serial = (_IFACE*) this->iface;
    _Serial->begin(115200);
    return true;
}

/**
  * @brief  Print a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI::hal_iface_print(const char* str)
{
    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return;

    _IFACE* _Serial = (_IFACE*) this->iface;

    _Serial->print(str);
}

/**
  * @brief  Print line a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI::hal_iface_println(const char* str)
{
    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return;

    _IFACE* _Serial = (_IFACE*) this->iface;

    _Serial->println(str);
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI::hal_iface_available()
{
    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return 0;

    _IFACE* _Serial = (_IFACE*) this->iface;

    return ((size_t) (_Serial->available()));
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI::hal_iface_read()
{
    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return 0;

    _IFACE* _Serial = (_IFACE*) this->iface;

    return _Serial->read();
}

/*****************************************************************************/

#endif /* ARDUINO */
