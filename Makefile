.PHONY: all run clean

CFLAGS = -Wall -Wextra -ffreestanding -O2

all: boot.bin

run: boot.bin
	qemu-system-i386 -drive file=$<,format=raw

clean:
	rm -rf src/*.o *.elf *.bin

boot.bin: bootloader.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel.elf
	i686-elf-objcopy -O binary -S $< $@

kernel.elf: src/start.o src/test.o
	i686-elf-gcc -T linker.ld -ffreestanding -O2 -nostdlib $^ -lgcc -o $@

src/start.o: src/start.S
	i686-elf-gcc -c ${CFLAGS} $< -o $@

src/test.o: src/test.c
	i686-elf-gcc -c ${CFLAGS} $< -o $@

bootloader.bin: src/bootloader.S
	nasm -f bin $< -o $@
