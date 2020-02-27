#include "include/types.h"
#include "include/string.h"
#include "include/system.h"

Void Main() {
  Byte* vidmem = (Byte*) 0xB8000;
  vidmem[0] = 'C';
  vidmem[1] = 0xA;
}
