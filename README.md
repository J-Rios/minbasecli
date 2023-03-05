# MinBaseCLI

A minimal Command Line Interface C++ library implementation with HAL emphasis to be used in different kind of devices and frameworks.

## Supported Devices & Frameworks

Current devices/frameworks supported:

- Arduino
- AVR
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

2. Include new device support sources in "minbasecli_hal_select.h" using an existing global define that allows to know which device/framework is being compiled (this define should exists in some way in device framework, otherwise, you need to pass it to the compiler at build time, i.e. using -DMY_NEW_DEV for gcc compiler):

```c++

/* ... */

/*****************************************************************************/

/* Hardware Abstraction Layer: NewDevice */

#elif defined(MY_NEW_DEV)

    // Interface HAL Selection
    #include "hal/newdev/minbasecli_newdev.h"
    #define MINBASECLI_HAL MINBASECLI_NEWDEV

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

/* ... */

```

3. Implement low level layer functionalities to control interface of the CLI in the **minbasecli_newdev.h/cpp** files:

```c++

/* ... */

bool MINBASECLI_NEWDEV::hal_setup(void* iface, const uint32_t baud_rate)
{
    // Specific device/framework interface initialization
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

void MINBASECLI_ARDUINO::hal_iface_print(const uint8_t data_byte)
{
    // Specific device/framework interface print text
    // should be written here
}

/* ... */

```
