.PHONY: all run clean

CFLAGS = -ffreestanding

all: kernel.bin

run: kernel.bin
	qemu-system-i386 -drive file=kernel.bin,format=raw

clean:
	rm -rf src/*.o test test.bin

kernel.bin: bootloader.bin test.bin
	cat $^ > kernel.bin

test.bin: src/start.o src/test.o
	i686-elf-ld -T linker.ld $^ -o test
	i686-elf-objcopy -O binary -S test test.bin

src/start.o: src/start.S
	i686-elf-gcc -c ${CFLAGS} $< -o $@

src/test.o: src/test.c
	i686-elf-gcc -c ${CFLAGS} $< -o $@

bootloader.bin: src/bootloader.S
	nasm -f bin $< -o $@
