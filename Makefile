.PHONY: all run clean

CFLAGS = -Wall -Wextra -g -ffreestanding -O2 -Iinclude
CC = i686-elf-gcc
BOOT = kernel/boot/bootloader.S
SRC = $(wildcard kernel/core/*.c kernel/core/*.S kernel/drivers/*.c kernel/libc/*.c kernel/video/*.c)
SRC += kernel/video/vesa/vesa.c
OBJ_T1 = $(patsubst %.c,%.o,${SRC})
OBJ_T2 += $(patsubst %.S,%.o,${OBJ_T1})
OBJ = $(patsubst kernel/%.o,build/%.o,${OBJ_T2})

all: build/boot.bin

run: build/boot.bin
	qemu-system-i386 -drive file=$<,format=raw

run_gdb: build/boot.bin
	qemu-system-i386 -s -S -drive file=$<,format=raw
	
run_log: build/boot.bin
	qemu-system-i386 -d int -D log.txt -drive file=$<,format=raw

clean:
	rm -rf build/*

build/boot.bin: build/bootloader.bin build/kernel.bin
	cat $^ > $@
	truncate -s 40K $@

build/kernel.bin: build/kernel.elf
	i686-elf-objcopy -O binary -S $< $@

build/kernel.elf: ${OBJ} ${SRC} linker.ld
	${CC} -T linker.ld -nostdlib ${OBJ} -lgcc -o $@

build/%.o: kernel/%.S
	${CC} -c ${CFLAGS} $< -o $@

build/%.o: kernel/%.c
	${CC} -c ${CFLAGS} $< -o $@

build/bootloader.bin: ${BOOT}
	mkdir -p build/boot
	mkdir -p build/core
	mkdir -p build/drivers
	mkdir -p build/libc
	mkdir -p build/video/vesa
	nasm -f bin $< -o $@

dump: build/kernel.elf
	i686-elf-objdump -D -Mintel $^ > build/dump

debug: build/kernel.elf
	i686-elf-gdb $^
