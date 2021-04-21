#include "panic.hpp"
#include "string.hpp"


String::String(const char* data):
    impl(Shared<String::Impl>(new LegacyString(data))) {};

String::String(Unique<List<Char>> list):
    impl(Shared<String::Impl>(new ListString(std::move(list)))) {};

String String::Chr(Char ch) {
    return String(Unique<List<Char>>(new List<Char>(ch)));
}

void String::Builder::Write(String s) {
    for (auto it = s.Iterate(); it->HasCurrent(); it->Proceed()) {
        buf->Append(it->Current());
    }
}
String String::Builder::Collect() {
    return std::move(buf);
}

Unique<String::Iterator> LegacyString::Iterate() const {
    return Unique<String::Iterator>(new LegacyString::Iterator(data));
}
bool LegacyString::Iterator::HasCurrent() const {
    return (data != nullptr && *data != 0);
}
Char LegacyString::Iterator::Current() const {
    if (!(HasCurrent())) { panic("invalid iterator usage"); }
    return static_cast<Char>(*data);
}
void LegacyString::Iterator::Proceed() {
    if (!(HasCurrent())) { panic("invalid iterator usage"); }
    data++;
}

Unique<String::Iterator> ListString::Iterate() const {
    return Unique<String::Iterator>(new ListString::Iterator(list->Iterate()));
}
bool ListString::Iterator::HasCurrent() const {
    return iterator->HasCurrent();
}
Char ListString::Iterator::Current() const {
    return iterator->Current();
}
void ListString::Iterator::Proceed() {
    iterator->Proceed();
}

String NumberToString(Number n, Number base);
String::String(Number n): String(NumberToString(n, 10)) {}
String String::Hex(Number n) { return NumberToString(n, 16); }
String String::Oct(Number n) { return NumberToString(n, 8); }
String String::Bin(Number n) { return NumberToString(n, 2); }

String String::ReadableSize(Number n) {
    if (n == 0) {
        return "0";
    }
    auto list = Unique<List<String>>(new List<String>());
    Number m;
    const char* units = "bkmgt";
    for (const char* unit = units; *unit != 0; unit += 1) {
        if (n > 0) {
            m = (n & 0x3FF);
            n = (n >> 10);
            if (m > 0) {
                String::Builder buf;
                buf.Write(String(m));
                buf.Write(String::Chr(*unit));
                list->Prepend(buf.Collect());
            }
        } else {
            break;
        }
    }
    return String::Join(*list, "+");
}

String String::Pad(String s, Number n) {
    String::Builder buf;
    Number count = 0;
    for (auto it = s.Iterate(); it->HasCurrent(); it->Proceed()) {
        buf.Write(String::Chr(it->Current()));
        count += 1;
    }
    if (count < n) {
        for (Number i = 0; i < (n - count); i += 1) {
            buf.Write(" ");
        }
    }
    return buf.Collect();
}

String String::Join(const List<String>& list, String sep) {
    String::Builder buf;
    bool first = true;
    for (auto it = list.Iterate(); it->HasCurrent(); it->Proceed()) {
        if (!(first)) { buf.Write(sep); }
        first = false;
        buf.Write(it->Current());
    }
    return buf.Collect();
}

String NumberToString(Number n, Number base) {
    static const char* mapping = "0123456789abcdef";
    char reversed[24];
    Number len = 0;
    while(n > 0) {
        char digit = mapping[n % base];
        n = (n / base);
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

