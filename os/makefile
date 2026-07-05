CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

SCRS_S = $(wildcard *.S) $(wildcard *.s)
SCRS_C = $(wildcard *.c)

OBJS = $(SCRS_S:.S=.o) $(SCRS_C:.c=.o)

os.elf: ${OBJS}
	$(CC) $(CFLAGS) -T os.ld -Wl,-Map=os.map -o os.elf $^
	$(OBJCOPY) -O binary os.elf os.bin

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin