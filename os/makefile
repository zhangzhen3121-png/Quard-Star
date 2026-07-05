CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

INCLUDE := -I ./include/timeros

SRCS_S = $(wildcard src/*.S) $(wildcard src/*.s)
SRCS_C = $(wildcard src/*.c) $(wildcard lib/*.c) 

OBJS = $(SRCS_S:.S=.o) $(SRCS_C:.c=.o)

os.elf: ${OBJS}
	$(CC) $(CFLAGS) $(INCLUDE) -T os.ld -Wl,-Map=os.map -o os.elf $^
	$(OBJCOPY) -O binary os.elf os.bin

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin
	rm -f ./src/*.o ./src/*.elf ./src/*.bin
	rm -f ./lib/*.o ./lib/*.elf ./lib/*.bin