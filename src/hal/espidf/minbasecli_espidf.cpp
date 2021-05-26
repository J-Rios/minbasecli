
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

#ifdef ESP_PLATFORM

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_espidf.h"

// Device/Framework Libraries
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/uart.h>
#include <esp_vfs_dev.h>

// Standard Libraries
#include <stdio.h>
#include <string.h>

/*****************************************************************************/

/* Logging Tag */

static const char TAG[] = "MinBaseCLI";

/*****************************************************************************/

/* Read STDIN Strem Thread Prototype */

void th_read_stdin(void* arg);

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
    this->th_rx_read_head = 0;
    this->th_rx_read_tail = 0;
    this->rx_read[0] = '\0';
    this->th_rx_read[0] = '\0';

    // Initialize millis count
    hal_millis();
}

/*****************************************************************************/

/* Public Methods */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  */
bool MINBASECLI::setup()
{
    if (hal_uart_setup() == false)
        return false;
    if (launch_stdin_read_thread() == false)
        return false;

    this->initialized = true;
    return true;
}

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI::setup(void* iface)
{
    if (setup() == false)
        return false;

    this->iface = iface;
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
  * @brief  Launch the Thread to read from STDIN Stream.
  * @return If thread has been created (true/false).
  */
bool MINBASECLI::launch_stdin_read_thread()
{
    if (xTaskCreate(&th_read_stdin, "th_read_stdin", SIMPLECLI_TASK_STACK,
                   (void*)(this), tskIDLE_PRIORITY+5, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Fail to create STDIN read thread");
        return false;
    }

    return true;
}

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
    // While there is any data incoming from the CLI interface
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
                    rx_read[this->received_bytes-1] = '\0';
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
    return (uint32_t)(esp_timer_get_time() / 1000);
}

/**
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI::hal_uart_setup()
{
    esp_err_t rc = ESP_OK;

    // Flush STDOUT
    fflush(stdout);
    fsync(fileno(stdout));

    // Disable buffering on stdin
    setvbuf(stdin, NULL, _IONBF, 0);

    // Configure UART. Note that
    const uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 127,
        #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
            // REF_TICK used to keep correct baud rate while APB frequency
            // is changing in light sleep mode
            .source_clk = UART_SCLK_REF_TICK,
        #else
            .source_clk = UART_SCLK_XTAL,
        #endif
    };

    // Install UART driver for interrupt-driven reads and writes
    rc = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (rc != ESP_OK)
    {
        ESP_ERROR_CHECK(rc);
        ESP_LOGE(TAG, "Fail to install UART driver");
        return false;
    }
    rc = uart_param_config(UART_NUM_0, &uart_config);
    if (rc != ESP_OK)
    {
        ESP_ERROR_CHECK(rc);
        ESP_LOGE(TAG, "Fail to setup UART parameters");
        return false;
    }

    // Set Virtual FileSystem Lines Endings
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    // Set Virtual FileSystem to use UART0 driver
    esp_vfs_dev_uart_use_driver(UART_NUM_0);

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

    printf("%s", str);
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

    printf("%s\n", str);
}

size_t MINBASECLI::hal_iface_available()
{
    // Do nothing if interface has not been initialized
    if (iface_is_not_initialized())
        return 0;

    return (this->th_rx_read_head - this->th_rx_read_tail);
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

    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    this->th_rx_read_tail = (this->th_rx_read_tail + 1) %
            SIMPLECLI_MAX_READ_SIZE;
    return th_rx_read[this->th_rx_read_tail];
}

/*****************************************************************************/

/* STDIN Read Thread */

/**
  * @brief  Thread to read from STDIN Stream.
  */
void th_read_stdin(void* arg)
{
    MINBASECLI* _this = (MINBASECLI*) arg;
    int ch = EOF;

    while (true)
    {
        ch = getc(stdin);
        if (ch != EOF)
        {
            _this->th_rx_read_head = (_this->th_rx_read_head + 1) % SIMPLECLI_MAX_READ_SIZE;
            _this->th_rx_read[_this->th_rx_read_head] = ch;
        }
        else
            vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*****************************************************************************/

#endif /* ESP_PLATFORM */
