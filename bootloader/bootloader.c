#include <efi/efi.h>
#include <efi/efilib.h>


#define ACCEPTABLE_MAXIMUM_SCREEN_WIDTH 1336
#define ACCEPTABLE_MAXIMUM_SCREEN_HEIGHT 768

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"=============== Bootloader ===============\n");
    Print(L">>> Hello World\n");
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if(EFI_ERROR(status)) {
        Print(L"Unable to locate GOP\n");
        return EFI_ABORTED;
    }
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo = gop->Mode->SizeOfInfo;
    UINTN numModes = gop->Mode->MaxMode;
    Print(L"Graphics Mode Candidates: ");
    UINTN i;
    UINT32 maxWidth = 0;
    UINT32 maxHeight = 0;
    UINTN maxSizeMode = 0;
    for (i = 0; i < numModes; i += 1) {
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
    // uefi_call_wrapper(BS->Stall, 1, 3000*1000);
    UINT32* fb = (UINT32*) gop->Mode->FrameBufferBase;
    UINT32 lineSize = gop->Mode->Info->HorizontalResolution;
    UINTN j;
    for (i = 0; i < gop->Mode->Info->VerticalResolution; i += 1) {
        for (j = 0; j < gop->Mode->Info->PixelsPerScanLine; j += 1) {
            fb[(i * lineSize) + j] = (0x9944DDCC + j*0x80);
        }
    }
    while(1) __asm__("hlt");
    return EFI_SUCCESS;
}

