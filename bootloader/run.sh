#!/bin/bash

OVMF_FD="/usr/share/edk2-ovmf/x64/OVMF.fd"

qemu-system-x86_64 -bios "$OVMF_FD" -drive file=fat:rw:./build/,format=raw

