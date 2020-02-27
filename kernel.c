#include "include/types.h"
#include "include/string.h"
#include "include/system.h"
#include "include/screen.h"

Void Main() {
  ClearScreen();
  WriteScreenString(0, 0, StringFrom("Hello World"), YELLOW);
  MoveCursor(3, 5);
}
