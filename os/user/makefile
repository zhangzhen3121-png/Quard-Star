CROSS_COMPILE ?= riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -g3 -O0

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

INCLUDE := -I ../include/timeros -I ../include


LIB =  ../lib/printf.c \
       ../lib/string.c \
       ../lib/syscall.c \
	   ../lib/getchar.c


write: write.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=write.map -o ./bin/write $^

time: time.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=time.map -o ./bin/time $^

xec: xec.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=xec.map -o ./bin/xec $^

shell: user_shell.c ${LIB}
	$(CC) $(CFLAGS) $(INCLUDE) -T user.ld -Wl,-Map=shell.map -o ./bin/shell $^

clean:
	rm -f *.o *.elf *.bin

