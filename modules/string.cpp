#include "string.hpp"


String::String(Char ch):
    String(Unique<List<Char>>(new List<Char>(ch))) {};

String::String(const char* data):
    impl(Shared<String::Impl>(new LegacyString(data))) {};

String::String(Unique<List<Char>> list):
    impl(Shared<String::Impl>(new ListString(std::move(list)))) {};

String String::Hex(Number n) {
    static const char* mapping = "0123456789abcdef";
    char reversed[24];
    Number len = 0;
    while(n > 0) {
        char digit = mapping[n & 0xF];
        n = (n >> 4);
        reversed[len] = digit;
        len += 1;
    }
    if (len == 0) {
        return "0";
    } else {
        auto buf = Unique<List<Char>>(new List<Char>());
        for (Number i = 0; i < len; i += 1) {
            Number j = (len - 1 - i);
            buf->Append(reversed[j]);
        }
        return std::move(buf);
    }
}

void String::Builder::Write(String s) {
    for (auto it = s.Iterate(); it->NotEmpty(); it->Shift()) {
        buf->Append(it->CurrentChar());
    }
}
String String::Builder::Collect() {
    return std::move(buf);
}

Unique<String::Iterator> LegacyString::Iterate() const {
    return Unique<String::Iterator>(new LegacyString::Iterator(data));
}
bool LegacyString::Iterator::NotEmpty() const {
    return (data != nullptr && *data != 0);
}
Char LegacyString::Iterator::CurrentChar() const {
    // TODO: check, panic if empty
    return static_cast<Char>(*data);
}
void LegacyString::Iterator::Shift() {
    data++;
}

Unique<String::Iterator> ListString::Iterate() const {
    return Unique<String::Iterator>(new ListString::Iterator(list->Iterate()));
}
bool ListString::Iterator::NotEmpty() const {
    return iterator->HasValue();
}
Char ListString::Iterator::CurrentChar() const {
    return iterator->Value();
}
void ListString::Iterator::Shift() {
    iterator->Proceed();
}
