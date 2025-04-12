
/**
 * @file    minbasecli_ch32.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    13-04-2025
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * A simple Command Line Interface C++ library implementation with HAL
 * emphasis to be used in different kind of devices and frameworks.
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

/* Include Guard */

#if defined(CH32V00X) /* || defined(CH32V10X) || defined(CH32V20X) || \
    defined(CH32V30X) || defined(CH32L10X) || defined(CH32X03X) */

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_ch32.h"

// Standard C++ Libraries
#include <cstdio>
#include <cstring>

// Device/Framework Libraries
#if defined(CH32V00X)
    #include "ch32v00x.h"
#elif defined(CH32V10X)
    #include "ch32v10x.h"
#elif defined(CH32V20X)
    #include "ch32v20x.h"
#elif defined(CH32V30X)
    #include "ch32v30x.h"
#elif defined(CH32L10X)
    #include "ch32l103.h" // WCH didn't offer a common "ch32v10x.h" yet
#elif defined(CH32X03X)
    #include "ch32x035.h" // WCH didn't offer a common "ch32x03x.h" yet
#else
    #error "minbasecli_ch32 - Unsupported CH32 chip."
#endif

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#define _IFACE USART_TypeDef

// Default USART
#define DEFAULT_UART USART1

// Default Baudrate
#define DEFAULT_BAUDS 115200

/*****************************************************************************/

/* In-Scope Static Private */

// Read index that point to read buffer data FIFO element
static volatile uint8_t rx_read_head = 0;
static volatile uint8_t rx_read_tail = 0;

// Received bytes buffer
static volatile uint8_t rx_buffer[MINBASECLI_MAX_READ_SIZE];

/*****************************************************************************/

/* Constructor */

/**
 * @details
 * This constructor initializes all attributes of the CLI class.
 */
MINBASECLI_CH32::MINBASECLI_CH32()
{
    this->iface = DEFAULT_UART;
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
 * @details
 * This function should get and initialize the interface element that is going
 * to be used by the CLI.
 */
bool MINBASECLI_CH32::hal_setup(void* iface, const uint32_t baud_rate)
{
    if (iface == nullptr)
    {   return false;   }

    this->iface = iface;
    if (!uart_setup(baud_rate, true))
    {   return false;   }

    return true;
}

/**
 * @details
 * This function return the number of bytes received by the interface that are
 * available to be read.
 */
size_t MINBASECLI_CH32::hal_iface_available()
{
    return (rx_read_head - rx_read_tail);
}

/**
 * @details
 * This function returns a received byte from the interface. It checks if
 * there is any byte available to be read and increase the read circular
 * buffer tail index to "pop" this element from the buffer and return it.
 */
uint8_t MINBASECLI_CH32::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
    {   return 0;   }

    // Return read bytes
    uint8_t data_byte = rx_buffer[rx_read_tail];
    rx_read_tail = (rx_read_tail + 1) % MINBASECLI_MAX_READ_SIZE;
    return data_byte;
}

/**
 * @details
 * This function send a byte through the interface.
 */
void MINBASECLI_CH32::hal_iface_print(const uint8_t data_byte)
{
    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    // Check if peripheral is ready to send data
    while(USART_GetFlagStatus(_Serial, USART_FLAG_TXE) == RESET)
    {}

    // Transmit
    USART_SendData(_Serial, data_byte);

    return;
}

/*****************************************************************************/

/* Private Methods */

/**
 * @details
 * This function configure and initialize the UART for the given baud rate
 * communication speed. It configures the Serial communication parameters,
 * initializes the UART peripheral, setup the reception and transmission
 * thresholds, disable the FIFO mode and start the asynchronous Interrupt
 * detection for each received data byte.
 */
