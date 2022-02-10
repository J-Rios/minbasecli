/**
 * @file    examples/espidf/basic_usage/main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    03-06-2021
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * MINBASECLI library basic usage example for ESP-IDF Framework.
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

// Standard Libraries
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

// Device Framework
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/gpio.h>

// Custom Libraries
#include <minbasecli.h>

/*****************************************************************************/

/* Defines, Macros and Constants */

// GPIO Pin with LED connected
#define IO_LED GPIO_NUM_13

// Delay milli-seconds Macro
#define delay_ms(x) do { vTaskDelay(x/portTICK_PERIOD_MS); } while(0)

// Current Firmware Version
#define FW_VER "1.0.0"

// Command Line Interface Interpreter Thread Stack Size
#define CLI_TASK_STACK 4096

// ESP Logging Main Tag
static const char TAG[] = "Main";

/*****************************************************************************/

/* Functions Prototypes */

// Main Function
extern "C" { void app_main(void); }

// Secondary Functions
bool init_hardware(void);
bool launch_threads(void);

// CLI Thread
void th_cli_interpreter(void* arg);
void cli_interpreter(t_cli_result* cli_read);

/*****************************************************************************/

/* main Function */

void app_main(void)
{
    // Setup and initialize device hardware
    init_hardware();

    // Launch threads
    launch_threads();

    // Main Thread Loop
    while(1)
    {
        /* Nothing to do here */

        // Some delay to free cpu usage
        delay_ms(10);
    }
}

/*****************************************************************************/

/* Secondary Functions */

/**
  * @brief  Setup and initialize device hardware.
  * @return If all hardware has been successfully initialized (true/false).
  */
bool init_hardware(void)
{
    // Set LED pin as digital output
    gpio_pad_select_gpio(IO_LED);
    gpio_set_direction(IO_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(IO_LED, 0);

    return true;
}

/**
  * @brief  Launch system Tasks Threads.
  * @return If all thread has been successfully created (true/false).
  */
bool launch_threads(void)
{
    bool any_thread_fail = false;

    // Create Command Line Interface Thread
    if (xTaskCreate(&th_cli_interpreter, "th_cli_interpreter", CLI_TASK_STACK,
            NULL, tskIDLE_PRIORITY+5, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Fail to create STDIN read thread");
        any_thread_fail = true;
    }

    /* Create other Tasks threads here */
    // ...

    return (!any_thread_fail);
}

/*****************************************************************************/

/* CLI Task */

/**
  * @brief  Thread for Command Line Interface Interpreter.
  * @param  arg Thread input arguments.
  */
void th_cli_interpreter(void* arg)
{
    MINBASECLI Cli;
    t_cli_result cli_read;
    bool command_received = false;

    // Setup Command Line Interface
    Cli.setup(MINBASECLI_DEFAULT_IFACE, MINBASECLI_DEFAULT_BAUDS);
    ESP_LOGI(TAG, "Command Line Interface is ready");
    printf("\n\n");

    while(1)
    {
        // Check and Handle CLI commands
        command_received = Cli.manage(&cli_read);
        if(command_received)
            cli_interpreter(&cli_read);

        // Some delay to free cpu usage
        delay_ms(10);
    }
}

/**
  * @brief  Command Line Interface Interpreter.
  * @param  cli_read CLI read result structure (t_cli_result).
  */
void cli_interpreter(t_cli_result* cli_read)
{
    // Show read result element
    printf("Command received: %s\n", cli_read->cmd);
    printf("Number of arguments: %d\n", (int)(cli_read->argc));
    for(int i = 0; i < cli_read->argc; i++)
        printf("    Argument %d: %s\n", i, cli_read->argv[i]);
    printf("\n");

    // Handle Commands
    if(strcmp(cli_read->cmd, "help") == 0)
    {
        printf("Available Commands:\n");
        printf("  heap - Show available HEAP memory\n");
        printf("  help - Current info\n");
        printf("  led [on/off] - Turn LED ON or OFF\n");
        printf("  mac - Show ESP Base MAC Address\n");
        printf("  reboot - Reboot the system\n");
        printf("  version - Shows current firmware version\n");
    }
    else if(strcmp(cli_read->cmd, "heap") == 0)
    {
        uint32_t heap_available = esp_get_free_heap_size();
        uint32_t heap_in_available = esp_get_free_internal_heap_size();
        uint32_t heap_min_available = esp_get_minimum_free_heap_size();
        printf("Available heap: %" PRIu32 " bytes\n",
                heap_available);
        printf("Available internal heap: %" PRIu32 " bytes\n",
                heap_in_available);
        printf("Minimum heap that has ever been available: " \
                "%" PRIu32 " bytes\n", heap_min_available);
    }
    else if(strcmp(cli_read->cmd, "led") == 0)
    {
        bool invalid_argv = false;

        // Check for argument
        if(cli_read->argc == 0)
            invalid_argv = true;
        else
        {
            char* test_mode = cli_read->argv[0];
            if(strcmp(test_mode, "on") == 0)
            {
                printf("LED (pin %" PRIu8 "), ON\n", IO_LED);
                gpio_set_level(IO_LED, 1);
            }
            else if(strcmp(test_mode, "off") == 0)
            {
                printf("LED (pin %" PRIu8 "), OFF\n", IO_LED);
                gpio_set_level(IO_LED, 0);
            }
            else
                invalid_argv = true;
        }

        if(invalid_argv)
            printf("LED command needs \"on\" or \"off\" arg.");
    }
    else if(strcmp(cli_read->cmd, "mac") == 0)
    {
        uint8_t mac_addr[6] = { 0 };
        ESP_ERROR_CHECK(esp_read_mac(mac_addr, ESP_MAC_WIFI_STA));
        printf("WiFi Station MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5]);
        printf("WiFi SoftAP MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5]+1);
        printf("Bluetooth MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5]+2);
        printf("Ethernet MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5]+3);
    }
    else if(strcmp(cli_read->cmd, "reboot") == 0)
    {
        printf("Rebooting...\n");
        printf("\n--------------------------------\n\n");
        esp_restart();
    }
    else if(strcmp(cli_read->cmd, "version") == 0)
    {
        printf("ESP-IDF Version: %s\n", esp_get_idf_version());
        printf("Firmware Version: %s\n", FW_VER);
    }
    // ...
    else
        printf("Unkown command.\n");
    printf("\n");
}

/*****************************************************************************/
