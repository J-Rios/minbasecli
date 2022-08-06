
/**
 * @file    minbasecli.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    16-07-2022
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
 * This constructor initializes all attributtes of the CLI class.
 */
MINBASECLI::MINBASECLI()
{
    this->initialized = false;
    this->received_bytes = 0U;
    this->use_builtin_help_cmd = false;
    this->num_added_commands = 0U;
    this->cli_result.argc = 0U;
    cursor_position = 0U;
    for (int i = 0; i < MINBASECLI_MAX_ARGV; i++)
    {
        memset(this->cli_result.argv[i], (int)('\0'), \
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
    memset(this->control_sequence_cmd, 0x00, MINBASECLI_MAX_CTRL_SEQ_LEN - 1U);
    control_sequence_cmd_i = 0U;
}

/*****************************************************************************/

/* Public Methods */

/**
 * @details
 * This function call to specific Hardware Abstraction Layer CLI interface setup
 * function to initializate the interface, and set the CLI initialized flag.
 */
bool MINBASECLI::setup(void* iface, const uint32_t baud_rate)
{
    hal_setup(iface, baud_rate);
    this->initialized = true;
    return true;
}

/**
 * @details
 * This function check if provided arguments are valid and if they there is
 * enough space in the added commands array to store a new command callback
 * info, and add a new command callback element to the list according to
 * provided arguments.
 */
bool MINBASECLI::add_cmd(const char* command,
        void (*callback)(int argc, char* argv[]),
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
 * received avaliable to be handled, then check if the received command is one
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
            added_commands[i].callback(cli_result.argc, ptr_argv);
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
 * apply a data conversion of provided variables and writting it then. It uses
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

/**
 * @details
 * This function is called when a "help" command is received through the CLI if
 * any command has been added to be handled through a callback and the "help"
 * command has not been added. It shows the list of commands that are added and
 * the descriptions of them.
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
    static bool last_byte_was_eol = false;
    uint8_t rx_byte = 0U;

    // Do nothing if there is no new data received from CLI interface
    if (hal_iface_available() == 0U)
        return false;

    // Read received byte from CLI interface
    rx_byte = hal_iface_read();

    // Check for end of line (LF or CR)
    if (((char)(rx_byte) == '\n') || ((char)(rx_byte) == '\r') )
    {
        if (last_byte_was_eol == true)
            return false;
        last_byte_was_eol = true;
        rx_read[received_bytes] = '\0';
printf("\n---\n%s\n\nlength: %d\ncursor: %d\n\n---\n", rx_read, int(received_bytes), int(cursor_position));
        this->printf("\n");
        cursor_position = 0U;
        return true;
    }
    last_byte_was_eol = false;

    // Add byte to current control sequence buffer
    control_sequence_cmd[control_sequence_cmd_i] = rx_byte;

    // Increase number of control sequence buffer bytes received and limit the
    // overflow of it
    control_sequence_cmd_i = control_sequence_cmd_i + 1U;
    if (control_sequence_cmd_i >= MINBASECLI_MAX_CTRL_SEQ_LEN)
    {
        control_sequence_cmd_i = 0U;
        memset(control_sequence_cmd, 0x00, MINBASECLI_MAX_CTRL_SEQ_LEN - 1U);
    }

    // Check and handle any supported escape control sequence command
    if (handle_control_sequence(control_sequence_cmd, &control_sequence_cmd_i))
    {
        control_sequence_cmd_i = 0U;
        memset(control_sequence_cmd, 0x00, MINBASECLI_MAX_CTRL_SEQ_LEN - 1U);
        return false;
    }

    // Don't print any character if a escape control sequence is started
    if ( (control_sequence_cmd[0] == ASCII_ESC) && \
         (control_sequence_cmd[1] == CONTROL_SEQUENCE_INTRODUCER) )
    {
        return false;
    }

    // Don't print any non-printable character
    if (is_ascii_printable(rx_byte) == false)
    {
        #if 0 // For development debug
            this->printf("No-Printable ASCII CODE: %d\n", int(rx_byte));
        #endif
        return false;
    }

    // Clear current Control Sequence buffer
    control_sequence_cmd_i = 0U;
    memset(control_sequence_cmd, 0x00, MINBASECLI_MAX_CTRL_SEQ_LEN - 1U);

    /* Handle ASCII Printable Bytes */

    // Check for read buffer full
    if ( (received_bytes + 1U) >= (rx_read_size - 1U) )
    {
        rx_read[rx_read_size - 1U] = '\0';
        return false;
    }

    // For cursor between previous received data
    if ( (received_bytes > 0U) && (cursor_position < (received_bytes - 1U)) )
    {
        // Shift right the read buffer from cursor position (removing
        // current cursor character from the buffer)
        array_shift_from_pos((uint8_t*)(rx_read), received_bytes,
            cursor_position, SHIFT_RIGHT);
    }

    // Store new data in the buffer cursor position
    rx_read[cursor_position] = rx_byte;
    received_bytes = received_bytes + 1U;

    // Print received characarter
    this->printf("%c", rx_read[cursor_position]);
    cursor_position = cursor_position + 1U;

    // Check and overwrite following characters (if needed)
    rewrite_shifted_buffer(cursor_position, received_bytes, false);

    return false;
}

/**
 * @details
 * This function check for control sequence commands from the received bytes in
 * the special bytes buffer and handle them.
 */
bool MINBASECLI::handle_control_sequence(uint8_t* control_sequence,
        uint8_t* control_sequence_len)
{
    static const uint8_t CTRL_SEQ_MOVE_CURSOR_RIGHT  = 67U;  // 'C'
    static const uint8_t CTRL_SEQ_MOVE_CURSOR_LEFT   = 68U;  // 'D'
    static const uint8_t CTRL_SEQ_DELETE_0           = 51U;  // '3'
    static const uint8_t CTRL_SEQ_DELETE_1           = 126U; // VT220/XTERM
    static const uint8_t KEYCODE_BACKSPACE           = 127U; // VT220/XTERM

    // Ignore if nothing has been received yet
    if (*control_sequence_len == 0U)
        return false;

    // Check and handle backspace key
    if (control_sequence[0] == KEYCODE_BACKSPACE)
    {
        backspace_key();
        return true;
    }

    // Ignore if not enough bytes has been received for the next checks
    if (*control_sequence_len < 3U)
        return false;

    // Check if no Control Sequence has been fully received yet
    if ( (control_sequence[0] != ASCII_ESC) || \
         (control_sequence[1] != CONTROL_SEQUENCE_INTRODUCER) )
    {
        return false;
    }

    // Check for move cursor right
    if (control_sequence[2] == CTRL_SEQ_MOVE_CURSOR_RIGHT)
    {
        move_cursor_right();
        return true;
    }

    // Check for move cursor left
    if (control_sequence[2] == CTRL_SEQ_MOVE_CURSOR_LEFT)
    {
        move_cursor_left();
        return true;
    }

    // Ignore if not enough bytes has been received for the next checks
    if (*control_sequence_len < 4U)
        return false;

    // Check and handle delete key
    if ( (control_sequence[2] == CTRL_SEQ_DELETE_0) && \
         (control_sequence[3] == CTRL_SEQ_DELETE_1) )
    {
        delete_key();
        return true;
    }

    // Unknown sequence, clear current one
    memset(control_sequence, 0x00, *control_sequence_len);
    *control_sequence_len = 0U;
    return false;
}

/**
 * @details
 * This function is used to rewrite a shifted read buffer when a character is
 * written or removed from an intermidiate position (cursor not at the end of
 * the written characters). It checks if the cursor is not in the last position
 * and rewrite the read buffer data to the terminal from cursor position to the
 * end, then the last character is removed by writing an space and at the end
 * the cursor is shifted left back again to the initial position.
 */
bool MINBASECLI::rewrite_shifted_buffer(const uint32_t cursor_pos,
        const uint32_t num_bytes, const bool clear_last_char)
{
    uint32_t num_shifts = 0U;

    // Do nothing if cursor is in last position (most right char)
    if (cursor_pos >= num_bytes)
        return false;
    if (num_bytes == 0U)
        return false;

    // Overwrite following characters
    for (uint32_t i = cursor_pos; i < num_bytes; i++)
    {
        this->printf("%c", rx_read[i]);
        num_shifts = num_shifts + 1U;
    }

    // Clear previous last (most right) character
    if (clear_last_char)
    {
        this->printf(" ");
        num_shifts = num_shifts + 1U;
    }

    // Move the cursor to the left the same number of characters
    for (uint32_t i = 0; i < num_shifts; i++)
        move_cursor_left(false);

    return true;
}

/**
 * @details
 * This function removes previous character of the read buffer from current
 * cursor position using the memmove() function to shift-left the following
 * characters, then moves the cursor position back 1 time by sending the
 * corresponding Escape Sequence command ("\e[1D"), and overwrite from that
 * position until the end of the string. After that, the cursor is moved again
 * the same number of shifted characters time to position it in the expected
 * location of the character that was removed.
 */
bool MINBASECLI::backspace_key()
{
    // Do nothing if there is no characters
    if ( (cursor_position == 0U) || (received_bytes == 0U) )
        return true;

    // Move cursor back 1 position (reduce cursor position)
    // Overwrite last character with space to remove it from the screen
    if (move_cursor_left() == false)
        return false;
    this->printf(" ");
    cursor_position = cursor_position + 1U;
    if (move_cursor_left() == false)
        return false;

    // Shift left the read buffer from cursor position (removing current cursor
    // character from the buffer)
    array_shift_from_pos((uint8_t*)(rx_read), received_bytes, cursor_position,
            SHIFT_LEFT);
    rx_read[received_bytes - 1U] = '\0';

    // Decrease the number of characters on the buffer
    // If it was the last character, end here
    received_bytes = received_bytes - 1U;
    if (received_bytes == 0U)
        return true;

    // Overwrite following characters (if needed)
    rewrite_shifted_buffer(cursor_position, received_bytes, true);

    return true;
}

/**
 * @details
 * This function removes following character of the read buffer from current
 * cursor position using the memmove() function to shift-left the following
 * characters, and then overwrite from that position until the end of the
 * string. After that, the cursor is moved again the same number of shifted
 * characters time to position it in the expected location of the character
 * that was removed.
 */
bool MINBASECLI::delete_key()
{
    // Do nothing if the cursor is already in the last printed character
    if ( (cursor_position >= this->received_bytes) || (received_bytes == 0U) )
        return false;

    // Overwrite next character with space to remove it from the screen
    this->printf(" ");
    cursor_position = cursor_position + 1U;
    if (move_cursor_left() == false)
        return false;

    // Shift left the read buffer from cursor position (removing current cursor
    // character from the buffer)
    array_shift_from_pos((uint8_t*)(rx_read), received_bytes, cursor_position,
            SHIFT_LEFT);
    rx_read[received_bytes - 1U] = '\0';

    // Decrease the number of characters on the buffer
    // If it was the last character, end here
    received_bytes = received_bytes - 1U;
    if (received_bytes == 0U)
        return true;

    // Overwrite following characters (if needed)
    rewrite_shifted_buffer(cursor_position, received_bytes, true);

    return true;
}

/**
 * @details
 * This function check if the cursor is not in the first character, then reduce
 * the cursor position attribute value and send the move cursor left Escape
 * Sequence ("\e[1D").
 */
bool MINBASECLI::move_cursor_left(const bool modify_cursor_position_var)
{
    // Reduce the cursor position index
    if (modify_cursor_position_var)
    {
        // Do nothing if the cursor is already in the first position
        if (cursor_position == 0U)
            return false;

        cursor_position = cursor_position - 1U;
    }

    // Send the cursor move left Escape sequence
    this->printf("\e[1D");

    return true;
}

/**
 * @details
 * This function check if the cursor is not in the last character, then increase
 * the cursor position attribute value and send the move cursor right Escape
 * Sequence ("\e[1C").
 */
bool MINBASECLI::move_cursor_right(const bool modify_cursor_position_var)
{
    // Do nothing if the cursor is already in the last printed character
    if (cursor_position >= received_bytes)
        return false;

    // Increase the cursor position index
    if (modify_cursor_position_var)
        cursor_position = cursor_position + 1U;

    // Send the cursor move right Escape sequence
    this->printf("\e[1C");

    return true;
}

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
 * This function check if the provided byte value is in the range of a printable
 * basic (and extended) ASCII encode character.
 * Check the next website for range information:
 * https://www.asciitable.com/
 */
bool MINBASECLI::is_ascii_printable(const uint8_t byte,
        const bool check_extended)
{
    static const uint8_t ASCII_BASIC_MIN_VAL = 32U;
    static const uint8_t ASCII_BASIC_MAX_VAL = 126U;
    static const uint8_t ASCII_EXTEND_MIN_VAL = 128U;
    static const uint8_t ASCII_EXTEND_MAX_VAL = 254U;

    // Basic ASCII
    if ( (byte >= ASCII_BASIC_MIN_VAL) && (byte <= ASCII_BASIC_MAX_VAL) )
        return true;

    // Extended ASCII
    if (check_extended)
    {
        if (( byte >= ASCII_EXTEND_MIN_VAL) && (byte <= ASCII_EXTEND_MAX_VAL) )
            return true;
    }

    return false;
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

/**
 * @details
 * This function shift all bytes of an array, from provided array position,
 * removing the current position byte from it. It checks that the given
 * arguments are valid, then uses the memmove function to move all the bytes
 * from the current posiition to the new one.
 */
bool MINBASECLI::array_shift_from_pos(uint8_t* array, const size_t array_size,
        const size_t from_position, const t_shift_to shift_to)
{
    uint32_t num_shifts = 0U;
    uint8_t* ptr_array_from_position = NULL;

    // Do nothing if given pointer of the array is Null
    if (array == NULL)
        return false;

    // Do nothing if array is empty
    if (array_size == 0U)
        return false;

    // Do nothing if shift to type value is unexpected
    if ( (shift_to != SHIFT_LEFT) && (shift_to != SHIFT_RIGHT) )
        return false;

    // Do nothing if given position is higher than array size
    if (from_position >= (array_size - 1U))
        return false;

    // Shift the array
    ptr_array_from_position = array + from_position;
    num_shifts = array_size - from_position;
    if (shift_to == SHIFT_LEFT)
    {
        //  all the bytes from the current posiition+1 to position.
        memmove(ptr_array_from_position, (ptr_array_from_position + 1U),
                num_shifts);

        // Add a zero byte to previous last position
        array[array_size - 1U] = 0x00;
    }
    else if (shift_to == SHIFT_RIGHT)
    {
        //  all the bytes from the current posiition+1 to position.
        memmove((ptr_array_from_position + 1U), ptr_array_from_position,
                num_shifts);

        // Add a zero byte to previous last position
        *ptr_array_from_position = 0x00;
    }

    return true;
}

/**
 * @details
 * This function checks if provided string buffer has enough size to store the
 * maximum unsigned integer value on it, and conver each individual digit of
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
 * negative numbers, and conver each individual digit of the number to
 * characters that are store in the str array. The conversion is done from most
 * significant digit to less one to allow easily append at the end the minus
 * signal in case the number was negative, then the string is reversed to get
 * the correct string number in the array.
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

/*****************************************************************************/
