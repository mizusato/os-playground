#ifndef LIST_HPP
#define LIST_HPP

#include "heap.hpp"
#include "unique.hpp"
#include "panic.hpp"


template <typename T>
class List {
public:
    class Iterator {
    private:
        List* list = nullptr;
        Chunk* current = nullptr;
        Number innerIndex = 0;
        Iterator() {}
        friend class List;
    public:
        ~Iterator() {}
        bool HasCurrent() const {
            return (current != nullptr);
        }
        T& Current() {
            if (!(HasCurrent())) { panic("invalid iterator usage"); }
            ChunkData* data = reinterpret_cast<ChunkData*>(&(current->data));
            return data->Get(innerIndex);
        }
        void Proceed() {
            if (!(HasCurrent())) { panic("invalid iterator usage"); }
            ChunkData* data = reinterpret_cast<ChunkData*>(&(current->data));
            if ((innerIndex + 1) < data->elementAmount) {
                innerIndex += 1;
            } else {
                Chunk* tail = list->head->previous;
                if (current == tail) {
                    current = nullptr;
                    innerIndex = 0;
                } else {
                    current = current->next;
                    innerIndex = 0;
                }
            }
        }
    };
private:
    Number elementPerChunk;
    Number numberOfChunks;
    Number length;
    Chunk* head;
    struct ChunkData {
    public:
        T& Get(Number index) {
            return elements[index];
        }
        void Set(Number index, T value) {
            if (index < elementAmount) {
                elements[index] = value;
            } else {
                new(&elements[index]) T(value);
            }
        }
        void Unset(Number index) {
            elements[index].~T();
        }
        Number elementAmount;
    private:
        T elements[];
    };
public:
    List() {
        static_assert(sizeof(ChunkData) < CHUNK_DATA_SIZE, "List: invalid ChunkData definition");
        constexpr Number sizeOfElements = (CHUNK_DATA_SIZE - sizeof(ChunkData));
        static_assert(sizeof(T) <= sizeOfElements, "List: element too big");
        if (2*sizeof(T) <= sizeOfElements) {
            elementPerChunk = (sizeOfElements / sizeof(T));
        } else {
            elementPerChunk = 1;
        }
        numberOfChunks = 0;
        length = 0;
        head = nullptr;
    }
    List(T one): List() {
        Append(one);
    }
    virtual ~List() {
        if (head != nullptr) {
            Chunk* current = head;
            for (Number i = 0; i < numberOfChunks; i += 1) {
                ChunkData* data = reinterpret_cast<ChunkData*>(&(current->data));
                for (Number j = 0; j < data->elementAmount; j += 1) {
                    data->Unset(j);
                }
                current = current->next;
            }
            Heap::Free(head, numberOfChunks);
        }
    }
    bool Empty() const {
        return (head == nullptr);
    }
    bool NotEmpty() const {
        return (head != nullptr);
    }
    Number Length() const {
        return length;
    }
    Unique<Iterator> Iterate() {
        Iterator *iterator = new Iterator;
        iterator->list = this;
        iterator->current = head;
        iterator->innerIndex = 0;
        return Unique<Iterator>(iterator);
    }
    T Shift() {
        if (head == nullptr) {
            panic("List: Shift() on an empty list");
        }
        ChunkData* data = reinterpret_cast<ChunkData*>(&(head->data));
        if (data->elementAmount == 1) {
            T first = data->Get(0);
            data->Unset(0);
            Chunk* next = head->next;
            Heap::Free(head, 1);
            head = next;
            return first;
        } else {
            if (data->elementAmount == 0) {
                panic("List: something went wrong");
            }
            Number n = data->elementAmount;
            T first = data->Get(0);
            for (Number i = 0; i < (n - 1); i += 1) {
                data->Set(i, data->Get(i + 1));
            }
            data->Unset(n - 1);
            data->elementAmount -= 1;
            return first;
        }
    };
    void Prepend(T element) {
        length += 1;
        if (head == nullptr) {
            head = Heap::Allocate(1);
            ChunkData* data = reinterpret_cast<ChunkData*>(&(head->data));
            data->elementAmount = 0;
            data->Set(0, element);
            data->elementAmount = 1;
            numberOfChunks = 1;
        } else {
            ChunkData* data = reinterpret_cast<ChunkData*>(&(head->data));
            if ((data->elementAmount + 1) <= elementPerChunk) {
                Number n = data->elementAmount;
                for (Number i = n; i >= 1; i -= 1) {
                    data->Set(i, data->Get(i - 1));
                }
                data->Set(0, element);
                data->elementAmount += 1;
            } else {
                Chunk* new_head = Heap::Allocate(1);
                ChunkData* data = reinterpret_cast<ChunkData*>(&(new_head->data));
                data->elementAmount = 0;
                data->Set(0, element);
                data->elementAmount = 1;
                Chunk* tail = head->previous;
                new_head->previous = tail;
                new_head->next = head;
                tail->next = new_head;
                head->previous = new_head;
                head = new_head;
                numberOfChunks += 1;
            }
        }
    }
    void Append(T element) {
        length += 1;
        if (head == nullptr) {
            head = Heap::Allocate(1);
            ChunkData* data = reinterpret_cast<ChunkData*>(&(head->data));
            data->elementAmount = 0;
            data->Set(0, element);
            data->elementAmount = 1;
            numberOfChunks = 1;
        } else {
            Chunk* tail = head->previous;
            ChunkData* data = reinterpret_cast<ChunkData*>(&(tail->data));
            if ((data->elementAmount + 1) <= elementPerChunk) {
                Number index = data->elementAmount;
                data->Set(index, element);
                data->elementAmount += 1;
            } else {
                Chunk* new_tail = Heap::Allocate(1);
                ChunkData* data = reinterpret_cast<ChunkData*>(&(new_tail->data));
                data->elementAmount = 0;
                data->Set(0, element);
                data->elementAmount = 1;
                Chunk* tail = head->previous;
                new_tail->next = head;
                new_tail->previous = tail;
                head->previous = new_tail;
                tail->next = new_tail;
                numberOfChunks += 1;
            }
        }
    }
    void AppendAll(Unique<List<T>> another_) {
        List* another = another_.get();
        length += another->length;
        if (head == nullptr) {
            *this = *another;
        } else {
            Chunk* tail = head->previous;
            Chunk* another_head = another->head;
            Chunk* another_tail = another_head->previous;
            another_tail->next = head;
            another_head->previous = tail;
            head->previous = another_tail;
            tail->next = another_head;
            numberOfChunks += another->numberOfChunks;
        }
        another->head = nullptr;
        another->numberOfChunks = 0;
    }
};

#endif

