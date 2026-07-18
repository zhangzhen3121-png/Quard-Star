CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

INCLUDE := -I ../include/timeros -I ../include


LIB =  ../lib/printf.c \
       ../lib/string.c \
       ../lib/syscall.c


write: write.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=write.map -o ./bin/write $^

time: time.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=time.map -o ./bin/time $^

clean:
	rm -f *.o *.elf *.bin

