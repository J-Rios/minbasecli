
/**
 * @file    minbasecli_hal_select.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    04-03-2023
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * MINBASECLI HAL selection and configuration file.
 *
 * @section LICENSE
 *
 * Copyright (c) 2023 Jose Miguel Rios Rubio. All right reserved.
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

#ifndef MINBASECLI_SELECT_H_
#define MINBASECLI_SELECT_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/

/* Hardware Abstraction Layer: Linux */

#if defined(__linux__)

    // Interface HAL Selection
    #include "hal/linux/minbasecli_linux.h"
    #define MINBASECLI_HAL MINBASECLI_LINUX

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 115200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 256
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 64
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 32
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 8
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 64
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 128
    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: Windows */

#elif defined(_WIN32) || defined(_WIN64)

    // Interface HAL Selection
    #include "hal/windows/minbasecli_windows.h"
    #define MINBASECLI_HAL MINBASECLI_WINDOWS

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 115200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 256
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 64
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 32
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 8
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 64
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 128
    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: Arduino */

#elif defined(ARDUINO)

    // Interface HAL Selection
    #include "hal/arduino/minbasecli_arduino.h"
    #define MINBASECLI_HAL MINBASECLI_ARDUINO

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Low Resources AVRs
    #if defined(ARDUINO_ARCH_AVR)

        // Default CLI Interface to use if not provided
        #if !defined(MINBASECLI_DEFAULT_IFACE)
            #define MINBASECLI_DEFAULT_IFACE NULL
        #endif

        // Default CLI Baud Rate Speed to use if not provided
        #if !defined(MINBASECLI_DEFAULT_BAUDS)
            #define MINBASECLI_DEFAULT_BAUDS 19200
        #endif

        // Maximum CLI read buffer size
        #if !defined(MINBASECLI_MAX_READ_SIZE)
            #define MINBASECLI_MAX_READ_SIZE 32
        #endif

        // Maximum CLI Command length
        #if !defined(MINBASECLI_MAX_CMD_LEN)
            #define MINBASECLI_MAX_CMD_LEN 16
        #endif

        // Maximum CLI Command Argument length
        #if !defined(MINBASECLI_MAX_ARGV_LEN)
            #define MINBASECLI_MAX_ARGV_LEN 12
        #endif

        // Maximum number of arguments to check on a received CLI command
        #if !defined(MINBASECLI_MAX_ARGV)
            #define MINBASECLI_MAX_ARGV 2
        #endif

        // Maximum Print formatted number array size
        #if !defined(MINBASECLI_MAX_PRINT_SIZE)
            #define MINBASECLI_MAX_PRINT_SIZE 22
        #endif

        // Maximum number of commands that can be added to the CLI
        #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
            #define MINBASECLI_MAX_CMD_TO_ADD 10
        #endif

        // Maximum length of command description text
        #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
            #define MINBASECLI_MAX_CMD_DESCRIPTION 32
        #endif

    #else // ARM, SAM, SAMD, STM32, ESP32, ESP8266, etc.

        // Default CLI Baud Rate Speed to use if not provided
        #if !defined(MINBASECLI_DEFAULT_BAUDS)
            #define MINBASECLI_DEFAULT_BAUDS 115200
        #endif

        // Maximum CLI read buffer size
        #if !defined(MINBASECLI_MAX_READ_SIZE)
            #define MINBASECLI_MAX_READ_SIZE 64
        #endif

        // Maximum CLI Command length
        #if !defined(MINBASECLI_MAX_CMD_LEN)
            #define MINBASECLI_MAX_CMD_LEN 24
        #endif

        // Maximum CLI Command Argument length
        #if !defined(MINBASECLI_MAX_ARGV_LEN)
            #define MINBASECLI_MAX_ARGV_LEN 32
        #endif

        // Maximum number of arguments to check on a received CLI command
        #if !defined(MINBASECLI_MAX_ARGV)
            #define MINBASECLI_MAX_ARGV 4
        #endif

        // Maximum Print formatted number array size
        #if !defined(MINBASECLI_MAX_PRINT_SIZE)
            #define MINBASECLI_MAX_PRINT_SIZE 22
        #endif

        // Maximum number of commands that can be added to the CLI
        #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
            #define MINBASECLI_MAX_CMD_TO_ADD 16
        #endif

        // Maximum length of command description text
        #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
            #define MINBASECLI_MAX_CMD_DESCRIPTION 64
        #endif

    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: AVR */

