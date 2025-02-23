.PHONY: all run clean

CFLAGS = -Wall -Wextra -g -ffreestanding -O2 -Iheaders
CC = i686-elf-gcc
OBJ_T = $(patsubst %.c,%.o,$(wildcard src/*.c))
OBJ_T += $(patsubst %.S,%.o,$(wildcard src/*.S))
OBJ = $(patsubst src/%.o,build/%.o,${OBJ_T})

all: build/boot.bin

run: build/boot.bin
	qemu-system-i386 -drive file=$<,format=raw

run_gdb: build/boot.bin
	qemu-system-i386 -S -drive file=$<,format=raw

clean:
	rm -rf build/*

build/boot.bin: build/bootloader.bin build/kernel.bin
	cat $^ > $@

build/kernel.bin: build/kernel.elf
	i686-elf-objcopy -O binary -S $< $@

build/kernel.elf: ${OBJ}
	${CC} -T linker.ld -nostdlib $^ -lgcc -o $@

build/%.o: src/%.S
	${CC} -c ${CFLAGS} $< -o $@

build/%.o: src/%.c
	${CC} -c ${CFLAGS} $< -o $@

build/bootloader.bin: boot/bootloader.S
	mkdir -p build
	nasm -f bin $< -o $@

dump: build/kernel.elf
	i686-elf-objdump -D -Mintel $^ > build/dump

debug: build/kernel.elf
	i686-elf-gdb $^
