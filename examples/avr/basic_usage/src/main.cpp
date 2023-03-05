
/**
 * @file    main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    05-03-2023
 * @version 1.0.2
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

/* Global Elements */

// UART Object
AvrUart Serial(UART0, F_CPU);

/*****************************************************************************/

/* Function Prototypes */

void led_init(void);
void led_on(void);
void led_off(void);

/*****************************************************************************/

/* Main Function */

int main(void)
{
    // Command Line Interface
    MINBASECLI Cli;

    // Set LED Pin as digital Output
    led_init();

    // Initilize UART0
    Serial.setup(SERIAL_BAUDS);

    // CLI init to use Serial as interface
    Cli.setup(&Serial);
    Cli.printf("\nCommand Line Interface is ready\n\n");

    while (1)
    {
        t_cli_result cli_read;

        // If any command was received
        if (Cli.manage(&cli_read))
        {
            // Show read result element
            Cli.printf("Command received: %s\n", cli_read.cmd);
            Cli.printf("Number of arguments: %d\n"), (int)(cli_read.argc);
            for (int i = 0; i < cli_read.argc; i++)
            {   Cli.printf("  Argument %d: %s", i, cli_read.argv[i]);   }
            Cli.printf("\n");

            // Handle Commands
            if (strcmp(cli_read.cmd, "help") == 0)
            {
                Cli.printf("Available Commands:\n");
                Cli.printf("  help - Current info.\n");
                Cli.printf("  led [on/off] - Turn LED ON or OFF.\n");
                Cli.printf("  version - Shows current firmware version.\n");
            }
            else if (strcmp(cli_read.cmd, "led") == 0)
            {
                bool invalid_argv = false;
                char* led_mode = NULL;

                // Check for argument
                if (cli_read.argc == 0)
                {   invalid_argv = true;   }
                else
                {
                    led_mode = cli_read.argv[0];
                    if (strcmp(led_mode, "on") == 0)
                    {
                        Cli.printf("Turning LED ON.\n");
                        led_on();
                    }
                    else if (strcmp(led_mode, "off") == 0)
                    {
                        Cli.printf("Turning LED OFF.\n");
                        led_off();
                    }
                    else
                    {   invalid_argv = true;   }
                }

                if (invalid_argv)
                {   Cli.printf("led command needs 'on' or 'off' arg.\n");   }
            }
            else if (strcmp(cli_read.cmd, "version") == 0)
            {
                Cli.printf("FW App Version: %s\n", FW_APP_VERSION);
            }
            // ...
            else
            {   Cli.printf("Unkown command.\n");   }
            Cli.printf("\n");
        }
    }
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
