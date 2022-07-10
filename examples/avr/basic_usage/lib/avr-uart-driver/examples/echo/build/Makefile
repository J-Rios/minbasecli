
# Specify project name and default compilers
PROJECTNAME = uartecho

# UART Driver path
UARTDRIVERPATH = $(WORKSPACE)/../../src

# Project root WorkSpace
WORKSPACE = ..

# Build and Binary Release directories
BUILDDIR = $(WORKSPACE)/build
BINDIR = $(WORKSPACE)/bin

# Specify target Microcontroller
ifndef $(DEVICE)
	DEVICE = none
endif

# Specify MCU CPU Frequency
ifndef $(F_CPU)
	F_CPU = 16000000UL
endif

# Low, High and Extended Fuses Values
ifndef $(LFU)
	LFU = "none"
endif
ifndef $(LFU)
	HFU = "none"
endif
ifndef $(LFU)
	EFU = "none"
endif

# Avrdude Programmer
ifndef $(PROGRAMMER)
	PROGRAMMER = usbasp
endif

###############################################################################

# Get actual date and setup output binary directory name
#DATE = $(shell date '+%Y_%m_%d_%H_%M_%S')
DATE = $(shell date '+%Y_%m_%d')
RELEASEDIR = $(WORKSPACE)/bin/$(DATE)

# Specify Sources files (Automatic search in specific directories)
SRCS  = $(shell find $(WORKSPACE)/src -type f -name *.c)
SRCS  += $(shell find $(WORKSPACE)/src -type f -name *.cpp)
SRCS  += $(shell find $(WORKSPACE)/lib -type f -name *.c)
SRCS  += $(shell find $(WORKSPACE)/lib -type f -name *.cpp)
SRCS  += $(shell find $(UARTDRIVERPATH) -type f -name *.c)
SRCS  += $(shell find $(UARTDRIVERPATH) -type f -name *.cpp)

# Specify Headers files (Automatic search in specific directories)
HEADS = $(shell find $(WORKSPACE)/inc -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/inc -type f -name *.hpp)
HEADS += $(shell find $(WORKSPACE)/src -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/src -type f -name *.hpp)
HEADS += $(shell find $(WORKSPACE)/lib -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/lib -type f -name *.hpp)
HEADS += $(shell find $(UARTDRIVERPATH) -type f -name *.h)
HEADS += $(shell find $(UARTDRIVERPATH) -type f -name *.hpp)

# Get Headers directories from headers paths
HEADERSDIR = $(shell echo $(HEADS) | xargs -n1 dirname | sort -u | xargs -n1 -i echo "-I{}")
HEADERSDIR += $(shell echo $(HEADS) | xargs -n1 dirname | sort -u | xargs -n1 -i echo "-L{}")

# Get objects files from sources and output object
_OBJS = $(SRCS:.c=.o)
OBJS = $(_OBJS:.cpp=.o)
BUILDOBJS = $(shell echo $(OBJS) | xargs -n1 basename | xargs -n1 -i echo "$(BUILDDIR)/{}")

###############################################################################

# Specify default compilers and tools
CC      = avr-gcc
CXX     = avr-g++
AVRDUDE = avrdude -c $(PROGRAMMER) -p $(DEVICE)
AVRDUDE_ARDUINO = avrdude -c arduino -b 115200 -P /dev/ttyUSB0 -p $(DEVICE)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(DEVICE)

# Setup compilation flags
CFLAGS = -Os -Wall -g -fstack-usage -flto
CFLAGS += '-mmcu=$(DEVICE)'
CFLAGS += '-DF_CPU=$(F_CPU)'
ifeq ($(TEST), 1)
	CFLAGS += -DTEST
endif
CFLAGS += $(HEADERSDIR)
CXXFLAGS = $(CFLAGS)

# Flash Section to place the binary
# Application placed at "Flash_Start" (0x0000)
# Bootloader placed at "Flash_End" - "Boot_Section_Size"
#   Boot Section size must be defined with BOOTSZ0 and BOOTSZ1 fuses
#   Example:
#     Device of 128KB Flash with 4KB Boot Section: (128-4)*1024 = 0x1F000
ifeq ($(BOOTLOADER), 1)
	PRJ = 'fw-boot-$(PROJECTNAME)'
	FLASHSECTION = --section-start=.text=0x1F000
