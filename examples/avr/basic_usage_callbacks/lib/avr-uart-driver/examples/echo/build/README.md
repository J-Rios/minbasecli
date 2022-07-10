# Build Help

```bash
Usage: make <TARGET> [OPTION=]...
Targets:
    help: Show build usage information (current file).
    check: Show build parameters.
    clean: Clean last built files.
    build: Build the project.
    rebuild: Force clean last built files and build again.
    flash: Build and write firmware to target device.
    fuses: Write fuses to target device.
    dump: Read target device flash memory into a dump.hex file.
Options:
    DEVICE: Specify MCU device to build/flash (check list of availables with 'avrdude -c usbasp -p none').
    F_CPU: Specify MCU CPU Frequency to use (16MHz if not provided).
    PROGRAMMER: Specify avrdude programmer to use (check the list using 'avrdude -c none').
    LFU/HFU/EFU: Specify High/Low/Extended Fuses values to program.
    TEST: Build a Test Firmware (add TEST global Define to code).
    BOOTLOADER: Build as Bootloader.
```

# Build Examples

```bash
make help
make check
make clean
make build DEVICE=atmega328p F_CPU=16000000L
make build DEVICE=atmega328p F_CPU=16000000L BOOTLOADER=1
make build DEVICE=t85 F_CPU=1000000L TEST=1
make rebuild DEVICE=atmega328p
make flash PROGRAMMER=usbasp # arduino, atmelice, jtagmkII, usbtiny, stk500...
make fuses LFU=0xFF HFU=0xDE EFU=0xFD
make dump
```
