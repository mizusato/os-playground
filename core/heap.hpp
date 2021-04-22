#ifndef HEAP_HPP
#define HEAP_HPP

#include "types.h"
#include "../boot.h"


#define CHUNK_DATA_SIZE 112

struct Chunk {
    Byte   data[CHUNK_DATA_SIZE];
    Chunk* previous;
    Chunk* next;
} __attribute__((packed));

struct HeapStatus {
    Number  ChunksTotal;
    Number  ChunksAvailable;
    Number  StaticPosition;
    Number  StaticSize;
    Number  StaticAvailable;
};

struct HeapMemoryInfo {
    Number  start;
    Number  size;
};

void* operator new (Number size);
void  operator delete (void* ptr);

inline void* operator new (Number size, void* place) { return place; };

namespace Heap {
    void Init(MemoryInfo* memInfo);
    Chunk* Allocate(Number n);
    void Free(Chunk* head, Number n);
    void* RequestStatic(Number size);
    const HeapMemoryInfo* GetInfo(Number* length);
    HeapStatus GetStatus();
}

#endif

