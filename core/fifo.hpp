#ifndef FIFO_HPP
#define FIFO_HPP

#include "heap.hpp"
#include "panic.hpp"


#define FIFO_LENGTH 64

template <typename T>
class FIFO final {
private:
    Number head;
    Number tail;
    T* buffer;
public:
    FIFO() {
        head = 0;
        tail = 0;
        buffer = reinterpret_cast<T*>(Heap::RequestStatic(FIFO_LENGTH * sizeof(T)));
    };
    ~FIFO() {
        panic("a FIFO is a static object so that cannot be deleted");
    };
    bool Read(T* value) {
        Number current = head;
        if (current == tail) {
            return false;
        }
        *value = buffer[current];
        head = ((current + 1) % FIFO_LENGTH);
        return true;
    }
    void Write(const T& value) {
        Number next_tail = ((tail + 1) % FIFO_LENGTH);
        if (next_tail == head) {
            return;
        }
        buffer[tail] = value;
        tail = next_tail;
    }
};

#endif