#elif defined(__AVR)

    // Interface HAL Selection
    #include "hal/avr/minbasecli_avr.h"
    #define MINBASECLI_HAL MINBASECLI_AVR

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 19200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 32
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 16
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 12
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 2
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 10
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 32
    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: ESP-IDF */

#elif defined(ESP_PLATFORM)

    // Interface HAL Selection
    #include "hal/espidf/minbasecli_espidf.h"
    #define MINBASECLI_HAL MINBASECLI_ESPIDF

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 115200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 64
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 24
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 32
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 4
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 16
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 64
    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: STM32 */

#elif defined(STM32F0) || defined(STM32F1) || defined(STM32F2) \
|| defined(STM32G0) || defined(STM32G4) || defined(STM32H7) \
|| defined(STM32F3) || defined(STM32F4) || defined(STM32F7) \
|| defined(STM32L0) || defined(STM32L1) || defined(STM32L4) \
|| defined(STM32L5) || defined(STM32MP1) || defined(STM32U5) \
|| defined(STM32WB) || defined(STM32WL)

    // Interface HAL Selection
    #include "hal/stm32/minbasecli_stm32.h"
    #define MINBASECLI_HAL MINBASECLI_STM32

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 115200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 64
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 24
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 32
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 4
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 16
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 64
    #endif

/*****************************************************************************/

/* Hardware Abstraction Layer: None */

#else

    // Interface HAL Selection
    #warning "minbasecli - Unsupported device/system."
    #define HAL_NONE
    #include "hal/none/minbasecli_none.h"
    #define MINBASECLI_HAL MINBASECLI_NONE

    // Default CLI Interface to use if not provided
    #if !defined(MINBASECLI_DEFAULT_IFACE)
        #define MINBASECLI_DEFAULT_IFACE NULL
    #endif

    // Default CLI Baud Rate Speed to use if not provided
    #if !defined(MINBASECLI_DEFAULT_BAUDS)
        #define MINBASECLI_DEFAULT_BAUDS 115200
    #endif

    // Maximum CLI read buffer size
    #if !defined(MINBASECLI_MAX_READ_SIZE)
        #define MINBASECLI_MAX_READ_SIZE 64
    #endif

    // Maximum CLI Command length
    #if !defined(MINBASECLI_MAX_CMD_LEN)
        #define MINBASECLI_MAX_CMD_LEN 24
    #endif

    // Maximum CLI Command Argument length
    #if !defined(MINBASECLI_MAX_ARGV_LEN)
        #define MINBASECLI_MAX_ARGV_LEN 32
    #endif

    // Maximum number of arguments to check on a received CLI command
    #if !defined(MINBASECLI_MAX_ARGV)
        #define MINBASECLI_MAX_ARGV 4
    #endif

    // Maximum Print formatted number array size
    #if !defined(MINBASECLI_MAX_PRINT_SIZE)
        #define MINBASECLI_MAX_PRINT_SIZE 22
    #endif

    // Maximum number of commands that can be added to the CLI
    #if !defined(MINBASECLI_MAX_CMD_TO_ADD)
        #define MINBASECLI_MAX_CMD_TO_ADD 16
    #endif

    // Maximum length of command description text
    #if !defined(MINBASECLI_MAX_CMD_DESCRIPTION)
        #define MINBASECLI_MAX_CMD_DESCRIPTION 64
    #endif

#endif

/*****************************************************************************/

#endif /* MINBASECLI_SELECT_H_ */
