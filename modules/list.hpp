#include "heap.hpp"
#include "unique.hpp"

template <typename T>
class List {
private:
    Number elementPerChunk;
    Number numberOfChunks;
    Chunk* head;
    struct ChunkData {
        Number elementAmount;
        T elements[];
    };
    class Iterator {
    private:
        const List* list = nullptr;
        const Chunk* current = nullptr;
        Number innerIndex = 0;
        Iterator() {}
        friend class List;
    public:
        ~Iterator() {}
        bool HasValue() const {
            return (current != nullptr);
        }
        T Value() const {
            // TODO: check if HasValue(), if not, panic
            const ChunkData* data = reinterpret_cast<const ChunkData*>(&(current->data));
            return data->elements[innerIndex];
        }
        void Proceed() {
            // TODO: check if HasValue(), if not, panic
            const ChunkData* data = reinterpret_cast<const ChunkData*>(&(current->data));
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
public:
    List() {
        static_assert(sizeof(T) < CHUNK_DATA_SIZE, "List: element too big");
        if (2*sizeof(T) < CHUNK_DATA_SIZE) {
            elementPerChunk = (CHUNK_DATA_SIZE / sizeof(T));
        } else {
            elementPerChunk = 1;
        }
        numberOfChunks = 0;
        head = nullptr;
    };
    virtual ~List() {
        if (head != nullptr) {
            Chunk* current = head;
            for (Number i = 0; i < numberOfChunks; i += 1) {
                ChunkData* data = reinterpret_cast<ChunkData*>(&(current->data));
                for (Number j = 0; j < data->elementAmount; j += 1) {
                    data->elements[j].~T();
                }
                current = current->next;
            }
            Heap::Free(head, numberOfChunks);
        }
    };
    bool NotEmpty() const {
        return (head != nullptr);
    }
    Unique<Iterator> Iterate() const {
        Iterator *iterator = new Iterator;
        iterator->list = this;
        iterator->current = head;
        iterator->innerIndex = 0;
        return Unique<Iterator>(iterator);
    }
    void Append(T element) {
        if (head == nullptr) {
            head = Heap::Allocate(1);
            ChunkData* data = reinterpret_cast<ChunkData*>(&(head->data));
            data->elements[0] = element;
            data->elementAmount = 1;
            numberOfChunks = 1;
        } else {
            Chunk* tail = head->previous;
            ChunkData* data = reinterpret_cast<ChunkData*>(&(tail->data));
            if (data->elementAmount < elementPerChunk) {
                Number index = data->elementAmount;
                data->elements[index] = element;
                data->elementAmount += 1;
            } else {
                Chunk* new_tail = Heap::Allocate(1);
                ChunkData* data = reinterpret_cast<ChunkData*>(&(new_tail->data));
                data->elements[0] = element;
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

