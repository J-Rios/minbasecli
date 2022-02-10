
/**
 * @file    minbasecli_espidf.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    10-02-2022
 * @version 1.0.2
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

#if defined(ESP_PLATFORM)

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

/* Constants & Defines */

// UART Read Task Stack Size
static const uint16_t SIMPLECLI_TASK_STACK = 2048;

// Logging Tag
static const char TAG[] = "MinBaseCLI";

/*****************************************************************************/

/* Read STDIN Stream Thread Prototype */

void th_read_stdin(void* arg);

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_ESPIDF::MINBASECLI_ESPIDF()
{
    this->iface = NULL;
    this->th_rx_read_head = 0;
    this->th_rx_read_tail = 0;
    this->th_rx_read[0] = '\0';
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI_ESPIDF::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    if (uart_setup(baud_rate) == false)
        return false;
    if (launch_stdin_read_thread() == false)
        return false;
    return true;
}

size_t MINBASECLI_ESPIDF::hal_iface_available()
{
    return (this->th_rx_read_head - this->th_rx_read_tail);
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_ESPIDF::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    this->th_rx_read_tail = (this->th_rx_read_tail + 1) %
            SIMPLECLI_MAX_READ_SIZE;
    return th_rx_read[this->th_rx_read_tail];
}

/**
  * @brief  Print a byte with ASCII encode to CLI HAL interface.
  * @param  data_byte Byte of data to write.
  */
void MINBASECLI_ESPIDF::hal_iface_print(const uint8_t data_byte)
{
    printf("%c", (char)(data_byte));
}

/*****************************************************************************/

/* Private Methods */

/**
  * @brief  Launch the Thread to read from STDIN Stream.
  * @return If thread has been created (true/false).
  */
bool MINBASECLI_ESPIDF::launch_stdin_read_thread()
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
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI_ESPIDF::uart_setup(const uint32_t baud_rate)
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
        .baud_rate = (int)(baud_rate),
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
    esp_vfs_dev_uart_port_set_rx_line_endings(UART_NUM_0, ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_port_set_tx_line_endings(UART_NUM_0,
            ESP_LINE_ENDINGS_CRLF);

    // Set Virtual FileSystem to use UART0 driver
    esp_vfs_dev_uart_use_driver(UART_NUM_0);

    return true;
}

/*****************************************************************************/

/* STDIN Read Thread */

/**
  * @brief  Thread to read from STDIN Stream.
  */
void th_read_stdin(void* arg)
{
    MINBASECLI_ESPIDF* _this = (MINBASECLI_ESPIDF*) arg;
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

#endif /* defined(ESP_PLATFORM) */
