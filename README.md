# ToyOS

This program is a learning experience for 32-bit protected mode OSes. The goal is to write a minimal bootloader + kernel that run some interesting code (e.g. GameBoy emulator, basic games, etc.). The relevant information is sourced from the various odev wiki pages.

## Running

- Requires the use of a i686-elf-gcc cross compiler and the corresponding binutils (see [this link](https://wiki.osdev.org/GCC_Cross-Compiler) for help on building the toolchain). Specifically uses:
    - gcc 15.0.1 20250216
    - GNU Binutils 2.25.1
    - GNU gdb 16.2
- Requires nasm to build the initial bootloader
- Requires qemu-system-i386 to execute the code

## Debugging

To debug execute Makefile target `run_gdb` in one window and start gdb in another. Then connect to qemu in GDB via `target remote localhost:1234`. To load the necessary debugging symbols for easier debugging do `symbol-file build/kernel.elf`.
