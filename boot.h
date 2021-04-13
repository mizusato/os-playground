#ifndef BOOT_H
#define BOOT_H

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
