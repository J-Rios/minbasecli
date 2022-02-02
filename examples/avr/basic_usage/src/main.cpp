
/**
 * @file    main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    02-02-2022
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * Basic usage example of MinBaseCLI library for AVR devices main file.
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

// Standard Headers (libc)
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

// Device/Framework Headers
#include <avr/io.h>

// Project Headers
#include "constants.h"
#include "minbasecli.h"
#include "avr_uart.h"

/*****************************************************************************/

/* Defines & Macros */

// Data Direction Register from Port Macro
// Address of DDRx is address of PORTx-1
#define DDR(x) (*(&x - 1))

/*****************************************************************************/

/* In-Scope Function Prototypes */

static void led_init(void);
static void led_on(void);
static void led_off(void);

static void serial_print(uint8_t data_byte);
static void serial_println(uint8_t data_byte);
static void serial_println(void);
static void serial_print(const char* str);
static void serial_println(const char* str);

/*****************************************************************************/

/* Global Elements */

AvrUart Serial(UART0, F_CPU);

/*****************************************************************************/

/* Main Function */

int main(void)
{
    // Command Line Interface
    MINBASECLI Cli;

    // Initilize UART0
    Serial.setup(SERIAL_BAUDS);

    // CLI init to use Serial as interface
    Cli.setup(&Serial);

    // Set LED Pin as digital Output
    led_init();

    while (1)
    {
        t_cli_result cli_read;

        // If any command was received
        if(Cli.manage(&cli_read))
        {
            // Show read result element
            serial_print("Command received: "); serial_println(cli_read.cmd);
            serial_print("Number of arguments: "); serial_println(cli_read.argc);
            for(uint8_t i = 0; i < cli_read.argc; i++)
            {
                serial_print("    Argument "); serial_print(i);
                serial_print(":"); serial_println(cli_read.argv[i]);
            }
            serial_println();

            // Handle Commands
            if(strcmp(cli_read.cmd, "help") == 0)
            {
                serial_println("Available Commands:");
                serial_println("  help - Current info.");
                serial_println("  led [on/off] - Turn LED ON or OFF");
                serial_println("  version - Shows current firmware version");
            }
            else if(strcmp(cli_read.cmd, "led") == 0)
            {
                bool invalid_argv = false;
                char* led_mode = NULL;

                // Check for argument
                if(cli_read.argc == 0)
                    invalid_argv = true;
                else
                {
                    led_mode = cli_read.argv[0];
                    if(strcmp(led_mode, "on") == 0)
                    {
                        serial_println("Turning LED ON.");
                        led_on();
                    }
                    else if(strcmp(led_mode, "off") == 0)
                    {
                        serial_println("Turning LED OFF.");
                        led_off();
                    }
                    else
                        invalid_argv = true;
                }

                if(invalid_argv)
                    serial_println("led command needs \"on\" or \"off\" arg.");
            }
            else if(strcmp(cli_read.cmd, "version") == 0)
            {
                serial_print("Fw Version: ");
                serial_println(FW_APP_VERSION);
            }
            // ...
            else
                serial_println("Unkown command.");
            serial_println();
        }
    }
}

/*****************************************************************************/

/* LED Functions */

// Set LED Pin as digital Output
static void led_init(void)
{
    DDR(COMMAND_LED_PORT) |= (1 << COMMAND_LED_PIN);
}

static void led_on(void)
{
    COMMAND_LED_PORT |= (1 << COMMAND_LED_PIN);
}

static void led_off(void)
{
    COMMAND_LED_PORT &= ~(1 << COMMAND_LED_PIN);
}

/*****************************************************************************/

/* Auxiliar Serial Functions */

static void serial_println(void)
{
    Serial.write((uint8_t)('\n'));
}

static void serial_print(const char* str)
{
    while (*str != '\0')
    {
        Serial.write(*str);
        str = str + 1;
    }
}

static void serial_println(const char* str)
{
    serial_print(str);
    Serial.write((uint8_t)('\n'));
}

static void serial_print(uint8_t data_byte)
{
    char str[4];
    char* ptr_str = str;

    snprintf(str, 4, "%u", data_byte);
    while (*ptr_str != '\0')
    {
        Serial.write(*ptr_str);
        ptr_str = ptr_str + 1;
    }
}

static void serial_println(uint8_t data_byte)
{
    serial_print(data_byte);
    serial_println();
}
