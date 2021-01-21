/**
 * @file    basic_usage.ino
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    17-01-2021
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * SIMPLECLI library basic usage example.
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

// Device Framework libraries
#include <Arduino.h>

// Custom libraries
#include <minbasecli.h>

/*****************************************************************************/

/* Defines, Macros, Constants and Types */

// Serial Speed
#define SERIAL_BAUDS 115200

// LED to be controlled thorugh CLI command
#define COMMAND_LED 13

// Current Firmware Version
#define FW_VER "1.0.0"

/*****************************************************************************/

/* Global Elements */

// Command Line Interface
MINBASECLI Cli;

/*****************************************************************************/

/* Setup & Loop */

void setup()
{
    // Serial Init
    Serial.begin(SERIAL_BAUDS);

    // LED Init
    digitalWrite(COMMAND_LED, LOW);
    pinMode(COMMAND_LED, OUTPUT);

    // CLI init to use Serial as interface
    Cli.setup(&Serial);
}

void loop()
{
    t_cli_result cli_read;

    // If any command was received
    if(Cli.manage(&cli_read))
    {
        // Show read result element
        Serial.print("Command received: "); Serial.println(cli_read.cmd);
        Serial.print("Number of arguments: "); Serial.println(cli_read.argc);
        for(uint8_t i = 0; i < cli_read.argc; i++)
        {
            Serial.print("    Argument "); Serial.print(i);
            Serial.print(":"); Serial.println(cli_read.argv[i]);
        }
        Serial.println();

        // Handle Commands
        if(strcmp(cli_read.cmd, "help") == 0)
        {
            Serial.println("Available Commands:");
            Serial.println("  help - Current info.");
            Serial.println("  led [on/off] - Turn LED ON or OFF");
            Serial.println("  version - Shows current firmware version");
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
                    Serial.println("Turning LED ON.");
                    digitalWrite(COMMAND_LED, HIGH);
                }
                else if(strcmp(led_mode, "off") == 0)
                {
                    Serial.println("Turning LED OFF.");
                    digitalWrite(COMMAND_LED, LOW);
                }
                else
                    invalid_argv = true;
            }
            
            if(invalid_argv)
                Serial.println("led command needs \"on\" or \"off\" arg.");
        }
        else if(strcmp(cli_read.cmd, "version") == 0)
        {
            Serial.print("Fw Version: ");
            Serial.println(FW_VER);
        }
        // ...
        else
            Serial.println("Unkown command.");
        Serial.println();
    }
}
