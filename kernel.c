#include "lib/types.h"
#include "lib/string.h"
#include "lib/vga.h"
#include "lib/keyboard.h"

Void Main() {
  ClearScreen();
  WriteScreenString(0, 0, StringFrom("Hello World"), YELLOW);
  MoveCursor(3, 5);
  Byte buf[100];
  while (True) {
    if (KeyboardReady()) {
      Byte key = ReadKeyboard();
      if (key != 0) {
      	String str = DecimalString(key, buf);
      	ClearScreen();
      	WriteScreenString(0, 0, str, BRIGHT_CYAN);
      	WriteScreen(0, (str.length + 1), key, YELLOW);
      }
    }
  }
}
