
/**
 * @file    minbasecli_avr.cpp
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

#if defined(__AVR) && !defined(ARDUINO)

/*****************************************************************************/

/* Libraries */

// Header Interface
#include "minbasecli_avr.h"

// Device/Framework Libraries
#if defined(MINBASECLI_USE_MILLIS)
    #include <avr/io.h>
    #include <avr/interrupt.h>
#endif

// (UART Driver)
#include "avr_uart.h"

// Standard Libraries
#include <string.h>

/*****************************************************************************/

/* Constants & Defines */

// Interface Element Data Type
#define _IFACE AvrUart

/*****************************************************************************/

/* In-Scope Global Elements */

#if defined(MINBASECLI_USE_MILLIS)
    volatile uint32_t systick;
#endif

/*****************************************************************************/

/* Constructor */

/**
  * @brief  Constructor, initialize internal attributes.
  */
MINBASECLI_AVR::MINBASECLI_AVR()
{
    this->iface = NULL;
}

/*****************************************************************************/

/* Specific Device/Framework HAL Methods */

/**
  * @brief  Setup and initialize UART for CLI interface.
  * @return If UART has been successfully initialized.
  */
bool MINBASECLI_AVR::hal_setup(const uint32_t baud_rate)
{
    _IFACE* _Serial = (_IFACE*) this->iface;
    _Serial->setup(baud_rate);
    return true;
}

/**
  * @brief  Hardware Abstraction Layer setup CLI interface.
  * @return If CLI has been successfully initialized.
  */
bool MINBASECLI_AVR::hal_setup(void* iface, const uint32_t baud_rate)
{
    this->iface = iface;
    return hal_setup(baud_rate);
}

/**
  * @brief  Check if the internal CLI HAL interface has received any data.
  * @return The number of bytes received by the interface.
  */
size_t MINBASECLI_AVR::hal_iface_available()
{
    _IFACE* _Serial = (_IFACE*) this->iface;

    return ((size_t) (_Serial->num_rx_data_available()));
}

/**
  * @brief  Read a byte from the CLI HAL interface.
  * @return The byte read from the interface.
  */
uint8_t MINBASECLI_AVR::hal_iface_read()
{
    uint8_t read_byte = 0;

    _IFACE* _Serial = (_IFACE*) this->iface;

    if (!_Serial->read(&read_byte))
        return 0;

    return read_byte;
}

/**
  * @brief  Print a byte with ASCII encode to CLI HAL interface.
  * @param  data_byte Byte of data to write.
  */
void MINBASECLI_AVR::hal_iface_print(const uint8_t data_byte)
{
    _IFACE* _Serial = (_IFACE*) this->iface;

    _Serial->write(data_byte);
}

/**
  * @brief  Timer1 setup and initialization to count System Tick.
  */
void MINBASECLI_AVR::hal_millis_init()
{
    #if defined(MINBASECLI_USE_MILLIS)
        unsigned long t_overflow;

        // Timer1 clock divisor 8 and clear at overflow
        TCCR1B |= (1 << WGM12) | (1 << CS11);

        // Set timer overflow for 1ms
        t_overflow = ((F_CPU / 1000) / 8);
        OCR1AH = (t_overflow >> 8);
        OCR1AL = t_overflow;

        // Enable Timer1 compare match interrupt
        TIMSK1 |= (1 << OCIE1A);

        // Enbale Global Interrupts
        sei();
    #endif /* defined(MINBASECLI_USE_MILLIS) */
}

/**
  * @brief  Get system-tick in ms (number of ms since system boot).
  * @return The number of milliseconds.
  */
uint32_t MINBASECLI_AVR::hal_millis()
{
    #if defined(MINBASECLI_USE_MILLIS)
        return systick;
    #else
        return 0;
    #endif /* defined(MINBASECLI_USE_MILLIS) */
}

/*****************************************************************************/

/* Millis Implementation */

#if defined(MINBASECLI_USE_MILLIS)

    /**
     * @brief  Timer1 Interrupt Service Rutine for System Tick counter.
     */
    ISR(TIMER1_COMPA_vect)
    {
        systick = systick + 1;
    }

#endif /* defined(MINBASECLI_USE_MILLIS) */

/*****************************************************************************/

#endif /* defined(__AVR) && !defined(ARDUINO) */
