#include "vga.h"


Byte* const VideoMemory = (Byte*) 0xB8000;

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25
#define SCREEN_DEPTH  2


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
  OutputByte(0x3D4, 14);
  OutputByte(0x3D5, (Byte) (ptr >> 8));
  OutputByte(0x3D4, 15);
  OutputByte(0x3D5, (Byte) ptr);
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
