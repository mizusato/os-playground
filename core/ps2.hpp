#ifndef PS2_HPP
#define PS2_HPP

#include "types.h"

#define PS2_DATA 0x60
#define PS2_CTRL 0x64
#define PS2_OUTPUT_STATUS 0x1
#define PS2_INPUT_STATUS 0x2
#define PS2_ENABLE_AUXILIARY 0xA8
#define PS2_GET_COMPAQ_STATUS 0x20
#define PS2_SET_COMPAQ_STATUS 0x60

namespace PS2 {
    Byte ReadData();
    void WriteData(Byte data);
    Byte ReadCtrl();
    void WriteCtrl(Byte data);
};

#endif

