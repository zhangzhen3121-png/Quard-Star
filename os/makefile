CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

SCRS_S = \
	entry.S
SCRS_C = \
	main.c \
	sbi.c \

OBJS = $(SCRS_S:.S=.o) $(SCRS_C:.c=.o)

os.elf: ${OBJS}
	$(CC) $(CFLAGS) -T os.ld -o os.elf $^
	$(OBJCOPY) -O binary os.elf os.bin

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin