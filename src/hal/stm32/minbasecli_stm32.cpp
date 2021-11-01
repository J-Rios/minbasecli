
/**
 * @file    minbasecli_stm32wl.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    22-10-2021
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

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F2) \
|| defined(STM32G0) || defined(STM32G4) || defined(STM32H7) \
|| defined(STM32F3) || defined(STM32F4) || defined(STM32F7) \
|| defined(STM32L0) || defined(STM32L1) || defined(STM32L4) \
|| defined(STM32L5) || defined(STM32MP1) || defined(STM32U5) \
|| defined(STM32WB) || defined(STM32WL)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_stm32wl.h"

// Device/Framework Libraries
#include "stm32wlxx_hal.h"

// Standard Libraries
#include <stdio.h>
#include <string.h>

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#define _IFACE UART_HandleTypeDef

// Default USART
#define DEFAULT_UART USART2

// Default Baudrate
#define DEFAULT_BAUDS 19200

/*****************************************************************************/

/* In-Scope Static Private */

// Specific UART element
static UART_HandleTypeDef uart2;

// IT callback received byte
static uint8_t rx_byte[1];

// Read index that point to read buffer data FIFO element
static uint8_t rx_read_head = 0;
static uint8_t rx_read_tail = 0;

// Received bytes buffer
static uint8_t rx_buffer[SIMPLECLI_MAX_READ_SIZE];

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
    this->iface = &uart2;
    if (!hal_uart_setup(true))
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
    this->iface = iface;
    if (!hal_uart_setup(false))
        return false;
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
    return ((uint32_t)(HAL_GetTick()));
}

/**
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI::hal_uart_setup(bool self_initialization)
{
    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    if (self_initialization)
    {
        // Setup UART Properties
        _Serial->Instance = DEFAULT_UART;
        _Serial->Init.BaudRate = DEFAULT_BAUDS;
        _Serial->Init.WordLength = UART_WORDLENGTH_8B;
        _Serial->Init.StopBits = UART_STOPBITS_1;
        _Serial->Init.Parity = UART_PARITY_ODD;
        _Serial->Init.Mode = UART_MODE_TX_RX;
        _Serial->Init.HwFlowCtl = UART_HWCONTROL_NONE;
        _Serial->Init.OverSampling = UART_OVERSAMPLING_16;
        _Serial->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        _Serial->Init.ClockPrescaler = UART_PRESCALER_DIV1;
        _Serial->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

        // Initialize Peripheral
        if (HAL_UART_Init(_Serial) != HAL_OK)
            return false;
        if (HAL_UARTEx_SetTxFifoThreshold(_Serial, UART_TXFIFO_THRESHOLD_1_8) \
        != HAL_OK)
            return false;
        if (HAL_UARTEx_SetRxFifoThreshold(_Serial, UART_RXFIFO_THRESHOLD_1_8) \
        != HAL_OK)
            return false;
        if (HAL_UARTEx_DisableFifoMode(_Serial) != HAL_OK)
            return false;
    }

    // Start Async Reception for 1 bytes chunks and store it in `rx_buffer`
    // The reception callback will fire for each byte reception
    if(HAL_UART_Receive_IT(_Serial, (uint8_t*)rx_byte, 1) != HAL_OK)
        return false;

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

    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    // Check if peripheral is ready to send data
    HAL_UART_StateTypeDef state = HAL_UART_GetState(_Serial);
    while ((state & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX);
    // No-Blocking instead above while:
    //if ((state & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX)
    //    return;

    // Transmit
    if (HAL_UART_Transmit_IT(_Serial, (uint8_t*)str, strlen(str)) != HAL_OK)
        return;

    return;
}

/**
  * @brief  Print line a given string through the CLI HAL interface.
  * @param  str String to print.
  */
void MINBASECLI::hal_iface_println(const char* str)
{
    //if (!hal_iface_print(str))
    //    return;
    hal_iface_print(str);
    return hal_iface_print("\n");
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI::hal_iface_available()
{
    return (rx_read_head - rx_read_tail);
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
    rx_read_tail = (rx_read_tail + 1) % SIMPLECLI_MAX_READ_SIZE;
    return rx_buffer[rx_read_tail];
}

/*****************************************************************************/

/* HAL UART Operation Callbacks */

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   UART Rx transfer completed callback (something was received).
  * @retval None
  */
static void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle)
{
    // Increase number of received bytes
    rx_read_head = (rx_read_head + 1) % SIMPLECLI_MAX_READ_SIZE;
    rx_buffer[rx_read_head] = rx_byte[0];

    // Reload Async Reception
    if(HAL_UART_Receive_IT(_Serial, (uint8_t*)rx_byte, 1) != HAL_OK)
        return 0;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   UART Tx transfer completed callback (something was sent).
  * @retval None
  */
static void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   UART transfer operation error callback.
  * @retval None
  */
static void HAL_UART_ErrorCallback(UART_HandleTypeDef* UartHandle)
{}

/*****************************************************************************/

#endif /* STM32 */
