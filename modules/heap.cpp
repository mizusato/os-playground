#include "../boot.h"
#include "panic.hpp"
#include "heap.hpp"


struct HeapInfo {
    Number size;
};
HeapInfo heap_info = { HEAP_SIZE };
Chunk* heap_space = (Chunk*) HEAP_START;

Chunk* free_list;
HeapStatus status;

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
    void Init() {
        Number n = (heap_info.size / sizeof(Chunk));
        status.ChunksAvailable = n;
        status.ChunksTotal = n;
        Chunk temp;
        temp.previous = nullptr;
        temp.next = nullptr;
        Chunk* prev = &temp;
        for (Number i = 0; i < n; i += 1) {
            Chunk* current = &heap_space[i];
            current->previous = prev;
            prev->next = current;
            prev = current;
        }
        Chunk* last = prev;
        free_list = temp.next;
        free_list->previous = last;
    }
    Chunk* Allocate(Number n) {
        if (n == 0) {
            panic("Heap::Allocate(): invalid argument");
        }
        if (status.ChunksAvailable < n) {
            panic("Heap::Allocate(): out of memory");
        }
        Chunk* head = free_list;
        Chunk* current = head;
        for (Number i = 1; i <= (n - 1); i += 1) {
            current = current->next;
        }
        Chunk* tail = current;
        free_list = tail->next;
        tail->next->previous = head->previous;
        head->previous->next = tail->next;
        free_list = tail->next;
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
        Chunk* list_prev = free_list->previous;
        list_prev->next = head;
        head->previous = list_prev;
        free_list->previous = tail;
        tail->next = free_list;
        status.ChunksAvailable += n;
    }
    HeapStatus GetStatus() {
        return status;
    }
}

