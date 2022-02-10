
/**
 * @file    minbasecli_stm32.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    10-02-2022
 * @version 1.0.2
 *
 * @section DESCRIPTION
 *
 * A simple Command Line Interface C++ library implementation with HAL
 * emphasis to be used in different kind of devices and frameworks.
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

/* Include Guard */

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F2)  || \
    defined(STM32F3) || defined(STM32F4) || defined(STM32F7)  || \
    defined(STM32L0) || defined(STM32L1) || defined(STM32L4)  || \
    defined(STM32L5) || defined(STM32G0) || defined(STM32G4)  || \
    defined(STM32H7) || defined(STM32U5) || defined(STM32MP1) || \
    defined(STM32WB) || defined(STM32WL)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_stm32.h"

// Standard Libraries
#include <stdio.h>
#include <string.h>

// Device/Framework Libraries
#if defined(STM32F0)
    #include "stm32f0xx_hal.h"
#elif defined(STM32F1)
    #include "stm32f1xx_hal.h"
#elif defined(STM32F2)
    #include "stm32f2xx_hal.h"
#elif defined(STM32F3)
    #include "stm32f3xx_hal.h"
#elif defined(STM32F4)
    #include "stm32f4xx_hal.h"
#elif defined(STM32F7)
    #include "stm32f7xx_hal.h"
#elif defined(STM32L0)
    #include "stm32l0xx_hal.h"
#elif defined(STM32L1)
    #include "stm32l1xx_hal.h"
#elif defined(STM32L4)
    #include "stm32l4xx_hal.h"
#elif defined(STM32L5)
    #include "stm32l5xx_hal.h"
#elif defined(STM32G0)
    #include "stm32g0xx_hal.h"
#elif defined(STM32G4)
    #include "stm32g4xx_hal.h"
#elif defined(STM32H7)
    #include "stm32h7xx_hal.h"
#elif defined(STM32U5)
    #include "stm32u5xx_hal.h"
#elif defined(STM32MP1)
    #include "stm32mp1xx_hal.h"
#elif defined(STM32WL)
    #include "stm32wlxx_hal.h"
#elif defined(STM32WB)
    #include "stm32wbxx_hal.h"
#else
    #error "minbasecli_stm32 - Unsupported device/system."
#endif

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#define _IFACE UART_HandleTypeDef

// Default USART
#define DEFAULT_UART USART2

// Default Baudrate
#define DEFAULT_BAUDS 19200

/*****************************************************************************/

/* In-Scope Static Private */

// Specific UART element
static UART_HandleTypeDef uart2;

// IT callback received byte
static uint8_t rx_byte[1];

// Read index that point to read buffer data FIFO element
static uint8_t rx_read_head = 0;
static uint8_t rx_read_tail = 0;

// Received bytes buffer
static uint8_t rx_buffer[SIMPLECLI_MAX_READ_SIZE];

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_STM32::MINBASECLI_STM32()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL functions */

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  */
bool MINBASECLI_STM32::hal_setup(const uint32_t baud_rate)
{
    this->iface = &uart2;
    if (!uart_setup(true))
        return false;
    return true;
}

/**
  * @brief  Initialize the Command Line Interface providing an interface.
  * @param  iface CLI interface to use.
  */
bool MINBASECLI_STM32::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    return hal_setup(baud_rate);
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI_STM32::hal_iface_available()
{
    return (rx_read_head - rx_read_tail);
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_STM32::hal_iface_read()
{
    // Ignore if there is no available bytes to be read
    if (hal_iface_available() == 0)
        return 0;

    // Return read bytes
    rx_read_tail = (rx_read_tail + 1) % SIMPLECLI_MAX_READ_SIZE;
    return rx_buffer[rx_read_tail];
}

/**
  * @brief  Print a byte with ASCII encode to CLI HAL interface.
  * @param  data_byte Byte of data to write.
  */
void MINBASECLI_STM32::hal_iface_print(const uint8_t data_byte)
{
    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    // Check if peripheral is ready to send data
    HAL_UART_StateTypeDef state = HAL_UART_GetState(_Serial);
    while ((state & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX);
    // No-Blocking instead above while:
    //if ((state & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX)
    //    return;

    // Transmit
    if (HAL_UART_Transmit_IT(_Serial, &data_byte, 1) != HAL_OK)
        return;

    return;
}

/**
  * @brief  Timer setup and initialization to count System Tick.
  */
void MINBASECLI_STM32::hal_millis_init()
{
    
    /* Nothing to be done, STM32 HAL already initialize a Timer for system
    tick count. */
}

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI_STM32::hal_millis()
{
    return ((uint32_t)(HAL_GetTick()));
}

/*****************************************************************************/

/* Private Methods */

/**
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI_STM32::uart_setup(const uint32_t baud_rate,
        const bool self_initialization)
{
    // Cast to specific interface type
    _IFACE* _Serial = (_IFACE*) this->iface;

    if (self_initialization)
    {
        // Setup UART Properties
        _Serial->Instance = DEFAULT_UART;
        _Serial->Init.BaudRate = baud_rate;
        _Serial->Init.WordLength = UART_WORDLENGTH_8B;
        _Serial->Init.StopBits = UART_STOPBITS_1;
        _Serial->Init.Parity = UART_PARITY_ODD;
        _Serial->Init.Mode = UART_MODE_TX_RX;
        _Serial->Init.HwFlowCtl = UART_HWCONTROL_NONE;
        _Serial->Init.OverSampling = UART_OVERSAMPLING_16;
        _Serial->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        _Serial->Init.ClockPrescaler = UART_PRESCALER_DIV1;
        _Serial->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

        // Initialize Peripheral
        if (HAL_UART_Init(_Serial) != HAL_OK)
            return false;
        if (HAL_UARTEx_SetTxFifoThreshold(_Serial, UART_TXFIFO_THRESHOLD_1_8) \
        != HAL_OK)
            return false;
        if (HAL_UARTEx_SetRxFifoThreshold(_Serial, UART_RXFIFO_THRESHOLD_1_8) \
        != HAL_OK)
            return false;
        if (HAL_UARTEx_DisableFifoMode(_Serial) != HAL_OK)
            return false;
    }

    // Start Async Reception for 1 bytes chunks and store it in `rx_buffer`
    // The reception callback will fire for each byte reception
    if(HAL_UART_Receive_IT(_Serial, (uint8_t*)rx_byte, 1) != HAL_OK)
        return false;

    return true;
}

/*****************************************************************************/

/* HAL UART Operation Callbacks */

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   UART Rx transfer completed callback (something was received).
  * @retval None
  */
static void HAL_UART_RxCpltCallback(UART_HandleTypeDef* UartHandle)
{
    // Increase number of received bytes
    rx_read_head = (rx_read_head + 1) % SIMPLECLI_MAX_READ_SIZE;
    rx_buffer[rx_read_head] = rx_byte[0];

    // Reload Async Reception
    if(HAL_UART_Receive_IT(_Serial, (uint8_t*)rx_byte, 1) != HAL_OK)
        return 0;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   UART Tx transfer completed callback (something was sent).
  * @retval None
  */
static void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   UART transfer operation error callback.
  * @retval None
  */
static void HAL_UART_ErrorCallback(UART_HandleTypeDef* UartHandle)
{}

/*****************************************************************************/

#endif /* STM32 */
