#ifndef HEAP_HPP
#define HEAP_HPP

#include "types.h"


#define CHUNK_DATA_SIZE 112

struct Chunk {
    Byte   data[CHUNK_DATA_SIZE];
    Chunk* previous;
    Chunk* next;
} __attribute__((packed));

struct HeapStatus {
    Number  ChunksTotal;
    Number  ChunksAvailable;
};

void HeapInit();
Chunk* HeapAllocate(Number n);
void HeapFree(Chunk* head, Number n);
HeapStatus HeapGetStatus();

void* operator new (Number size);
void  operator delete (void* ptr);

#endif

