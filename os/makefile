CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -g

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

OS_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
INCLUDE := -I$(OS_DIR)/include/timeros -I$(OS_DIR)/include

SRCS_S := $(wildcard $(OS_DIR)/src/*.S) $(wildcard $(OS_DIR)/src/*.s)
SRCS_C := $(wildcard $(OS_DIR)/src/*.c) $(wildcard $(OS_DIR)/lib/*.c)

OBJS := $(patsubst %.S,%.o,$(patsubst %.s,%.o,$(SRCS_S))) \
	$(patsubst %.c,%.o,$(SRCS_C))

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(OS_DIR)/os.bin

$(OS_DIR)/os.elf: $(OBJS) $(OS_DIR)/os.ld
	$(CC) $(CFLAGS) $(INCLUDE) -T $(OS_DIR)/os.ld -Wl,-Map=$(OS_DIR)/os.map -o $@ $(OBJS)

$(OS_DIR)/os.bin: $(OS_DIR)/os.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<
%.o: %.s
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -f $(OBJS)  $(OS_DIR)/os.bin 

cleanall:
	rm -f $(OBJS)  $(OS_DIR)/os.bin $(OS_DIR)/os.map $(OS_DIR)/os.elf 