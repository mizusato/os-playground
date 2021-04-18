#ifndef PANIC_HPP
#define PANIC_HPP

class String;

namespace Panic {
    void Init();
    const String& GetMessageTitle();
    const String& GetMessageDetail();
}

void panic(const char* msg);

extern "C" {
    void panicInterruptHandler(const char* desc);
}

#endif

