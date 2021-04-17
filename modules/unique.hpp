#include <utility>

template <typename T>
class Unique {
private:
    T* ptr;
public:
    Unique(T* ptr): ptr(ptr) {};
    virtual ~Unique() { delete ptr; };
    Unique(const Unique&) = delete;
    Unique(Unique&& another): ptr(another.ptr) {};
    Unique& operator = (const Unique& another) = delete;
    Unique&& operator = (Unique&& another) {
        delete ptr;
        ptr = another.ptr;
    };
    T& operator * () const { return *ptr; };
    T* operator -> () const { return ptr; };
    T* get() const { return ptr; };
};

