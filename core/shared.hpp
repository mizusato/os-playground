#ifndef SHARED_HPP
#define SHARED_HPP

#include "types.h"
#include "heap.hpp"


template <typename T>
class Shared {
private:
    struct ControlBlock {
        T* raw;
        Number ref;
    };
    ControlBlock* blk;
public:
    Shared(T* raw) {
        blk = new ControlBlock;
        blk->raw = raw;
        blk->ref = 1;
    }
    Shared(const Shared& another): blk(another.blk) {
        blk->ref += 1;
    }
    Shared& operator = (const Shared& another) {
        blk->ref -= 1;
        if (blk->ref == 0) {
            delete blk->raw;
            delete blk;
            blk = nullptr;
        }
        blk = another.blk;
        blk->ref += 1;
        return *this;
    }
    virtual ~Shared() {
        blk->ref -= 1;
        if (blk->ref == 0) {
            delete blk->raw;
            delete blk;
            blk = nullptr;
        }
    }
    T& operator * () const {
        return *(blk->raw);
    }
    T* operator -> () const {
        return blk->raw;
    }
};

#endif

