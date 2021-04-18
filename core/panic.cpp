#include "interrupt.hpp"
#include "string.hpp"
#include "panic.hpp"


extern "C" {
    void PanicInterruptHandler();
}

namespace Panic {
    struct Message {
        String title;
        String detail;
        Message(String title, String detail): title(title), detail(detail) {};
    };
    Message* message;
    char detailBuffer[256] = {0};
    void Init() {
        String title = "kernel panic";
        String detail = detailBuffer;
        message = new Message(title, detail);
        Interrupt::Setup(0x77, (Number) PanicInterruptHandler, 0x08, 0x8E);
    }
    String GetMessageTitle() {
        return message->title;
    }
    String GetMessageDetail() {
        return message->detail;
    }
}

void panic(const char* detail) {
    Number i = 0;
    for (const char* ch = detail; (ch != nullptr && *ch != 0 && i < 255); ch += 1) {
        Panic::detailBuffer[i] = *ch;
        i += 1;
    }
    Panic::detailBuffer[i] = 0;
    __asm__("int $0x77");
}

