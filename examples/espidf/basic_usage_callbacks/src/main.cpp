/**
 * @file    examples/espidf/basic_usage_callbacks/main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    09-07-2022
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * MINBASECLI library basic usage example for ESP-IDF Framework that shows the
 * usage of the CLI to setup and handle commands callbacks.
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

// Current Firmware Application Version
#define FW_APP_VERSION "1.0.0"

// Command Line Interface Interpreter Thread Stack Size
#define CLI_TASK_STACK 4096

// ESP Logging Main Tag
static const char TAG[] = "Main";

/*****************************************************************************/

/* Global Elements */

static MINBASECLI Cli;

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

// CLI command callback functions
void cmd_help(int argc, char* argv[]);
void cmd_heap(int argc, char* argv[]);
void cmd_led(int argc, char* argv[]);
void cmd_mac(int argc, char* argv[]);
void cmd_reboot(int argc, char* argv[]);
void cmd_version(int argc, char* argv[]);

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
    // Setup Command Line Interface
    Cli.setup();

    // Add commands and bind callbacks to them
    Cli.add_cmd("heap", &cmd_heap, "Show available HEAP memory.");
    Cli.add_cmd("led", &cmd_led, "led [on/off], turn LED ON or OFF.");
    Cli.add_cmd("mac", &cmd_mac, "Show ESP Base MAC Address.");
    Cli.add_cmd("reboot", &cmd_reboot, "Reboot the system.");
    Cli.add_cmd("version", &cmd_version, "Shows current frimware version.");

    // The "help" command is already builtin and available from the CLI, and it
    // will shows added command descriptions, but you can setup a custom one
    Cli.add_cmd("help", &cmd_help, "Shows program help information.");

    Cli.printf("\nCommand Line Interface is ready\n\n");

    while(1)
    {
        // Check and Handle CLI commands
        Cli.run();

        // Some delay to free cpu usage
        delay_ms(10);
    }
}

/*****************************************************************************/

/* CLI Commands Callbacks */

void cmd_help(int argc, char* argv[])
{
    // Show some Info text
    Cli.printf("\nCustom Help Command\n");
    Cli.printf("MINBASECLI basic_usage_callbacks %s\n", FW_APP_VERSION);

    // Call the builtin "help" function to show added command descriptions
    Cli.cmd_help(argc, argv);
}

void cmd_heap(int argc, char* argv[])
{
    uint32_t heap_available = esp_get_free_heap_size();
    uint32_t heap_in_available = esp_get_free_internal_heap_size();
    uint32_t heap_min_available = esp_get_minimum_free_heap_size();
    Cli.printf("Available heap: %" PRIu32 " bytes\n", heap_available);
    Cli.printf("Available internal heap: %" PRIu32 " bytes\n",
            heap_in_available);
    Cli.printf("Minimum heap that has ever been available: " \
            "%" PRIu32 " bytes\n", heap_min_available);
}

void cmd_led(int argc, char* argv[])
{
    bool invalid_argv = false;

    // Check for argument
    if(argc == 0)
        invalid_argv = true;
    else
    {
        char* test_mode = argv[0];
        if(strcmp(test_mode, "on") == 0)
        {
            Cli.printf("LED (pin %" PRIu8 "), ON\n", IO_LED);
            gpio_set_level(IO_LED, 1);
        }
        else if(strcmp(test_mode, "off") == 0)
        {
            Cli.printf("LED (pin %" PRIu8 "), OFF\n", IO_LED);
            gpio_set_level(IO_LED, 0);
        }
        else
            invalid_argv = true;
    }

    if(invalid_argv)
        Cli.printf("LED command needs \"on\" or \"off\" arg.");
}

void cmd_mac(int argc, char* argv[])
{
    uint8_t mac_addr[6] = { 0 };
    ESP_ERROR_CHECK(esp_read_mac(mac_addr, ESP_MAC_WIFI_STA));
    Cli.printf("WiFi Station MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]);
    Cli.printf("WiFi SoftAP MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]+1);
    Cli.printf("Bluetooth MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]+2);
    Cli.printf("Ethernet MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]+3);
}

void cmd_reboot(int argc, char* argv[])
{
    Cli.printf("Rebooting...\n");
    Cli.printf("\n--------------------------------\n\n");
    esp_restart();
}

void cmd_version(int argc, char* argv[])
{
    Cli.printf("ESP-IDF Version: %s\n", esp_get_idf_version());
    Cli.printf("FW App Version: %s\n", FW_APP_VERSION);
}
