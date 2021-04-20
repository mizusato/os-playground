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
        virtual bool HasCurrent() const = 0;
        virtual Char Current() const = 0;
        virtual void Proceed() = 0;
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
    Unique<Iterator> Iterate() const { return impl->Iterate(); };
    String(const char* data);
    String(Unique<List<Char>> list);
    static String Chr(Char ch);
    explicit String(Number n);
    static String Hex(Number n);
    static String Oct(Number n);
    static String Bin(Number n);
    static String ReadableSize(Number n);
    static String Join(const List<String>& list, String sep);
};

class LegacyString final: public String::Impl {
public:
    class Iterator final: public String::Iterator {
    private:
        const char* data;
    public:
        Iterator(const char* data): data(data) {};
        ~Iterator() {};
        bool HasCurrent() const override;
        Char Current() const override;
        void Proceed() override;
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
        bool HasCurrent() const override;
        Char Current() const override;
        void Proceed() override;
    };
private:
    Unique<List<Char>> list;
public:
    ListString(Unique<List<Char>> list): list(std::move(list)) {};
    ~ListString() {}
    Unique<String::Iterator> Iterate() const override;
};

#endif

