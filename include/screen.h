#ifndef OS_SCREEN_H
#define OS_SCREEN_H

Byte* const VideoMemory = (Byte*) 0xB8000;

const Size
  SCREEN_WIDTH = 80,
  SCREEN_HEIGHT = 25,
  SCREEN_DEPTH = 2;

typedef Byte Color;
const Color
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
  WHITE          = 0xF;

Void ClearScreen() {
  Size i;
  for (i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT*SCREEN_DEPTH; i += 1) {
    VideoMemory[i] = 0;
  }
}

Void WriteScreen(Size row, Size col, Byte content, Color color) {
  Size ptr = ((row * SCREEN_WIDTH * SCREEN_DEPTH) + (col * SCREEN_DEPTH));
  VideoMemory[ptr] = content;
  VideoMemory[ptr+1] = color;
}

Void MoveCursor(Size row, Size col) {
  Word ptr = (((Word) row) * SCREEN_WIDTH) + ((Word) col);
  OutPortB(0x3D4, 14);
  OutPortB(0x3D5, (Byte) (ptr >> 8));
  OutPortB(0x3D4, 15);
  OutPortB(0x3D5, (Byte) ptr);
}

Void WriteScreenString(Size row, Size col, String str, Color color) {
  Size start = col;
  Size i;
  for (i = 0; i < str.length; i += 1) {
    Size pos = start + i;
    if (pos >= SCREEN_WIDTH) {
      break;
    }
    WriteScreen(row, pos, str.chars[i], color);
  }
}

#endif
