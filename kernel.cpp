#include "boot.h"

typedef unsigned int Number;
typedef unsigned long Uint32;

extern "C"
void Main(GraphicsInfo* gfxInfo) {
    for (Number i = 0; i < gfxInfo->screenHeight; i += 1) {
        for (Number j = 0; j < gfxInfo->screenViewportWidth; j += 1) {
            (gfxInfo->framebuffer)[(i * gfxInfo->screenBufferWidth) + j] = (0x99DD44CC + j*0x80);
        }
    }
    while(1) __asm__("hlt");
}
