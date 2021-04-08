#include "heap.h"

typedef struct _HeapInfo {
    Number size;
} __attribute__((packed)) HeapInfo;

extern HeapInfo heap_info;
extern Chunk heap_space[];
Chunk* free_list;
HeapStatus status;

Void HeapInit() {
    Number n = (heap_info.size / sizeof(Chunk));
    status.ChunksAvailable = n;
    status.ChunksTotal = n;
    Number i;
    Chunk temp = { Null, Null };
    Chunk* prev = &temp;
    for (i = 0; i < n; i += 1) {
        Chunk* this = &heap_space[i];
        this->previous = prev;
        prev->next = this;
        prev = this;
    }
    Chunk* last = prev;
    free_list = temp.next;
    free_list->previous = last;
}

Chunk* HeapAllocate(Number n) {
    if (n == 0) {
        return Null;
    }
    Chunk* head = free_list;
    Number i;
    Chunk* this = head;
    for (i = 1; i <= (n - 1); i += 1) {
        this = this->next;
    }
    Chunk* tail = this;
    tail->next->previous = head->previous;
    head->previous->next = tail->next;
    head->previous = Null;
    tail->next = Null;
    status.ChunksAvailable -= n;
    return head;
} 

Void HeapFree(Chunk* head, Number n) {
    if (n == 0) {
        return;
    }
    Chunk* this = head;
    Number i;
    for (i = 1; i <= (n - 1); i += 1) {
        this = this->next;
    }
    Chunk* tail = this;
    Chunk* list_next = free_list->next;
    free_list->next = head;
    head->previous = free_list;
    list_next->previous = tail;
    tail->next = list_next;
    status.ChunksAvailable += n;
}

HeapStatus HeapGetStatus() {
    return status;
}

