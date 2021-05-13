#ifndef UNIQUE_HPP
#define UNIQUE_HPP

#include <utility>


template <typename T>
class Unique {
private:
    T* ptr;
public:
    explicit Unique(T* ptr): ptr(ptr) {};
    virtual ~Unique() {
        if (ptr != nullptr) {
            delete ptr;
        }
    };
    Unique(Unique&& another):
        ptr(another.ptr) {
        another.ptr = nullptr;
    };
    Unique& operator = (Unique&& another) {
        if (ptr != nullptr) {
            delete ptr;
        }
        ptr = another.ptr;
        another.ptr = nullptr;
        return *this;
    };
    Unique(const Unique&) = delete;
    Unique& operator = (const Unique& another) = delete;
    T& operator * () const { return *ptr; };
    T* operator -> () const { return ptr; };
    T* get() const { return ptr; };
};

#endif

