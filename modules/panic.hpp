#ifndef PANIC_HPP
#define PANIC_HPP


void panic(const char* msg);

class String;

namespace Panic {
    void Init();
    const String& GetMessageTitle();
    const String& GetMessageDetail();
}

extern "C" {
    void panicInterruptHandler(const char* desc);
}

#endif

