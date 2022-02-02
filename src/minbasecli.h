
/**
 * @file    minbasecli.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    02-02-2022
 * @version 1.0.1
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

#ifndef MAIN_MINBASECLI_H_
#define MAIN_MINBASECLI_H_

/*****************************************************************************/

/* Use Specific HAL for build system */

#if defined(ARDUINO)
    #include "hal/arduino/minbasecli_arduino.h"
#elif defined(__AVR)
    #include "hal/avr/minbasecli_avr.h"
#elif defined(ESP_PLATFORM)
    #include "hal/espidf/minbasecli_espidf.h"
#elif defined(STM32F0) || defined(STM32F1) || defined(STM32F2) \
|| defined(STM32G0) || defined(STM32G4) || defined(STM32H7) \
|| defined(STM32F3) || defined(STM32F4) || defined(STM32F7) \
|| defined(STM32L0) || defined(STM32L1) || defined(STM32L4) \
|| defined(STM32L5) || defined(STM32MP1) || defined(STM32U5) \
|| defined(STM32WB) || defined(STM32WL)
    #include "hal/stm32/minbasecli_stm32.h"
#elif defined(__linux__)
    #include "hal/linux/minbasecli_linux.h"
#elif defined(_WIN32) || defined(_WIN64)
    #include "hal/windows/minbasecli_windows.h"
#else
    #warning "minbasecli - Unsupported device/system."
    #include "hal/none/minbasecli_none.h"
#endif

/*****************************************************************************/

#endif /* MAIN_MINBASECLI_H_ */
