#include "panic.hpp"
#include "heap.hpp"


#define RESERVED_CHUNKS 16
#define HEAP_STATIC_AREA_SIZE (16 * 1024 * 1024)
#define HEAP_MEMORY_INFO_LOG_MAX 128
#define MINIMAL_ACCEPTED_MEMORY_AREA_SIZE (100 * 1024)

Chunk sentinel_chunk;
Chunk* free_list;
Byte* static_ptr;
HeapStatus status;
HeapMemoryInfo info[HEAP_MEMORY_INFO_LOG_MAX];
Number info_length = 0;

void* operator new (Number size) {
    if (size > CHUNK_DATA_SIZE) {
        panic("operator new: object too big");
    }
    return (void*) Heap::Allocate(1);
}

void operator delete (void* ptr) {
    Heap::Free((Chunk*) ptr, 1);
}

namespace Heap {
    void ConsumeMemory(void* start, Number size) {
        Number n = (size / sizeof(Chunk));
        if (n == 0) {
            return;
        }
        if (n < MINIMAL_ACCEPTED_MEMORY_AREA_SIZE) {
            return;
        }
        Chunk* first = reinterpret_cast<Chunk*>(start);
        Chunk temp;
        Chunk* prev = &temp;
        for (Number i = 0; i < n; i += 1) {
            Chunk* current = (first + i);
            current->previous = prev;
            prev->next = current;
            prev = current;
        }
        Chunk* last = prev;
        last->next = free_list->next;
        free_list->next->previous = last;
        first->previous = free_list;
        free_list->next = first;
        status.ChunksAvailable += n;
        status.ChunksTotal += n;
        if (info_length < HEAP_MEMORY_INFO_LOG_MAX) {
            Number index = info_length;
            info[index] = { .start = (Number) start, .size = size };
            info_length += 1;
        }
    }
    void Init(MemoryInfo* memInfo) {
        sentinel_chunk.next = &sentinel_chunk;
        sentinel_chunk.previous = &sentinel_chunk;
        free_list = &sentinel_chunk;
        static_ptr = nullptr;
        status.ChunksTotal = 0;
        status.ChunksAvailable = 0;
        status.StaticPosition = 0;
        status.StaticSize = 0;
        status.StaticAvailable = 0;
        Number totalConsumed = 0;
        Number base = (Number) memInfo->mapBuffer;
        for (Number ptr = base; (ptr - base) < memInfo->mapSize; ptr += memInfo->descSize) {
            MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(ptr);
            if (desc->kind.value == MK_ConventionalMemory) {
                Number size = (desc->numberOfPages * 4096);
                Number start = desc->physicalStart;
                Number end = (start + size);
                constexpr Number kernel_end = KERNEL_POSITION + KERNEL_RESERVED_AMOUNT;
                if (start < kernel_end) {
                    if (kernel_end < end) {
                        start = kernel_end;
                        size = (end - start);
                    } else {
                        continue;
                    }
                }
                Number static_size = HEAP_STATIC_AREA_SIZE;
                if (static_ptr == nullptr && size > static_size) {
                    static_ptr = reinterpret_cast<Byte*>(start);
                    status.StaticPosition = start;
                    status.StaticSize = static_size;
                    status.StaticAvailable = static_size;
                    size -= static_size;
                    start += static_size;
                }
                ConsumeMemory(reinterpret_cast<void*>(start), size);
                totalConsumed += 1;
            }
        }
        if (totalConsumed == 0) {
            ConsumeMemory((void*) FALLBACK_HEAP_START, FALLBACK_HEAP_SIZE);
        }
    }
    Chunk* Allocate(Number n) {
        if (n == 0) {
            panic("Heap::Allocate(): invalid argument");
        }
        if (status.ChunksAvailable < (n + RESERVED_CHUNKS)) {
            panic("Heap::Allocate(): out of memory");
        }
        Chunk* head = free_list->next;
        Chunk* current = head;
        for (Number i = 1; i <= (n - 1); i += 1) {
            current = current->next;
        }
        Chunk* tail = current;
        free_list->next = tail->next;
        tail->next->previous = free_list;
        head->previous = tail;
        tail->next = head;
        status.ChunksAvailable -= n;
        return head;
    } 
    void Free(Chunk* head, Number n) {
        if (n == 0) {
            panic("Heap::Free(): invalid argument");
        }
        Chunk* current = head;
        for (Number i = 1; i <= (n - 1); i += 1) {
            current = current->next;
        }
        Chunk* tail = current;
        tail->next = free_list->next;
        free_list->next->previous = tail;
        head->previous = free_list;
        free_list->next = head;
        status.ChunksAvailable += n;
    }
    void* RequestStatic(Number size) {
        if (size > status.StaticAvailable) {
            panic("Heap::AllocateStatic(): static memory not enough");
        }
        void* allocated = static_ptr;
        static_ptr += size;
        status.StaticAvailable -= size;
        return allocated;
    }
    const HeapMemoryInfo* GetInfo(Number* length) {
        *length = info_length;
        return info;
    }
    HeapStatus GetStatus() {
        return status;
    }
}

