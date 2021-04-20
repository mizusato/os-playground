#ifndef BOOT_H
#define BOOT_H

#define KERNEL_POSITION 0x100000  // 1M ~
#define KERNEL_RESERVED_AMOUNT (8 * 1024 * 1024)
#define DEFAULT_HEAP_START 0x1000000  // 16M ~
#define DEFAULT_HEAP_SIZE (32 * 1024 * 1024)

typedef enum {
    MK_ReservedMemoryType,
    MK_LoaderCode,
    MK_LoaderData,
    MK_BootServicesCode,
    MK_BootServicesData,
    MK_RuntimeServicesCode,
    MK_RuntimeServicesData,
    MK_ConventionalMemory,
    MK_UnusableMemory,
    MK_ACPIReclaimMemory,
    MK_ACPIMemoryNVS,
    MK_MemoryMappedIO,
    MK_MemoryMappedIOPortSpace,
    MK_PalCode,
    MK_MaxMemoryType
 } __attribute__((packed)) MemoryKindEnum;

#define MEMORY_KIND_NAMES \
    "reserved", \
    "ldr-code", \
    "ldr-data", \
    "bs-code", \
    "bs-data", \
    "rs-code", \
    "rs-data", \
    "conventional", \
    "unusable", \
    "acpi-reclaim", \
    "acpi-nvs", \
    "mmio", \
    "mmio-port", \
    "pal-code", \
    "max-mem-type"

typedef union {
    MemoryKindEnum  value;
    unsigned int    raw;
} MemoryKind;

typedef struct {
    MemoryKind   kind;
    unsigned int _;
    unsigned long int  physicalStart;
    unsigned long int  virtualStart;
    unsigned long int  numberOfPages;
    unsigned long int  attribute;
} __attribute__((packed)) MemoryDescriptor;

typedef struct {
    void* mapBuffer;
    unsigned int mapSize;
    unsigned int descSize;
} MemoryInfo;

typedef enum {
    PF_Unsupported,
    PF_RGB,
    PF_BGR
} PixelFormat;

typedef struct {
    unsigned int* framebuffer;
    unsigned int  screenBufferWidth;
    unsigned int  screenViewportWidth;
    unsigned int  screenHeight;
    PixelFormat   pixelFormat;
} GraphicsInfo;

typedef void (*KernelEntryPoint)(MemoryInfo* memInfo, GraphicsInfo* gfxInfo);

#endif
