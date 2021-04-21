#include "io.hpp"
#include "ps2.hpp"

namespace PS2 {
    inline void WaitRead() {
        Number timeout = 100000;
        while (--timeout) {
            if (InputByte(PS2_CTRL) & PS2_OUTPUT_STATUS) {
                return;
            }
        }
    }
    inline void PrepareWrite() {
        Number timeout = 100000;
        while (--timeout) {
            if ( !(InputByte(PS2_CTRL) & PS2_INPUT_STATUS) ) {
                return;
            }
        }
    }
    Byte ReadData() {
        WaitRead();
        return InputByte(PS2_DATA);
    }
    void WriteData(Byte byte) {
        PrepareWrite();
        OutputByte(PS2_DATA, byte);
    }
    Byte ReadCtrl() {
        WaitRead();
        return InputByte(PS2_CTRL);
    }
    void WriteCtrl(Byte byte) {
        PrepareWrite();
        OutputByte(PS2_CTRL, byte);
    }
};

