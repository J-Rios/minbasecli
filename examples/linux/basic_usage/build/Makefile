
# Specify project name and default compilers
PRJ = minbasecli_basicusage

# UART Driver path
MINBASECLIPATH = $(WORKSPACE)/../../../src

# Project root WorkSpace
WORKSPACE = ..

# Build and Binary Release directories
BUILDDIR = $(WORKSPACE)/build
BINDIR = $(WORKSPACE)/bin

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

# MinBaseCLI Sources
SRCS  += $(MINBASECLIPATH)/minbasecli.cpp
SRCS  += $(MINBASECLIPATH)/hal/linux/minbasecli_linux.cpp

# Specify Headers files (Automatic search in specific directories)
HEADS = $(shell find $(WORKSPACE)/inc -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/inc -type f -name *.hpp)
HEADS += $(shell find $(WORKSPACE)/src -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/src -type f -name *.hpp)
HEADS += $(shell find $(WORKSPACE)/lib -type f -name *.h)
HEADS += $(shell find $(WORKSPACE)/lib -type f -name *.hpp)

# MinBaseCLI Headers
HEADS += $(MINBASECLIPATH)/minbasecli.h
HEADS += $(MINBASECLIPATH)/hal/linux/minbasecli_linux.h

# Get Headers directories from headers paths
HEADERSDIR = $(shell echo $(HEADS) | xargs -n1 dirname | sort -u | xargs -n1 -i echo "-I{}")
HEADERSDIR += $(shell echo $(HEADS) | xargs -n1 dirname | sort -u | xargs -n1 -i echo "-L{}")

# Get objects files from sources and output object
_OBJS = $(SRCS:.c=.o)
OBJS = $(_OBJS:.cpp=.o)
BUILDOBJS = $(shell echo $(OBJS) | xargs -n1 basename | xargs -n1 -i echo "$(BUILDDIR)/{}")

###############################################################################

# Specify default compilers and tools
CC      = gcc
CXX     = g++
OBJCOPY = objcopy
OBJDUMP = objdump
NM      = nm --line-numbers --print-size --size-sort --radix=d
SIZE    = size

# Setup compilation flags
CFLAGS = -Os -Wall -g -fstack-usage -flto -pthread 
ifeq ($(TEST), 1)
	CFLAGS += -DTEST
endif
CFLAGS += $(HEADERSDIR)
CXXFLAGS = $(CFLAGS)

# Linker flags
LDFLAGS = -lpthread

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
	@../tools/memusageram $(BUILDDIR)/memory_ram.txt >/dev/null 2>&1
	@rm -f $(BUILDDIR)/*.su
	@mkdir -p $(BUILDDIR)/obj
	@mv $(BUILDDIR)/*.o $(BUILDDIR)/obj/
	$(SIZE) $(PRJ).elf
	@../tools/binary_size $(PRJ).bin
	@echo ""

# Target: make clean (remove all previously builds)
clean:
	rm -rf $(BUILDDIR)/obj
	rm -f $(BUILDDIR)/*.su
	rm -f $(BUILDDIR)/$(PRJ).elf
	rm -f $(BUILDDIR)/$(PRJ).hex
	rm -f $(BUILDDIR)/$(PRJ).bin
	rm -f $(BUILDDIR)/memory_program.txt
	rm -f $(BUILDDIR)/memory_ram.txt

# Target: make rebuild (clean previously builds and build again)
rebuild: clean build

# Target: check (custom target to check build variables)
check:
	@echo ""
	@echo "HEADERS:"
	@echo "  $(HEADS)"
	@echo ""
	@echo "SRCS:"
	@echo "  $(SRCS)"
	@echo ""
	@echo "BUILD FLAGS:"
	@echo "  $(CFLAGS)"
	@echo ""
	@echo "LINK FLAGS:"
	@echo "  $(LDFLAGS)"
	@echo ""
	@echo "OBJS:"
	@echo "  $(BUILDOBJS)"
	@echo ""
	@echo "BINDIR:"
	@echo "  $(BINDIR)"
	@echo ""

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
