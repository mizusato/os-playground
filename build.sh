#!/bin/bash

nasm -f elf32 kernel.asm -o temp/kasm.o && \
gcc -m32 -c kernel.c -o temp/kc.o -fno-stack-protector && \
ld -m elf_i386 -T link.ld -o iso/boot/kernel.bin temp/kasm.o temp/kc.o && \
grub-mkrescue -o bootable.iso iso/ && \
qemu-system-x86_64 -kernel iso/boot/kernel.bin
