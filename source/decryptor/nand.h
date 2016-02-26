#pragma once

#include "common.h"

#define NAND_SECTOR_SIZE 0x200
#define SECTORS_PER_READ (BUFFER_MAX_SIZE / NAND_SECTOR_SIZE)

// these three are not handled by the feature functions
// they have to be handled by the menu system
#define N_EMUNAND   (1<<29)
#define N_FORCENAND (1<<30)
#define N_NANDWRITE (1<<31)

// return values for the CheckEmuNAND() function
#define EMUNAND_NOT_READY 0 // must be zero
#define EMUNAND_READY     1
#define EMUNAND_GATEWAY   2
#define EMUNAND_REDNAND   3

// these offsets are used by Multi EmuNAND Creator
#define EMUNAND_MULTI_OFFSET_O3DS 0x00200000
#define EMUNAND_MULTI_OFFSET_N3DS 0x00400000

u32 OutputFileNameSelector(char* filename, const char* basename, char* extension);
u32 InputFileNameSelector(char* filename, const char* basename, char* extension, u8* magic, u32 msize, u32 fsize);

// --> FEATURE FUNCTIONS <--
u32 CheckEmuNand(void);
u32 SetNand(bool set_emunand, bool force_emunand);

u32 DumpNand(u32 param);
u32 RestoreNand(u32 param);

