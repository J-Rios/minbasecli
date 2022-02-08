# MinBaseCLI

A minimal Command Line Interface C++ library implementation with HAL emphasis to be used in different kind of devices and frameworks.

## Supported Devices & Frameworks

Current devices/frameworks supported:

- Arduino
- ESP-IDF
- STM32
- Linux
- Windows

It's simply to add support to other devices and frameworks, check next sections.

## Adding new Device Support

To add a new device/framework support to the library, you need to create a new Hardware Abstraction Layer component for it and implement the corresponding low level functions while maintain the same commons functions interface that minbasecli expose to be used...

1. Create a directory and sources in hal directory for a new device support (lets use "newdev" name as example):

```text
hal/newdev
    hal/newdev/minbasecli_newdev.h
    hal/newdev/minbasecli_newdev.cpp
```

**Note:** Use any of the other device support implementation of minbasecli_X.h/cpp files as example to edit.

2. Include new device support sources in "minbasecli.h" using an existing global define that allows to know wich device/framework is being compiled (this define should exists in some way in device framework, otherwise, you need to pass it to the compiler at build time, i.e. using -DMY_NEW_DEV for gcc compiler):

```c++

/* ... */

/* Use Specific HAL for build system */

#if defined(__linux__)
    #include "hal/linux/minbasecli_linux.h"
    #define MINBASECLI_HAL MINBASECLI_LINUX
#elif defined(_WIN32) || defined(_WIN64)
    #include "hal/windows/minbasecli_windows.h"
    #define MINBASECLI_HAL MINBASECLI_WINDOWS
#elif defined(ARDUINO)
    #include "hal/arduino/minbasecli_arduino.h"
    #define MINBASECLI_HAL MINBASECLI_ARDUINO
#elif defined(__AVR)
    #include "hal/avr/minbasecli_avr.h"
    #define MINBASECLI_HAL MINBASECLI_AVR
#elif defined(ESP_PLATFORM)
    #include "hal/espidf/minbasecli_espidf.h"
    #define MINBASECLI_HAL MINBASECLI_ESPIDF
#elif defined(MY_NEW_DEV) // This has been include
    #include "hal/pico/minbasecli_newdev.h" // This has been include
    #define MINBASECLI_HAL MINBASECLI_NEWDEV // This has been include
#else
    #warning "minbasecli - Unsupported device/system."
    #define HAL_NONE
    #include "hal/none/minbasecli_none.h"
    #define MINBASECLI_HAL MINBASECLI_NONE
#endif

/* ... */

```

3. Implement low level layer functionalities to control interface of the CLI in the **minbasecli_newdev.h/cpp** files:

```c++

/* ... */

bool MINBASECLI_NEWDEV::hal_setup(const uint32_t baud_rate)
{
    // Specific device/framework interface initialization
    // should be written here
}

bool MINBASECLI_NEWDEV::hal_setup(void* iface, const uint32_t baud_rate)
{
    // Specific device/framework interface initialization
    // should be written here
}

void MINBASECLI_NEWDEV::hal_iface_print(const char* str)
{
    // Specific device/framework interface print text
    // should be written here
}

void MINBASECLI_NEWDEV::hal_iface_println(const char* str)
{
    // Specific device/framework interface print text line
    // should be written here
}

size_t MINBASECLI_NEWDEV::hal_iface_available()
{
    // Specific device/framework return num bytes received from interface
    // should be written here
}

uint8_t MINBASECLI_NEWDEV::hal_iface_read(void)
{
    // Specific device/framework interface read and return incoming bytes
    // should be written here
}

uint32_t MINBASECLI_NEWDEV::hal_millis_init()
{
    // Specific device/framework system tick count initialization
    // should be written here
}

uint32_t MINBASECLI_NEWDEV::hal_millis()
{
    // Specific device/framework return system tick milliseconds
    // should be written here
}

/* ... */

```
