#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP

#include "types.h"

#define IRQ(n) (0x20 + n)

struct InterruptDescriptor {
    Word   Offset_0;
    Word   Selector;
    Byte   _;
    Byte   Flags;
    Word   Offset_16;
    Dword  Offset_32;
    Dword  __;
} __attribute__((packed));

struct InterruptTablePointer {
    Word    Limit;
    Number  Base;
} __attribute__((packed));

namespace Interrupt {
    void Setup(Number n, void (*handler)(), Word selector, Byte flags);
    void Setup(Number n, void (*handler)());
    void Init();
    void UnmaskIRQ(Number irq);
};

extern "C" {
    void SetInterruptFlag();
    void ClearInterruptFlag();
}

#endif
