#include "io.hpp"
#include "ps2.hpp"
#include "interrupt.hpp"
#include "mouse.hpp"

#define MOUSE_BIT 0x20
#define MOUSE_COMMAND 0xD4
#define MOUSE_SET_DEFAULTS 0xF6
#define MOUSE_ENABLE_PACKET_STREAMING 0xF4
#define MOUSE_PKT_OVERFLOW_Y 0x80
#define MOUSE_PKT_OVERFLOW_X 0x40
#define MOUSE_PKT_SIGN_Y 0x20
#define MOUSE_PKT_SIGN_X 0x10
#define MOUSE_PKT_ALWAYS_1 0x08
#define MOUSE_PKT_BTN_MIDDLE 0x04
#define MOUSE_PKT_BTN_RIGHT 0x02
#define MOUSE_PKT_BTN_LEFT 0x01

extern "C" void MouseInterruptHandler();

namespace Mouse {
    void SendCommand(Byte cmd) {
        PS2::WriteCtrl(MOUSE_COMMAND);
        PS2::WriteData(cmd);
        PS2::ReadData(); // ACK
    }
    void Init() {
        PS2::WriteCtrl(PS2_GET_COMPAQ_STATUS);
        Byte status = PS2::ReadData();
        status |= 0x2;
        status &= ~ ((Byte) 0x20);
        PS2::WriteCtrl(PS2_SET_COMPAQ_STATUS);
        PS2::WriteData(status);
        PS2::ReadData(); // ACK
        PS2::WriteCtrl(PS2_ENABLE_AUXILIARY);
        PS2::ReadData(); // ACK
        SendCommand(MOUSE_SET_DEFAULTS);
        SendCommand(MOUSE_ENABLE_PACKET_STREAMING);
        Interrupt::Setup(0x2c, (Number) MouseInterruptHandler, 0x08, 0x8E);
        Interrupt::UnmaskPIC1(2);
        Interrupt::UnmaskPIC2(4);
    }
    inline bool ReadyToRead() {
        Byte flags = PS2::ReadCtrl();
        return ((flags & PS2_OUTPUT_STATUS) && (flags & MOUSE_BIT));
    }
    inline Byte ReadPacketPart() {
        return InputByte(PS2_DATA);
    }
    bool ReadPacket(Packet* packet) {
        static Byte buf[3];
        static Number cycle = 0;
        while (ReadyToRead()) {
            Byte part = ReadPacketPart();
            if (cycle == 0) {
                buf[cycle] = part;
                cycle = 1;
            } else if (cycle == 1) {
                buf[cycle] = part;
                cycle = 2;
            } else if (cycle == 2) {
                buf[cycle] = part;
                cycle = 0;
                Byte header = buf[0];
                Number dx = buf[1];
                Number dy = buf[2];
                Button buttons = Button::None;
                if (header & (MOUSE_PKT_OVERFLOW_X | MOUSE_PKT_OVERFLOW_Y)) {
                    return false;
                }
                if (header & MOUSE_PKT_SIGN_X) {
                    dx |= ~ ((Number) 0xFF);
                }
                if (header & MOUSE_PKT_SIGN_Y) {
                    dy |= ~ ((Number) 0xFF);
                }
                if (header & MOUSE_PKT_BTN_LEFT) {
                    buttons = buttons | Button::Left;
                }
                if (header & MOUSE_PKT_BTN_RIGHT) {
                    buttons = buttons | Button::Right;
                }
                if (header & MOUSE_PKT_BTN_MIDDLE) {
                    buttons = buttons | Button::Middle;
                }
                packet->dx = dx;
                packet->dy = dy;
                packet->buttons = buttons;
                return true;
            }
        }
        return false;
    }
};

