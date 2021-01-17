# MinBaseCLI

A minimal Command Line Interface C++ library implementation with HAL emphasis to be used in different kind of devices and frameworks.

## Supported Devices & Frameworks

Currently, just Arduino devices are compatible, however is simply to add support to other devices and frameworks.

## Adding new Device Support

To add a new device or framework support to library, yo just need to write the low level functionalities to control interface of the CLI in the **hal_*()** private methods (*hal_iface_available(), hal_iface_read(), hal_iface_print(), etc.*).

```cpp

// ...

// Device/Framework Libraries
#if defined(ARDUINO)
    #include <Arduino.h>
#elif defined(ESP_IDF)
    // Unimplemented
#elif defined(SAM_ASF)
    // Unimplemented
#elif defined(__AVR__)
    // Unimplemented
#endif

// ...

uint8_t MINBASECLI::hal_iface_read(void)
{
    // Do nothing if interface has not been initialized
    if(iface_is_not_initialized())
        return 0;

    #if defined(ARDUINO)
        #if defined(USBCON) // Arduinos: Leonardo, Micro, MKR, etc
            Serial_* _Serial = (Serial_*)this->iface;
        #else // Arduinos: UNO, MEGA, Nano, etc
            HardwareSerial* _Serial = (HardwareSerial*)this->iface;
        #endif
        return _Serial->read();
    #elif defined(ESP_IDF)
        // Unimplemented
    #elif defined(SAM_ASF)
        // Unimplemented
    #elif defined(__AVR__)
        // Unimplemented
    #endif

    return 0;
}
```

## Donation

[![paypal](https://www.paypal.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/josrios)

Do you like this project?

Please, considere making a donation.
