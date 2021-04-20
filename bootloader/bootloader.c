#include <efi/efi.h>
#include <efi/efilib.h>
#include "../boot.h"


#define KERNEL_FILE_NAME L"kernel.bin"
#define ACCEPTABLE_MAXIMUM_SCREEN_WIDTH 1336
#define ACCEPTABLE_MAXIMUM_SCREEN_HEIGHT 768

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image);
UINT64 FileSize(EFI_FILE_HANDLE FileHandle);

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"=============== Bootloader ===============\n");
    Print(L">>> Hello World\n");
    /* Load Kernel */
    EFI_STATUS status;
    CHAR16* kernelFileName = KERNEL_FILE_NAME;
    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);
    EFI_FILE_HANDLE fd;
    BOOLEAN kernelLoaded;
    status = uefi_call_wrapper(Volume->Open, 5, Volume, &fd, kernelFileName,
        EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    if(EFI_ERROR(status)) {
        Print(L"*** Unable to open kernel file\n");
        kernelLoaded = FALSE;
        goto kernel_failed_to_load;
    }
    UINT8* kernelContent = (UINT8*) KERNEL_POSITION;
    UINT64 size = FileSize(fd);
    status = uefi_call_wrapper(fd->Read, 3, fd, &size, kernelContent);
    if(EFI_ERROR(status)) {
        Print(L"*** Unable to read kernel file\n");
        kernelLoaded = FALSE;
        goto kernel_failed_to_load;
    }
    uefi_call_wrapper(fd->Close, 1, fd);
    kernelLoaded = TRUE; kernel_failed_to_load: {}
    /* Get Memory Map */
    UINT8 mapBuffer[16 * 1024];
    UINTN mapSize = sizeof(mapBuffer);
    UINTN mapKey, descSize, descVer;
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapSize, &mapBuffer, &mapKey, &descSize, &descVer);
    if(EFI_ERROR(status)) {
        Print(L"Unable to get memory map\n");
        return EFI_ABORTED;
    }

    /* Initialize GOP */
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(status)) {
        Print(L"Unable to locate GOP\n");
        return EFI_ABORTED;
    }
    UINTN SizeOfInfo = gop->Mode->SizeOfInfo;
    UINTN numModes = gop->Mode->MaxMode;
    Print(L"Graphics Mode Candidates: ");
    UINTN i;
    UINT32 maxWidth = 0;
    UINT32 maxHeight = 0;
    UINTN maxSizeMode = 0;
    for (i = 0; i < numModes; i += 1) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
        UINT32 width = info->PixelsPerScanLine;
        UINT32 height = info->VerticalResolution;
        if (width > ACCEPTABLE_MAXIMUM_SCREEN_WIDTH || height > ACCEPTABLE_MAXIMUM_SCREEN_HEIGHT) {
            continue;
        }
        if (width >= maxWidth && height >= maxHeight) {
            maxWidth = width;
            maxHeight = height;
            maxSizeMode = i;
        }
        Print(L" #%d(%dx%d)",
            i,
            info->PixelsPerScanLine,
            info->VerticalResolution
        );
    }
    Print(L"\nChoose mode #%d\n", maxSizeMode);
    // uefi_call_wrapper(BS->Stall, 1, 3000*1000);
    status = uefi_call_wrapper(gop->SetMode, 2, gop, maxSizeMode);
    if(EFI_ERROR(status)) {
        Print(L"Unable to set graphics mode\n");
        return EFI_ABORTED;
    }
    Print(L"Framebuffer address %x size %d, width %d height %d pixelsperline %d\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferSize,
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        gop->Mode->Info->PixelsPerScanLine
    );
    /* Boot the Kernel */
    if (kernelLoaded) {
        MemoryInfo kernelMemoryInfo;
        kernelMemoryInfo.mapBuffer = (void*) mapBuffer;
        kernelMemoryInfo.mapSize = mapSize;
        kernelMemoryInfo.descSize = descSize;
        GraphicsInfo kernelGfxInfo;
        EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* mode = gop->Mode;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = mode->Info;
        kernelGfxInfo.framebuffer = (UINT32*) mode->FrameBufferBase;
        kernelGfxInfo.screenHeight = info->VerticalResolution;
        kernelGfxInfo.screenBufferWidth = info->HorizontalResolution;
        kernelGfxInfo.screenViewportWidth = info->PixelsPerScanLine;
        if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
            kernelGfxInfo.pixelFormat = PF_RGB;
        } else if (info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
            kernelGfxInfo.pixelFormat = PF_BGR;
        } else {
            kernelGfxInfo.pixelFormat = PF_Unsupported;
        }
        KernelEntryPoint entry = (KernelEntryPoint) kernelContent;
        entry(&kernelMemoryInfo, &kernelGfxInfo);
    }
    while(1) __asm__("hlt");
    return EFI_SUCCESS;
}

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
  EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
  EFI_FILE_HANDLE Volume;                                 /* the volume's interface */
 
  /* get the loaded image protocol interface for our "image" */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
  /* get the volume handle */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
  return Volume;
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle)
{
  UINT64 ret;
  EFI_FILE_INFO *FileInfo;         /* file information structure */
  /* get the file's size */
  FileInfo = LibFileInfo(FileHandle);
  ret = FileInfo->FileSize;
  FreePool(FileInfo);
  return ret;
}

