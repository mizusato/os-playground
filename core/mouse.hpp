#ifndef MOUSE_HPP
#define MOUSE_HPP

#include "types.h"

namespace Mouse {
    enum class Button: Number {
        None   = 0,
        Left   = 0x01,
        Right  = 0x02,
        Middle = 0x04
    };
    constexpr Button operator | (const Button a, const Button b) {
        return (Button)(Number(a) | Number(b));
    }
    struct Packet {
        Number  dx;
        Number  dy;
        Button  buttons;
    };
    void Init();
    bool ReadPacket(Packet* packet);
};

extern "C" {
    void handleMouseInterrupt();
}

#endif

