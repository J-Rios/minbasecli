
/**
 * @file    main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    09-07-2022
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * Basic usage example of MinBaseCLI library for AVR devices main file that
 * shows the usage of the CLI to setup and handle commands callbacks.
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
#include <avr/pgmspace.h>

// Project Headers
#include "constants.h"
#include "minbasecli.h"
#include "avr_uart.h"

/*****************************************************************************/

/* Defines & Macros */

// Data Direction Register from Port Macro
// Address of DDRx is address of PORTx-1
#define DDR(x) (*(&x - 1))

// Current Firmware Application Version
#define FW_APP_VERSION "1.0.0"

/*****************************************************************************/

/* Function Prototypes */

// Initialize the LED (GPIO)
void led_init(void);

// Turn the LED on
void led_on(void);

// Turn the LED off
void led_off(void);

// CLI command "help" callback function
void cmd_help(MINBASECLI* Cli, int argc, char* argv[]);

// CLI command "led" callback function
void cmd_led(MINBASECLI* Cli, int argc, char* argv[]);

// CLI command "version" callback function
void cmd_version(MINBASECLI* Cli, int argc, char* argv[]);

/*****************************************************************************/

/* Main Function */

int main(void)
{
    static AvrUart Serial(UART0, F_CPU);
    static MINBASECLI Cli;

    // Set LED Pin as digital Output
    led_init();

    // Initilize UART0
    Serial.setup(SERIAL_BAUDS);

    // CLI init to use Serial as interface
    Cli.setup(&Serial);

    // Add commands and bind callbacks to them
    Cli.add_cmd("led", &cmd_led, PSTR("led [on/off], Turn LED ON or OFF.."));
    Cli.add_cmd("version", &cmd_version, PSTR("Shows current firmware version."));

    // The "help" command is already builtin and available from the CLI, and it
    // will shows added command descriptions, but you can setup a custom one
    Cli.add_cmd("help", &cmd_help, PSTR("Shows program help information."));

    Cli.printf(PSTR("\nCommand Line Interface is ready\n\n"));

    while (1)
    {
        // Check and Handle CLI commands
        Cli.run();
    }
}

/*****************************************************************************/

/* CLI Commands Callbacks */

void cmd_help(MINBASECLI* Cli, int argc, char* argv[])
{
    // Show some Info text
    Cli->printf(PSTR("\nCustom Help Command\n"));
    Cli->printf(PSTR("MINBASECLI basic_usage_callbacks %s\n"), FW_APP_VERSION);

    // Call the builtin "help" function to show added command descriptions
    Cli->cmd_help(argc, argv);
}

void cmd_led(MINBASECLI* Cli, int argc, char* argv[])
{
    bool invalid_argv = false;

    if(argc == 0)
        invalid_argv = true;
    else
    {
        char* test_mode = argv[0];
        if(strcmp(test_mode, "on") == 0)
        {
            Cli->printf(PSTR("Turning LED ON.\n"));
            led_on();
        }
        else if(strcmp(test_mode, "off") == 0)
        {
            Cli->printf(PSTR("Turning LED OFF.\n"));
            led_off();
        }
        else
            invalid_argv = true;
    }

    if(invalid_argv)
        Cli->printf(PSTR("led command needs \"on\" or \"off\" arg.\n"));

    Cli->printf(PSTR("\n"));
}

void cmd_version(MINBASECLI* Cli, int argc, char* argv[])
{
    Cli->printf(PSTR("FW App Version: %s\n"), FW_APP_VERSION);
}

/*****************************************************************************/

/* LED Functions */

// Set LED Pin as digital Output
void led_init(void)
{
    DDR(COMMAND_LED_PORT) |= (1 << COMMAND_LED_PIN);
}

void led_on(void)
{
    COMMAND_LED_PORT |= (1 << COMMAND_LED_PIN);
}

void led_off(void)
{
    COMMAND_LED_PORT &= ~(1 << COMMAND_LED_PIN);
}
