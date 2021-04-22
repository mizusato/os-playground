#include "io.hpp"
#include "interrupt.hpp"

#define IDT_SIZE 256
#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

InterruptDescriptor Idt[IDT_SIZE] = {0};
InterruptTablePointer IdtPointer;
extern "C" {
    void LoadInterruptTable(InterruptTablePointer*);
}
void setupPIC();

namespace Interrupt {
    void Setup(Number n, void (*handler)(), Word selector, Byte flags) {
        Number offset = (Number) handler;
        Idt[n].Offset_0 = offset & 0xFFFF;
        Idt[n].Offset_16 = (offset >> 16) & 0xFFFF;
        Idt[n].Offset_32 = 0;
        Idt[n].Selector = selector;
        Idt[n].Flags = flags;
    }
    void Setup(Number n, void (*handler)()) {
        Setup(n, handler, 0x08, 0x8E);
    }
    void Init() {
        setupPIC();
        IdtPointer.Limit = ((sizeof(InterruptDescriptor) * IDT_SIZE) - 1);
        IdtPointer.Base = (Number) &Idt;
        LoadInterruptTable(&IdtPointer);
        SetInterruptFlag();
    }
    void UnmaskIRQ(Number irq) {
        if (irq >= 8) {
            Byte mask = InputByte(PIC_2_DATA);
            OutputByte(PIC_2_DATA, mask & (~ (1 << (irq - 8))));
            mask = InputByte(PIC_1_DATA);
            OutputByte(PIC_1_DATA, mask & (~ 0x04));
        } else {
            Byte mask = InputByte(PIC_1_DATA);
            OutputByte(PIC_1_DATA, mask & (~ (1 << irq)));
        }
    }
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
    OutputByte(PIC_1_DATA, 0x04);
    OutputByte(PIC_2_DATA, 0x02);
    /* ICW4 - environment info */
    OutputByte(PIC_1_DATA, 0x01);
    OutputByte(PIC_2_DATA, 0x01);
    /* Initialization finished */
    /* mask interrupts */
    OutputByte(PIC_1_DATA , 0xFF);
    OutputByte(PIC_2_DATA , 0xFF);
}

