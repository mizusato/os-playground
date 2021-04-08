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
    Size  Base;
} __attribute__((packed)) IdtPointer;

Void IdtSetEntry(Size N, Size base, Word selector, Byte flags);
Void IdtInit();

Void IdtPointerInit();
Void IdtLoad(IdtPointer* ptr_ptr);
Void InitPIC();

#endif  // OS_IDT_H

