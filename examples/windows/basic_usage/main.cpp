/**
 * @file    examples/windows/basic_usage/main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    02-04-2022
 * @version 1.0.1
 *
 * @section DESCRIPTION
 *
 * MINBASECLI library basic usage example for Windows OS system.
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

// Device/System Framework
#include <windows.h>

// Custom Libraries
#include <minbasecli.h>

/*****************************************************************************/

/* Defines, Macros, Constants and Types */

// Delay milli-seconds Macro
#define delay_ms(x) do { Sleep(x); } while(0)

// Current Application Version
#define APP_VER "1.0.0"

/*****************************************************************************/

/* main Function */

int main()
{
    MINBASECLI Cli;
    t_cli_result cli_read;
    bool command_received = false;
    bool exit = false;

    // Setup Command Line Interface
    Cli.setup(MINBASECLI_DEFAULT_IFACE, MINBASECLI_DEFAULT_BAUDS);
    Cli.printf("\nCommand Line Interface is ready\n\n");

    while(1)
    {
        // Exit loop if exit command received
        if (exit)
            break;

        // Check and Handle CLI commands
        command_received = Cli.manage(&cli_read);
        if(command_received)
        {
            // Show read result element
            Cli.printf("Command received: %s\n", cli_read.cmd);
            Cli.printf("Number of arguments: %d\n", (int)(cli_read.argc));
            for(int i = 0; i < cli_read.argc; i++)
                Cli.printf("    Argument %d: %s", i, cli_read.argv[i]);
            Cli.printf("\n");

            // Handle Commands
            if(strcmp(cli_read.cmd, "help") == 0)
            {
                Cli.printf("Available Commands:\n");
                Cli.printf("  help - Current info.\n");
                Cli.printf("  test [on/off] - Turn test mode ON or OFF\n");
                Cli.printf("  version - Shows current firmware version\n");
                Cli.printf("  exit - Exit and close the program\n");
            }
            else if(strcmp(cli_read.cmd, "test") == 0)
            {
                bool invalid_argv = false;

                // Check for argument
                if(cli_read.argc == 0)
                    invalid_argv = true;
                else
                {
                    char* test_mode = cli_read.argv[0];
                    if(strcmp(test_mode, "on") == 0)
                        Cli.printf("Turning Test Mode ON.\n");
                    else if(strcmp(test_mode, "off") == 0)
                        Cli.printf("Turning test Mode OFF.\n");
                    else
                        invalid_argv = true;
                }

                if(invalid_argv)
                    Cli.printf("Test mode command needs \"on\" or \"off\" arg.");
            }
            else if(strcmp(cli_read.cmd, "version") == 0)
            {
                Cli.printf("App Version: %s\n", APP_VER);
            }
            else if(strcmp(cli_read.cmd, "exit") == 0)
            {
                Cli.printf("Exiting Application...\n");
                exit = true;
            }
            // ...
            else
                Cli.printf("Unkown command.\n");
            Cli.printf("\n");
        }

        // Some delay to free cpu usage
        delay_ms(10);
    }

    return 0;
}
