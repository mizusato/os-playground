bits 64

section .text
global EntryPoint
extern Main
EntryPoint:
    cli
    o64 lgdt [GDT_POINTER]
    push rbp
    mov rbp, rsp
    mov rax, .setcs
    mov rbx, CODE_SELECTOR
    push rbx
    push rax
    o64 retf
.setcs:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ax, DATA_SELECTOR
    mov ss, ax
    call Main
    ret

global InputByte
global OutputByte
InputByte:
    mov dx, di
    in al, dx
    ret
OutputByte:
    mov dx, di
    mov ax, si
    out dx, al
    ret

%macro PUSHAQ 0
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro POPAQ 0
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
%endmacro

%macro ISR_ENTER 0
    cli
    PUSHAQ
    cld
%endmacro

%macro ISR_EXIT_PIC_1 0
    mov dx, 0x20
    mov al, 0x20
    out dx, al
    POPAQ
    sti
    iretq
%endmacro

%macro ISR_EXIT_PIC_2 0
    mov dx, 0xA0
    mov al, 0x20
    out dx, al
    mov dx, 0x20
    mov al, 0x20
    out dx, al
    POPAQ
    sti
    iretq
%endmacro

global SetInterruptFlag
global ClearInterruptFlag
global LoadInterruptTable
SetInterruptFlag:
    sti
    ret
ClearInterruptFlag:
    cli
    ret
LoadInterruptTable:
    lidt [rdi]
    ret

global TimerInterruptHandler
extern handleTimerInterrupt
TimerInterruptHandler:
    ISR_ENTER
    call handleTimerInterrupt
    ISR_EXIT_PIC_1

global KeyboardInterruptHandler
extern handleKeyboardInterrupt
KeyboardInterruptHandler:
    ISR_ENTER
    call handleKeyboardInterrupt
    ISR_EXIT_PIC_1

global MouseInterruptHandler
extern handleMouseInterrupt
MouseInterruptHandler:
    ISR_ENTER
    call handleMouseInterrupt
    ISR_EXIT_PIC_2

global PanicInterruptHandler
extern handlePanicInterrupt
PanicInterruptHandler:
    cli
    cld
    call handlePanicInterrupt
    hlt


section .data

global PrimaryFontData
PrimaryFontData:
incbin "ui/primary.font.bin"

; Macro to build a GDT descriptor entry
%define MAKE_GDT_DESC(base, limit, access, flags) \
    (((base & 0x00FFFFFF) << 16) | \
    ((base & 0xFF000000) << 32) | \
    (limit & 0x0000FFFF) | \
    ((limit & 0x000F0000) << 32) | \
    ((access & 0xFF) << 40) | \
    ((flags & 0x0F) << 52))

GDT_0_NULL:
    dq MAKE_GDT_DESC(0, 0, 0, 0); null descriptor
GDT_1_CODE:
    dq MAKE_GDT_DESC(0, 0x00ffffff, 10011010b, 1010b)
    ; 64bit code, 4kb gran, limit 0xffffffff bytes, base=0
GDT_2_DATA:
    dq MAKE_GDT_DESC(0, 0x00ffffff, 10010010b, 1010b)
    ; 64-bit data, 4kb gran, limit 0xffffffff bytes, base=0
GDT_END:

GDT_POINTER:
    dw GDT_END - GDT_0_NULL - 1  ; limit (Size of GDT - 1)
    dq GDT_0_NULL  ; base of GDT

CODE_SELECTOR equ GDT_1_CODE - GDT_0_NULL
DATA_SELECTOR equ GDT_2_DATA - GDT_0_NULL

