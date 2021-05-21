
/**
 * @file    minbasecli.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    09-05-2021
 * @version 1.0.0
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
#elif defined(ESP_PLATFORM)
    #include "hal/espidf/minbasecli_espidf.h"
#elif defined(PICO_BOARD)
    #include "hal/pico/minbasecli_pico.h"
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
