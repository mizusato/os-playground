#include <efi/efi.h>
#include <efi/efilib.h>
 
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);
  Print(L"=============== Bootloader ===============\n");
  Print(L">>> Hello World\n");
  while(1) __asm__("hlt");
  return EFI_SUCCESS;
}