bool MINBASECLI_CH32::uart_setup(const uint32_t baud_rate,
        const bool self_initialization)
{
    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    if (self_initialization)
    {
        // Set GPIOs to use according to USART-N
        // Note only CH32V003 and UART1 is supported
        GPIO_TypeDef* gpio_usart_tx_port = nullptr;
        GPIO_TypeDef* gpio_usart_rx_port = nullptr;
        uint16_t gpio_usart_tx_pin = 0U;
        uint16_t gpio_usart_rx_pin = 0U;
        uint32_t rcc_usart_peripheral = 0U;
        uint32_t rcc_usart_gpio_port = 0U;
        uint8_t irq_channel = 0U;
        if (_Serial == USART1)
        {
            #if defined(CH32V003)
                gpio_usart_tx_port = GPIOD;
                gpio_usart_rx_port = GPIOD;
                gpio_usart_tx_pin = GPIO_Pin_5;
                gpio_usart_rx_pin = GPIO_Pin_6;
            #endif
            rcc_usart_peripheral = RCC_APB2Periph_USART1;
            rcc_usart_gpio_port = RCC_APB2Periph_GPIOD;
            irq_channel = USART1_IRQn;
        }
        else
        {   return false;   }
        if (gpio_usart_tx_port == nullptr)
        {   return false;   }

        // Reset USART & GPIO Peripheral Clocks
        RCC_APB2PeriphClockCmd(
            rcc_usart_peripheral | rcc_usart_gpio_port | RCC_APB2Periph_AFIO,
            ENABLE);

        // Setup USART-Tx Pin
        GPIO_InitTypeDef gpio_config;
        GPIO_StructInit(&gpio_config);
        gpio_config.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_config.GPIO_Pin = gpio_usart_tx_pin;
        gpio_config.GPIO_Speed = GPIO_Speed_30MHz;
        GPIO_Init(gpio_usart_tx_port, &gpio_config);

        // Setup USART-Rx Pin
        GPIO_StructInit(&gpio_config);
        gpio_config.GPIO_Mode = GPIO_Mode_IPU;
        gpio_config.GPIO_Pin = gpio_usart_rx_pin;
        gpio_config.GPIO_Speed = GPIO_Speed_30MHz;
        GPIO_Init(gpio_usart_rx_port, &gpio_config);

        // USART Clock Configuration
        USART_ClockInitTypeDef usart_clock_config;
        USART_ClockStructInit(&usart_clock_config);
        USART_ClockInit(_Serial, &usart_clock_config);

        // USART Configuration
        USART_InitTypeDef usart_config;
        USART_StructInit(&usart_config);
        usart_config.USART_BaudRate = baud_rate;
        usart_config.USART_WordLength = USART_WordLength_8b;
        usart_config.USART_StopBits = USART_StopBits_1;
        usart_config.USART_Parity = USART_Parity_No;
        usart_config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        usart_config.USART_HardwareFlowControl =
            USART_HardwareFlowControl_None;
        USART_Init(_Serial, &usart_config);

        // Setup and Enable USART Interrupts
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = irq_channel;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        USART_ITConfig(_Serial, USART_IT_RXNE, ENABLE);
        NVIC_Init(&NVIC_InitStructure);
        NVIC_EnableIRQ(USART1_IRQn);

        // Enable USART1 Peripheral
        USART_Cmd(_Serial, ENABLE);
    }

    return true;
}

/*****************************************************************************/

/* HAL UART Operation Callbacks */

#ifdef __cplusplus
extern "C"
{
    void USART1_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));
}
#endif

/**
 * @details
 * This function fires on each UART Rx reception. It get the last received
 * byte and store it into the circular buffer, and set again the interrupt for
 * next detections.
 */
void USART1_IRQHandler()
{
    // Handle USART RX Buffer not empty
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // Read buffer byte (read from DATAR clears the RXE IRQ flag)
        uint8_t rx_byte = USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

        // Increase number of received bytes
        uint8_t next_head = (rx_read_head + 1) % MINBASECLI_MAX_READ_SIZE;

        // If software buffer is not full, store read byte into it
        if (next_head != rx_read_tail)
        {
            rx_buffer[rx_read_head] = rx_byte;
            rx_read_head = next_head;
        }
        else
        {
            // Handle software buffer full
        }
    }
}

/*****************************************************************************/

#endif /* CH32 */
