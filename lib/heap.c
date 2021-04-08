#include "heap.h"
#include "vga.h"

typedef struct _HeapInfo {
    Size size;
} __attribute__((packed)) HeapInfo;

extern HeapInfo heap_info;
extern Chunk heap_space[];
Chunk* free_list;

void HeapInit() {
    Size n = (heap_info.size / sizeof(Chunk));
    Size i;
    Chunk temp = { 0, 0 };
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

Chunk* HeapAllocate(Size n) {
    if (n == 0) {
        return 0;
    }
    Chunk* head = free_list;
    Size i;
    Chunk* this = head;
    for (i = 1; i <= (n - 1); i += 1) {
        this = this->next;
    }
    Chunk* tail = this;
    tail->next->previous = head->previous;
    head->previous->next = tail->next;
    head->previous = 0;
    tail->next = 0;
    return head;
} 

void HeapFree(Chunk* head, Size n) {
    if (n == 0) {
        return;
    }
    Chunk* this = head;
    Size i;
    for (i = 1; i <= (n - 1); i += 1) {
        this = this->next;
    }
    Chunk* tail = this;
    Chunk* list_next = free_list->next;
    free_list->next = head;
    head->previous = free_list;
    list_next->previous = tail;
    tail->next = list_next;
}

