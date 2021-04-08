#include "idt.h"
#include "io.h"


#define IDT_SIZE 256
#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

static IdtEntry idt_table[IDT_SIZE];
static IdtPointer idt_pointer;

Void IdtSetEntry(Size N, Size base, Word selector, Byte flags) {
    idt_table[N].OffsetLowerBits = base & 0xFFFF;
    idt_table[N].OffsetHigherBits = (base >> 16) & 0xFFFF;
    idt_table[N].Selector = selector;
    idt_table[N].Flags = flags;
    idt_table[N].Zero = 0;
}

Void IdtInit() {
    InitPIC();
    IdtPointerInit();
    IdtLoad(&idt_pointer);
}

Void IdtPointerInit() {
    idt_pointer.Limit = ((sizeof(IdtEntry) * IDT_SIZE) - 1);
    idt_pointer.Base = (Size) &idt_table;
}

Void IdtLoad(IdtPointer* ptr_ptr) {
    __asm__ __volatile__ ("lidt %0" :: "m"(*ptr_ptr));
    __asm__ __volatile__ ("sti");
}

Void InitPIC() {
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
    OutputByte(0x21 , 0xff);
    OutputByte(0xA1 , 0xff);
}

