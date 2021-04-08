#include "vga.h"


typedef struct _Cell {
  Byte   content;
  Color  color;
} Cell;

Cell* DisplayBuffer = (Cell*) 0xB8000;

#define WIDTH  80
#define HEIGHT 25


Color ComposeColor(Color fg, Color bg) {
  return (((bg & 7) << 4) | fg);
}

Void ClearScreen() {
  Cell blank = { 0, ComposeColor(MAGENTA, GRAY) };
  Size i;
  for (i = 0; i < WIDTH*HEIGHT; i += 1) {
    DisplayBuffer[i] = blank;
  }
}

Void ClearRow(Size row) {
  Cell blank = { 0, ComposeColor(MAGENTA, GRAY) };
  Size i;
  for (i = 0; i < WIDTH; i += 1) {
    DisplayBuffer[row*WIDTH + i] = blank;
  }
}


Void WriteScreen(Size row, Size col, Byte content, Color color) {
  Cell cell = { content, color };
  DisplayBuffer[row*WIDTH + col] = cell;
}


Void MoveCursor(Size row, Size col) {
  Word ptr = (((Word) row) * WIDTH) + ((Word) col);
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
    if (pos >= WIDTH) {
      break;
    }
    WriteScreen(row, pos, str.chars[i], color);
  }
}
