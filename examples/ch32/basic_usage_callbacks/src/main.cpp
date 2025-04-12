/**
 * @file    examples/ch32/basic_usage_callbacks/src/main.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    13-04-2025
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * MINBASECLI library basic usage example for CH32 devices that shows the
 * usage of the CLI to setup and handle commands callbacks.
 *
 * @section LICENSE
 *
 * Copyright (c) 2025 Jose Miguel Rios Rubio. All right reserved.
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

// CH32 HAL Library
#include "ch32v00x.h"

// Standard C++ Libraries
#include <cstring>

// Third-Party Libraries
#include "minbasecli.h"

/*****************************************************************************/

/* Defines, Macros and Constants */

// CLI UART Speed
static constexpr uint32_t UART_BAUD_RATE = 115200U;

// LED Pin
#define LED_PORT GPIOD
#define LED_PIN  GPIO_Pin_4

// Current Firmware Application Version
#define FW_APP_VERSION "1.0.0"

/*****************************************************************************/

/* Function Prototypes */

// CLI command callback functions
void cli_init(MINBASECLI* Cli);
void cmd_help(MINBASECLI* Cli, int argc, char* argv[]);
void cmd_led(MINBASECLI* Cli, int argc, char* argv[]);
void cmd_version(MINBASECLI* Cli, int argc, char* argv[]);

// LED functions
void led_init();
void led_on();
void led_off();

// Delay functions
void delay_init();
void delay_ms(const uint32_t n);

/*****************************************************************************/

/* Main Function */

int main()
{
    MINBASECLI Cli;

    delay_init();
    led_init();

    // Setup Command Line Interface
    cli_init(&Cli);

    while (1)
    {
        // Check and Handle Commands
        Cli.run();

        delay_ms(10U);
    }

    return 0;
}

/*****************************************************************************/

/* Command Line Interface */

void cli_init(MINBASECLI* Cli)
{
    // Setup Command Line Interface
    Cli->setup(USART1, UART_BAUD_RATE);

    // Add commands and bind callbacks to them
    Cli->add_cmd("led", &cmd_led, "led [on/off], Turn LED ON or OFF..");
    Cli->add_cmd("version", &cmd_version, "Shows current firmware version.");

    // The "help" command is already builtin and available from the CLI, and it
    // will shows added command descriptions, but you can setup a custom one
    Cli->add_cmd("help", &cmd_help, "Shows program help information.");

    Cli->printf("\nCommand Line Interface is ready\n\n");
}

void cmd_help(MINBASECLI* Cli, int argc, char* argv[])
{
    // Show some Info text
    Cli->printf("\nCustom Help Command\n");
    Cli->printf("MINBASECLI basic_usage_callbacks %s\n", FW_APP_VERSION);

    // Call the builtin "help" function to show added command descriptions
    Cli->cmd_help(argc, argv);
}

void cmd_led(MINBASECLI* Cli, int argc, char* argv[])
{
    bool invalid_argv = false;

    if (argc == 0)
    {   invalid_argv = true;   }
    else
    {
        char* test_mode = argv[0];
        if (strcmp(test_mode, "on") == 0)
        {
            Cli->printf("Turning LED ON.\n");
            led_on();
        }
        else if (strcmp(test_mode, "off") == 0)
        {
            Cli->printf("Turning LED OFF.\n");
            led_off();
        }
        else
        {   invalid_argv = true;   }
    }

    if (invalid_argv)
    {   Cli->printf("led command needs 'on' or 'off' arg.\n");   }

    Cli->printf("\n");
}

void cmd_version(MINBASECLI* Cli, int argc, char* argv[])
{
    Cli->printf("FW App Version: %s\n", FW_APP_VERSION);
}

/*****************************************************************************/

/* LED Control */

void led_init()
{
    GPIO_InitTypeDef gpio_config;
    GPIO_StructInit(&gpio_config);
    gpio_config.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_config.GPIO_Pin = LED_PIN;
    gpio_config.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &gpio_config);
}

void led_on(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

void led_off(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}

/*****************************************************************************/

/* Delay */

static uint8_t  us = 0;
static uint16_t ms = 0;

void delay_init()
{
    us = static_cast<uint8_t>(SystemCoreClock / 8000000U);
    ms = static_cast<uint16_t>(us * 1000U);
}

void delay_ms(const uint32_t n)
{
    uint32_t i;

    SysTick->SR &= ~(1 << 0);
    i = static_cast<uint32_t>(n * ms);

    SysTick->CMP = i;
    SysTick->CNT = 0;
    SysTick->CTLR |=(1 << 0);

    while((SysTick->SR & (1 << 0)) != (1 << 0));
    SysTick->CTLR &= ~(1 << 0);
}

/*****************************************************************************/
