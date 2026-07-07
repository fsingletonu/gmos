#ifndef _VGAVIDEO_H_
#define _VGAVIDEO_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include "Include/BootInfo.h"

EFI_STATUS GetVGAInfo(IN EFI_SYSTEM_TABLE *SystemTable,
                      OUT BootInfoHead *Info);

#endif
