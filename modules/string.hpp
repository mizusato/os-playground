#ifndef STRING_HPP
#define STRING_HPP

#include "types.h"
#include "shared.hpp"
#include "list.hpp"


typedef Dword Char;

class String final {
public:
    class Builder {
    private:
        Unique<List<Char>> buf;
    public:
        Builder(): buf(Unique<List<Char>>(new List<Char>())) {};
        virtual ~Builder() {};
        void Write(String s);
        String Collect();
    };
    class Iterator {
    public:
        virtual ~Iterator() {};
        virtual bool NotEmpty() const = 0;
        virtual Char CurrentChar() const = 0;
        virtual void Shift() = 0;
    };
    class Impl {
    public:
        virtual ~Impl() {};
        virtual Unique<Iterator> Iterate() const = 0;
    };
private:
    Shared<Impl> impl;
    String(Shared<Impl> impl): impl(impl) {};
public:
    ~String() {};
    String(Char ch);
    String(const char* data);
    String(Unique<List<Char>> list);
    Unique<Iterator> Iterate() const { return impl->Iterate(); };
    static String Hex(Number n);
};

class LegacyString final: public String::Impl {
public:
    class Iterator final: public String::Iterator {
    private:
        const char* data;
    public:
        Iterator(const char* data): data(data) {};
        ~Iterator() {};
        bool NotEmpty() const override;
        Char CurrentChar() const override;
        void Shift() override;
    };
private:
    const char* const data;
public:
    LegacyString(const char* data): data(data) {};
    ~LegacyString() {}
    Unique<String::Iterator> Iterate() const override;
};

class ListString final: public String::Impl {
public:
    class Iterator final: public String::Iterator {
    private:
        Unique<List<Char>::Iterator> iterator;
    public:
        Iterator(Unique<List<Char>::Iterator> iterator): iterator(std::move(iterator)) {};
        ~Iterator() {};
        bool NotEmpty() const override;
        Char CurrentChar() const override;
        void Shift() override;
    };
private:
    Unique<List<Char>> list;
public:
    ListString(Unique<List<Char>> list): list(std::move(list)) {};
    ~ListString() {}
    Unique<String::Iterator> Iterate() const override;
};

#endif

