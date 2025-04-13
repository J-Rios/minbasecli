
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

// Auxiliary Functions
static inline uint8_t _strcmp(const char* str_a, const char* str_b);

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
        if (_strcmp(test_mode, "on") == 0)
        {
            Cli->printf("Turning LED ON.\n");
            led_on();
        }
        else if (_strcmp(test_mode, "off") == 0)
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

    led_off();
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

/* Auxiliary Functions */

// Lightweight implementation of stdlib strcmp() function
static inline uint8_t _strcmp(const char* str_a, const char* str_b)
{
    while (*str_a == *str_b)
    {
        if (*str_a == '\0')
        {   return 0U;   }

        str_a = str_a + 1U;
        str_b = str_b + 1U;
    }

    return 1U;
}

/*****************************************************************************/
