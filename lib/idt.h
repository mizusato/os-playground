#ifndef OS_IDT_H
#define OS_IDT_H

#include "types.h"

typedef struct {
    Word  OffsetLowerBits;
    Word  Selector;
    Byte  Zero;
    Byte  Flags;
    Word  OffsetHigherBits;
} __attribute__((packed)) IdtEntry;

typedef struct {
    Word  Limit;
    Number  Base;
} __attribute__((packed)) IdtPointer;

Void IdtSetEntry(Number N, Number base, Word selector, Byte flags);
Void IdtInit();

Void IdtPointerInit();
Void IdtLoad(IdtPointer* ptr_ptr);
Void InitPIC();

#endif  // OS_IDT_H

