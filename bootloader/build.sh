#!/bin/bash

GNU_EFI_INCLUDE="/usr/include/efi"

GNU_EFI_LIB="/usr/lib"

gcc -I"$GNU_EFI_INCLUDE"/ -fpic \
    -ffreestanding -fno-stack-protector -fno-stack-check \
    -fshort-wchar -mno-red-zone -maccumulate-outgoing-args \
    -c bootloader.c -o build/bootloader.o && \

ld -shared -Bsymbolic -L"$GNU_EFI_LIB"/ -T"$GNU_EFI_LIB"/elf_x86_64_efi.lds \
    "$GNU_EFI_LIB"/crt0-efi-x86_64.o build/bootloader.o -o build/bootloader.so \
    -lgnuefi -lefi && \

objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
    -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
    --target efi-app-x86_64 --subsystem=10 \
    build/bootloader.so build/EFI/BOOT/BOOTX64.EFI

