.PHONY: all run clean

CFLAGS = -Wall -Wextra -g -ffreestanding -O2 -Iheaders
CC = i686-elf-gcc
OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
OBJ += src/start.o src/interrupt_wrapper.o

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

kernel.elf: ${OBJ} linker.ld
	${CC} -T linker.ld -nostdlib $^ -lgcc -o $@

%.o: %.S
	${CC} -c ${CFLAGS} $< -o $@

%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

bootloader.bin: src/bootloader.S
	nasm -f bin $< -o $@
