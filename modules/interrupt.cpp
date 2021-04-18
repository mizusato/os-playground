#include "io.hpp"
#include "interrupt.hpp"

#define IDT_SIZE 256
#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

void setupPIC();
Byte getInterruptMask();
void setInterruptMask(Byte mask);

InterruptDescriptor Idt[IDT_SIZE] = {0};
InterruptTablePointer IdtPointer;
extern "C" {
    void LoadInterruptTable(InterruptTablePointer*);
}

namespace Interrupt {
    void Setup(Number N, Number base, Word selector, Byte flags) {
        Idt[N].Offset_0 = base & 0xFFFF;
        Idt[N].Offset_16 = (base >> 16) & 0xFFFF;
        Idt[N].Offset_32 = 0;
        Idt[N].Selector = selector;
        Idt[N].Flags = flags;
    }
    void Init() {
        setupPIC();
        IdtPointer.Limit = ((sizeof(InterruptDescriptor) * IDT_SIZE) - 1);
        IdtPointer.Base = (Number) &Idt;
        LoadInterruptTable(&IdtPointer);
        SetInterruptFlag();
    }
    void Unmask(Number which) {
        Byte mask = getInterruptMask();
        setInterruptMask(mask & (~(1 << which)));
    }
}

Byte getInterruptMask() {
    return InputByte(PIC_1_DATA);
}

void setInterruptMask(Byte mask) {
    OutputByte(PIC_1_DATA, mask);
}

void setupPIC() {
    /* ICW1 - begin initialization */
    OutputByte(PIC_1_CTRL, 0x11);
    OutputByte(PIC_2_CTRL, 0x11);
    /* ICW2 - remap offset address of idt_table */
    /*
    * In x86 protected mode, we have to remap the PICs beyond 0x20 because
    * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
    */
    OutputByte(PIC_1_DATA, 0x20);
    OutputByte(PIC_2_DATA, 0x28);
    /* ICW3 - setup cascading */
    OutputByte(PIC_1_DATA, 0x00);
    OutputByte(PIC_2_DATA, 0x00);
    /* ICW4 - environment info */
    OutputByte(PIC_1_DATA, 0x01);
    OutputByte(PIC_2_DATA, 0x01);
    /* Initialization finished */
    /* mask interrupts */
    OutputByte(PIC_1_DATA , 0xFF);
    OutputByte(PIC_2_DATA , 0xFF);
}

