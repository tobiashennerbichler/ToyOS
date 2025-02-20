.PHONY: all run clean

CFLAGS = -Wall -Wextra -g -ffreestanding -O2
CC = i686-elf-gcc

all: boot.bin

run: boot.bin
	qemu-system-i386 -drive file=$<,format=raw

run_gdb: boot.bin
	qemu-system-i386 -S -drive file=$<,format=raw

clean:
	rm -rf src/*.o *.elf *.bin

boot.bin: bootloader.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel.elf
	i686-elf-objcopy -O binary -S $< $@

kernel.elf: src/start.o src/test.o
	${CC} -T linker.ld -ffreestanding -O2 -nostdlib $^ -lgcc -o $@
	rm $^

src/start.o: src/start.S
	${CC} -c ${CFLAGS} $< -o $@

src/test.o: src/test.c
	${CC} -c ${CFLAGS} $< -o $@

bootloader.bin: src/bootloader.S
	nasm -f bin $< -o $@