else
	PRJ = 'fw-app-$(PROJECTNAME)'
	FLASHSECTION = --section-start=.text=0x00000
	BOOTLOADER=0
endif

# Linker flags
LDFLAGS = '-Wl,$(FLASHSECTION)'

# Compile instruction
COMPILE_C = $(CC) $(CFLAGS) $(LDFLAGS)
COMPILE_CXX = $(CXX) $(CXXFLAGS) $(LDFLAGS)

###############################################################################

help:
	@cat README.md

# Target: make all (build project generating output directory)
build: $(PRJ).bin
	rm -f ${RELEASEDIR}/*
	mkdir -p ${RELEASEDIR}
	cp -a $(PRJ).elf $(RELEASEDIR)
	cp -a $(PRJ).hex $(RELEASEDIR)
	cp -a $(PRJ).bin $(RELEASEDIR)
	@echo "ADDRESS    SIZE    FUNCTION    FILE:LINE\n" > memory_program.txt
	@$(NM) $(PRJ).elf >> memory_program.txt
	@chmod +x $(BUILDDIR)/../tools/*
	@$(BUILDDIR)/../tools/memusageram $(BUILDDIR)/memory_ram.txt >/dev/null 2>&1
	@rm -f $(BUILDDIR)/*.su
	@mkdir -p $(BUILDDIR)/obj
	@mv $(BUILDDIR)/*.o $(BUILDDIR)/obj/
	$(SIZE) $(PRJ).elf
	@if [ ${BOOTLOADER} = "1" ]; then\
		echo "Bootloader Built";\
	else\
		echo "Application Built";\
	fi
	@$(BUILDDIR)/../tools/binary_size $(PRJ).bin
	@echo ""

# Target: make clean (remove all previously builds)
clean:
	rm -rf $(BUILDDIR)/obj
	rm -f $(BUILDDIR)/*.su
	rm -f $(BUILDDIR)/$(PRJ).elf
	rm -f $(BUILDDIR)/$(PRJ).hex
	rm -f $(BUILDDIR)/$(PRJ).bin
	rm -f $(BUILDDIR)/dump.bin
	rm -f $(BUILDDIR)/dump.hex
	rm -f $(BUILDDIR)/memory_program.txt
	rm -f $(BUILDDIR)/memory_ram.txt

# Target: make rebuild (clean previously builds and build again)
rebuild: clean build

# Flash program to MCU
flash: build
	$(AVRDUDE) -U flash:w:$(PRJ).hex:i

# Flash program to MCU
flash_arduino: build
	$(AVRDUDE_ARDUINO) -U flash:w:$(PRJ).hex:i

# Write fuses to MCU
fuse:
	$(AVRDUDE) -U lfuse:w:$(LFU):m -U hfuse:w:$(HFU):m -U efuse:w:$(EFU):m

# Read flash memory from MCU
dump:
	rm -f $(BUILDDIR)/dump.bin
	rm -f $(BUILDDIR)/dump.hex
	$(AVRDUDE) -U flash:r:dump.bin:r
	$(OBJCOPY) -I binary -O ihex dump.bin dump.hex

# Target: check (custom target to check build variables)
check:
	@echo "HEADERS:"
	@echo "  $(HEADS)"
	@echo "SRCS:"
	@echo "  $(SRCS)"
	@echo "BUILD FLAGS:"
	@echo "  $(CFLAGS)"
	@echo "LINK FLAGS:"
	@echo "  $(LDFLAGS)"
	@echo "OBJS:"
	@echo "  $(BUILDOBJS)"
	@echo "BINDIR:"
	@echo "  $(BINDIR)"

###############################################################################

# Target for generate BIN file from .elf file
$(PRJ).bin: $(PRJ).hex
	rm -f $(PRJ).bin
	$(OBJCOPY) -j .text -j .data -O binary $(PRJ).elf $(PRJ).bin

# Target for generate HEX file from .elf file
$(PRJ).hex: $(PRJ).elf
	rm -f $(PRJ).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PRJ).elf $(PRJ).hex

# Target for generate ELF file linking all .o files
$(PRJ).elf: $(OBJS)
	$(COMPILE_C) -o $(PRJ).elf $(BUILDOBJS)

# Target for generate object file of each .c file
%.o: %.c
	$(COMPILE_C) -c $<

# Target for generate object file of each .cpp file
%.o: %.cpp
	$(COMPILE_CXX) -c $<
