OUTPUT_FORMAT(elf64-x86-64)
ENTRY(EntryPoint)
SECTIONS {
    . = 0x100000;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}
