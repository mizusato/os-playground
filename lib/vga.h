#ifndef OS_VGA_H
#define OS_VGA_H

#include "types.h"
#include "io.h"
#include "string.h"

typedef enum {
  BLACK   = 0x0,
  BLUE    = 0x1,
  GREEN   = 0x2,
  CYAN    = 0x3,
  RED     = 0x4,
  MAGENTA = 0x5,
  BROWN   = 0x6,
  GRAY    = 0x7,
  DARK_DRAY      = 0x8,
  BRIGHT_BLUE    = 0x9,
  BRIGHT_GREEN   = 0xA,
  BRIGHT_CYAN    = 0xB,
  BRIGHT_RED     = 0xC,
  BRIGHT_MAGENTA = 0xD,
  YELLOW         = 0xE,
  WHITE          = 0xF
} Color;

Void ClearScreen();

Void WriteScreen(Size row, Size col, Byte content, Color color);

Void MoveCursor(Size row, Size col);

Void WriteScreenString(Size row, Size col, String str, Color color);


#endif  // OS_VGA_H
