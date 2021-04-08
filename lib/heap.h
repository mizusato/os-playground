#ifndef OS_HEAP_H
#define OS_HEAP_H

#include "types.h"

struct _Chunk;

typedef struct _Chunk Chunk;

struct _Chunk {
    Chunk* previous;
    Chunk* next;
    Byte   data[56];
} __attribute__((packed));

typedef struct _HeapStatus {
    Number  ChunksTotal;
    Number  ChunksAvailable;
} HeapStatus;

void HeapInit();
Chunk* HeapAllocate(Number n);
void HeapFree(Chunk* head, Number n);
HeapStatus HeapGetStatus();

#endif  // OS_HEAP_H