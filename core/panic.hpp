#ifndef PANIC_HPP
#define PANIC_HPP

class String;

namespace Panic {
    void Init();
    String GetMessageTitle();
    String GetMessageDetail();
}

void panic(const char* msg);

extern "C" {
    void handlePanicInterrupt();
}

#endif

