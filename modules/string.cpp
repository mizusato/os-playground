#include "string.hpp"


String::String(const char* data):
    impl(Shared<String::Impl>(new LegacyString(data))) {};

String::String(Unique<List<Char>> list):
    impl(Shared<String::Impl>(new ListString(std::move(list)))) {};


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

