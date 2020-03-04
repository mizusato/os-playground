#!/bin/bash

options="-m32 -ffreestanding -fno-stack-protector"

nasm -f elf32 kernel.asm -o temp/kasm.o && \

for file in lib/*.c; do
  gcc -c "$file" -o temp/"${file#lib/}".o $options
done && \
gcc -c kernel.c -o temp/kernel.c.o $options && \

ld -m elf_i386 -T link.ld -o iso/boot/kernel.bin temp/kasm.o temp/*.c.o && \

grub-mkrescue -o bootable.iso iso/ && \

qemu-system-x86_64 -kernel iso/boot/kernel.bin
