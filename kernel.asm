bits 64

section .text
global EntryPoint
extern Main
EntryPoint:
    call Main
    ret
