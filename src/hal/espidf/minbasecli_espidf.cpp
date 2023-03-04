
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

#if defined(ESP_PLATFORM) && !defined(ARDUINO)

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
static const uint16_t MINBASECLI_TASK_STACK = 2048;

// Logging Tag
static const char TAG[] = "MinBaseCLI";

/*****************************************************************************/

/* Read STDIN Stream Thread Prototype */

/**
 * @brief FreeRTOS Task handler function to read STDIN data from the
 * interface.
 * @param arg FreeRTOS Task arguments.
 */
void th_read_stdin(void* arg);

/*****************************************************************************/

/* Constructor */

/**
 * @details
 * This constructor initializes all attributes of the CLI class.
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
 * @details
 * This function should get and initialize the interface element that is going
 * to be used by the CLI and it also start the STDIN data read thread.
 */
bool MINBASECLI_ESPIDF::hal_setup(void* iface, const uint32_t baud_rate)
{
    if (iface == NULL)
        { return false; }

    this->iface = iface;
    if (uart_setup(baud_rate) == false)
        { return false; }
    if (launch_stdin_read_thread() == false)
        { return false; }

    return true;
}

/**
 * @details
 * This function return the number of bytes received by the interface that are
 * available to be read.
 */
size_t MINBASECLI_ESPIDF::hal_iface_available()
{
    return (this->th_rx_read_head - this->th_rx_read_tail);
}

/**
 * @details
 * This function returns a received byte from the interface. It checks if
 * there is any byte available to be read and increase the read circular
 * buffer tail index to "pop" this element from the buffer and return it.
 */
uint8_t MINBASECLI_ESPIDF::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    this->th_rx_read_tail = (this->th_rx_read_tail + 1) %
            MINBASECLI_MAX_READ_SIZE;
    return th_rx_read[this->th_rx_read_tail];
}

/**
 * @details
 * This function send a byte through the interface.
 */
void MINBASECLI_ESPIDF::hal_iface_print(const uint8_t data_byte)
{
    printf("%c", (char)(data_byte));
}

/*****************************************************************************/

/* Private Methods */

/**
 * @details
 * This function create a FreeRTOS Task to handle the STDIN data read from the
 * interface.
 */
bool MINBASECLI_ESPIDF::launch_stdin_read_thread()
{
    if (xTaskCreate(&th_read_stdin, "th_read_stdin", MINBASECLI_TASK_STACK,
                   (void*)(this), tskIDLE_PRIORITY+5, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Fail to create STDIN read thread");
        return false;
    }

    return true;
}

/**
 * @details
 * This function configure and initialize the UART for the given baud rate
 * communication speed. It fush the STDOUT, disable the buffering of the
 * STDIN, configure the Serial communication parameters, setup the ESP-IDF
 * UART driver to manage the UART peripheral, apply the configuration, set the
 * reception and transmission line endings and bind a virtual file system to
 * the UART.
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
 * @details
 * This function is the FreeRTOS Task that manages the STDIN data read.
 * It gets each new byte received from the interface and store them in the
 * read buffer (increasing the circular buffer head index).
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
            _this->th_rx_read_head = \
                    (_this->th_rx_read_head + 1) % MINBASECLI_MAX_READ_SIZE;
            _this->th_rx_read[_this->th_rx_read_head] = ch;
        }
        else
            vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*****************************************************************************/

#endif /* defined(ESP_PLATFORM) && !defined(ARDUINO) */
