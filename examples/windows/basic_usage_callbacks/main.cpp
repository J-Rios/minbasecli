/**
 * @file    examples/windows/basic_usage_callbacks/main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    05-03-2023
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

/* Global Elements */

// Application exit flag
static volatile bool exit = false;

/*****************************************************************************/

/* Function Prototypes */

// CLI command "help" callback function
void cmd_help(MINBASECLI* Cli, int argc, char* argv[]);

// CLI command "test" callback function
void cmd_test(MINBASECLI* Cli, int argc, char* argv[]);

// CLI command "version" callback function
void cmd_version(MINBASECLI* Cli, int argc, char* argv[]);

// CLI command "exit" callback function
void cmd_exit(MINBASECLI* Cli, int argc, char* argv[]);

/*****************************************************************************/

/* main Function */

int main()
{
    MINBASECLI Cli;

    // Setup Command Line Interface
    Cli.setup();

    // Add commands and bind callbacks to them
    Cli.add_cmd("test", &cmd_test, "test [on/off] - Turn test mode ON/OFF.");
    Cli.add_cmd("version", &cmd_version, "Shows current application version.");
    Cli.add_cmd("exit", &cmd_exit, "Exit and close the program.");

    // The "help" command is already builtin and available from the CLI, and it
    // will shows added command descriptions, but you can setup a custom one
    Cli.add_cmd("help", &cmd_help, "Shows program help information.");

    Cli.printf("\nCommand Line Interface is ready\n\n");

    while (1)
    {
        // Exit loop if exit command received
        if (exit)
        {   break;   }

        // Check and Handle CLI commands
        Cli.run();

        // Some delay to free cpu usage
        delay_ms(10);
    }

    return 0;
}

/*****************************************************************************/

/* CLI Commands Callbacks */

void cmd_help(MINBASECLI* Cli, int argc, char* argv[])
{
    // Show some Info text
    Cli->printf("\nCustom Help Command\n");
    Cli->printf("MINBASECLI basic_usage_callbacks %s\n", APP_VER);

    // Call the builtin "help" function to show added command descriptions
    Cli->cmd_help(argc, argv);
}

void cmd_test(MINBASECLI* Cli, int argc, char* argv[])
{
    bool invalid_argv = false;

    if (argc == 0)
    {   invalid_argv = true;   }
    else
    {
        char* test_mode = argv[0];
        if (strcmp(test_mode, "on") == 0)
        {   Cli->printf("Turning Test Mode ON.\n");   }
        else if (strcmp(test_mode, "off") == 0)
        {   Cli->printf("Turning test Mode OFF.\n");   }
        else
        {   invalid_argv = true;   }
    }

    if (invalid_argv)
    {   Cli->printf("Test mode command needs \"on\" or \"off\" arg.\n");   }

    Cli->printf("\n");
}

void cmd_version(MINBASECLI* Cli, int argc, char* argv[])
{
    Cli->printf("App Version: %s\n\n", APP_VER);
}

void cmd_exit(MINBASECLI* Cli, int argc, char* argv[])
{
    Cli->printf("Exiting Application...\n\n");
    exit = true;
}
