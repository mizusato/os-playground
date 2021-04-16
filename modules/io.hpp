#ifndef IO_HPP
#define IO_HPP

#include "types.h"

extern "C" {
    Byte InputByte(Word port);
    void OutputByte(Word port, Byte data);
}

#endif