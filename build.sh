#!/bin/bash -x

IMG_SIZE=1 # * 4M
CXX_OPTIONS="-Wall -g -std=c++11 -O0 -fno-pie "\
" -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti"\
""

OBJECTS_DIR="build/objects"
ISO_DIR="build/iso"
BOOTLOADER_DIR="bootloader"
MOUNTPOINT="build/mountpoint"
OUTPUT_IMG="build/iso.img"
OUTPUT_ISO="build/bootable.iso"

PROJECT_DIR="$PWD"

cd "$BOOTLOADER_DIR" && ./build.sh && cd "$PROJECT_DIR" && \

cp "$BOOTLOADER_DIR/build/EFI/BOOT/BOOTX64.EFI" "$ISO_DIR/EFI/BOOT/" && \

nasm -f elf64 "kernel.asm" -o "$OBJECTS_DIR/kernel.asm.o" && \

for file in modules/*.cpp; do
  g++ -c "$file" -o "$OBJECTS_DIR/${file#modules/}".o $CXX_OPTIONS
done && \
g++ -c "kernel.cpp" -o "$OBJECTS_DIR/kernel.cpp.o" $CXX_OPTIONS && \

ld -m elf_x86_64 -T link.ld -o "$OBJECTS_DIR/kernel.elf" \
  "$OBJECTS_DIR/kernel.asm.o" "$OBJECTS_DIR/"*.cpp.o && \

objcopy -O binary "$OBJECTS_DIR/kernel.elf" "$ISO_DIR/kernel.bin" && \

> "$OUTPUT_IMG" && \

dd if=/dev/zero of="$OUTPUT_IMG" bs=4M count="$IMG_SIZE" && \

mkfs.vfat "$OUTPUT_IMG" && \

mcopy -s -i "$OUTPUT_IMG" "$ISO_DIR/"* "::" && \

mdel -i "$OUTPUT_IMG" "::/EFI/BOOT/.gitkeep" && \

xorriso -as mkisofs -o "$OUTPUT_ISO" -m ".gitkeep" -iso-level 3 -V UEFI "$ISO_DIR" \
  "$OUTPUT_IMG" -e "/${OUTPUT_IMG#build/}" -no-emul-boot

