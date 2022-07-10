# avr-uart-driver echo example

UART low level driver for AVR devices "echo" example that reads all bytes received from an UART and write it back to it.

## Requirements

- Install required toolchain components:

```bash
sudo apt-get update
sudo apt-get -y install binutils
sudo apt-get -y install make
sudo apt-get -y install gcc-avr
sudo apt-get -y install avr-libc
sudo apt-get -y install cflow
```

- Install Flashing Tool:

```bash
sudo apt-get -y install avrdude
```

- Give execute permissions to tools scripts:

```bash
chmod +x vscode-skelly-avr/tools/*
```

## Usage

Build:

1. Physically connect the hardware programmer to target device and the PC.

2. Using a shell, go to **build/** directory and run **make** to see build instructions.

3. Run **make build DEVICE=X F_CPU=X** to launch the Build.

4. Run **make flash PROGRAMMER=X DEVICE=X F_CPU=X** to build and flash the firmware into target device.
