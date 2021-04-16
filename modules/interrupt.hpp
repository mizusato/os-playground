#ifndef INTERRUPT_HPP
#define INTERRUPT_HPP

#include "types.h"


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
    void Setup(Number N, Number base, Word selector, Byte flags);
    void Init();
    void Unmask(Number which);
    void NotifyHandled();
};

extern "C" {
    void SetInterruptFlag();
    void ClearInterruptFlag();
    void LoadInterruptTable(InterruptTablePointer*);
}

#endif
