#include "interrupt.hpp"
#include "string.hpp"
#include "panic.hpp"


extern "C" {
    void PanicInterruptHandler();
}

namespace Panic {
    struct MessageType {
        String title;
        String detail;
        MessageType(String title, String detail): title(title), detail(detail) {};
    };
    MessageType* Message;
    char DetailBuffer[256] = {0};
    void Init() {
        String title = "kernel panic";
        String detail = DetailBuffer;
        Message = new MessageType(title, detail);
        Interrupt::Setup(0x77, (Number) PanicInterruptHandler, 0x08, 0x8E);
    }
    const String& GetMessageTitle() {
        return Message->title;
    }
    const String& GetMessageDetail() {
        return Message->detail;
    }
}

void panic(const char* detail) {
    Number i = 0;
    for (const char* ch = detail; (ch != nullptr && *ch != 0 && i < 255); ch += 1) {
        Panic::DetailBuffer[i] = *ch;
        i += 1;
    }
    Panic::DetailBuffer[i] = 0;
    __asm__("int $0x77");
}

