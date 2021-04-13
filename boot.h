#ifndef BOOT_H
#define BOOT_H

#define HEAP_START 0x1000000  // 16M ~
#define HEAP_SIZE 0x2000000  // 32M

typedef enum {
    PF_Unsupported,
    PF_RGB,
    PF_BGR
} __attribute__((packed)) PixelFormat;

typedef struct {
    unsigned int* framebuffer;
    unsigned int  screenBufferWidth;
    unsigned int  screenViewportWidth;
    unsigned int  screenHeight;
    PixelFormat   pixelFormat;
} __attribute__((packed)) GraphicsInfo;

typedef void (*KernelEntryPoint)(GraphicsInfo* gfxInfo);

#endif
