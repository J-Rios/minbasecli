
/**
 * @file    minbasecli_none.h
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

#if defined(HAL_NONE)

#ifndef MINBASECLI_NONE_H_
#define MINBASECLI_NONE_H_

/*****************************************************************************/

/* Libraries */

// Standard C/C++ libraries
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************/
/* Class Interface */

class MINBASECLI_NONE
{
    public:
        MINBASECLI_NONE();

    protected:
        bool hal_setup();
        bool hal_setup(void* iface);
        size_t hal_iface_available();
        uint8_t hal_iface_read();
        void hal_iface_print(const uint8_t data_byte);
        void hal_millis_init();
        uint32_t hal_millis();

    private:
        void* iface;
};

/*****************************************************************************/

#endif /* MINBASECLI_NONE_H_ */

#endif /* defined(HAL_NONE) */
