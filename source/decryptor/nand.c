#include "fs.h"
#include "draw.h"
#include "hid.h"
#include "platform.h"
#include "decryptor/aes.h"
#include "decryptor/decryptor.h"
#include "decryptor/nand.h"
#include "fatfs/sdmmc.h"

static u32 emunand_header = 0;
static u32 emunand_offset = 0;

u32 CheckEmuNand(void)
{
    u8* buffer = BUFFER_ADDRESS;
    u32 nand_size_sectors = getMMCDevice(0)->total_size;
    u32 multi_sectors = (GetUnitPlatform() == PLATFORM_3DS) ? EMUNAND_MULTI_OFFSET_O3DS : EMUNAND_MULTI_OFFSET_N3DS;
    u32 ret = EMUNAND_NOT_READY;

    // check the MBR for presence of a hidden partition
    sdmmc_sdcard_readsectors(0, 1, buffer);
    u32 hidden_sectors = getle32(buffer + 0x1BE + 0x8);
    
    for (u32 offset_sector = 0; offset_sector + nand_size_sectors < hidden_sectors; offset_sector += multi_sectors) {
        // check for Gateway type EmuNAND
        sdmmc_sdcard_readsectors(offset_sector + nand_size_sectors, 1, buffer);
        if (memcmp(buffer + 0x100, "NCSD", 4) == 0) {
            ret |= EMUNAND_GATEWAY << (2 * (offset_sector / multi_sectors)); 
            continue;
        }
        // check for RedNAND type EmuNAND
        sdmmc_sdcard_readsectors(offset_sector + 1, 1, buffer);
        if (memcmp(buffer + 0x100, "NCSD", 4) == 0) {
            ret |= EMUNAND_REDNAND << (2 * (offset_sector / multi_sectors)); 
            continue;
        }
        // EmuNAND ready but not set up
       ret |= EMUNAND_READY << (2 * (offset_sector / multi_sectors)); 
    }
    
    return ret;
}

u32 SetNand(bool set_emunand, bool force_emunand)
{
    if (set_emunand) {
        u32 emunand_state = CheckEmuNand();
        u32 emunand_count = 0;
        u32 offset_sector = 0;
        
        for (emunand_count = 0; (emunand_state >> (2 * emunand_count)) & 0x3; emunand_count++);
        if (emunand_count > 1) { // multiple EmuNANDs -> use selector
            u32 multi_sectors = (GetUnitPlatform() == PLATFORM_3DS) ? EMUNAND_MULTI_OFFSET_O3DS : EMUNAND_MULTI_OFFSET_N3DS;
            u32 emunand_no = 0;
            Debug("Use arrow keys and <A> to choose EmuNAND");
            while (true) {
                u32 emunandn_state = (emunand_state >> (2 * emunand_no)) & 0x3;
                offset_sector = emunand_no * multi_sectors;
                Debug("\rEmuNAND #%u: %s", emunand_no, (emunandn_state == EMUNAND_READY) ? "EmuNAND ready" : (emunandn_state == EMUNAND_GATEWAY) ? "GW EmuNAND" : "RedNAND");
                // user input routine
                u32 pad_state = InputWait();
                if (pad_state & BUTTON_DOWN) {
                    emunand_no = (emunand_no + 1) % emunand_count;
                } else if (pad_state & BUTTON_UP) {
                    emunand_no = (emunand_no) ?  emunand_no - 1 : emunand_count - 1;
                } else if (pad_state & BUTTON_A) {
                    Debug("EmuNAND #%u", emunand_no);
                    emunand_state = emunandn_state;
                    break;
                } else if (pad_state & BUTTON_B) {
                    Debug("(cancelled by user)");
                    return 2;
                }
            }
        }
        
        if ((emunand_state == EMUNAND_READY) && force_emunand)
            emunand_state = EMUNAND_GATEWAY;
        switch (emunand_state) {
            case EMUNAND_NOT_READY:
                Debug("SD is not formatted for EmuNAND");
                return 1;
            case EMUNAND_GATEWAY:
                emunand_header = offset_sector + getMMCDevice(0)->total_size;
                emunand_offset = offset_sector;
                Debug("Using EmuNAND @ %06X/%06X", emunand_header, emunand_offset);
                return 0;
            case EMUNAND_REDNAND:
                emunand_header = offset_sector + 1;
                emunand_offset = offset_sector + 1;
                Debug("Using RedNAND @ %06X/%06X", emunand_header, emunand_offset);
                return 0;
            default:
                Debug("EmuNAND is not available");
                return 1;
        }
    } else {
        emunand_header = 0;
        emunand_offset = 0;
        return 0;
    }
}

static inline int ReadNandSectors(u32 sector_no, u32 numsectors, u8 *out)
{
    if (emunand_header) {
        if (sector_no == 0) {
            int errorcode = sdmmc_sdcard_readsectors(emunand_header, 1, out);
            if (errorcode) return errorcode;
            sector_no = 1;
            numsectors--;
            out += 0x200;
        }
        return sdmmc_sdcard_readsectors(sector_no + emunand_offset, numsectors, out);
    } else return sdmmc_nand_readsectors(sector_no, numsectors, out);
}

static inline int WriteNandSectors(u32 sector_no, u32 numsectors, u8 *in)
{
    if (emunand_header) {
        if (sector_no == 0) {
            int errorcode = sdmmc_sdcard_writesectors(emunand_header, 1, in);
            if (errorcode) return errorcode;
            sector_no = 1;
            numsectors--;
            in += 0x200;
        }
        return sdmmc_sdcard_writesectors(sector_no + emunand_offset, numsectors, in);
    } else return sdmmc_nand_writesectors(sector_no, numsectors, in);
}

u32 OutputFileNameSelector(char* filename, const char* basename, char* extension) {
    char bases[3][64] = { 0 };
    char* dotpos = NULL;
    
    // build first base name and extension
    strncpy(bases[0], basename, 63);
    dotpos = strrchr(bases[0], '.');
    
    if (dotpos) {
        *dotpos = '\0';
        if (!extension)
            extension = dotpos + 1;
    }
    
    // build other two base names
    snprintf(bases[1], 63, "%s_%s", bases[0], (emunand_header) ? "emu" : "sys");
    snprintf(bases[2], 63, "%s%s" , (emunand_header) ? "emu" : "sys", bases[0]);
    
    u32 fn_id = (emunand_header) ? 1 : 0;
    u32 fn_num = (emunand_header) ? (emunand_offset / ((GetUnitPlatform() == PLATFORM_3DS) ? EMUNAND_MULTI_OFFSET_O3DS : EMUNAND_MULTI_OFFSET_N3DS)) : 0;
    bool exists = false;
    char extstr[16] = { 0 };
    if (extension)
        snprintf(extstr, 15, ".%s", extension);
    Debug("Use arrow keys and <A> to choose a name");
    while (true) {
        char numstr[2] = { 0 };
        // build and output file name (plus "(!)" if existing)
        numstr[0] = (fn_num > 0) ? '0' + fn_num : '\0';
        snprintf(filename, 63, "%s%s%s", bases[fn_id], numstr, extstr);
        if ((exists = FileOpen(filename)))
            FileClose();
        Debug("\r%s%s", filename, (exists) ? " (!)" : "");
        // user input routine
        u32 pad_state = InputWait();
        if (pad_state & BUTTON_DOWN) { // increment filename id
            fn_id = (fn_id + 1) % 3;
        } else if (pad_state & BUTTON_UP) { // decrement filename id
            fn_id = (fn_id > 0) ? fn_id - 1 : 2;
        } else if ((pad_state & BUTTON_RIGHT) && (fn_num < 9)) { // increment number
            fn_num++;
        } else if ((pad_state & BUTTON_LEFT) && (fn_num > 0)) { // decrement number
            fn_num--;
        } else if (pad_state & BUTTON_A) {
            Debug("%s%s", filename, (exists) ? " (!)" : "");
            break;
        } else if (pad_state & BUTTON_B) {
            Debug("(cancelled by user)");
            return 2;
        }
    }
    
    // overwrite confirmation
    if (exists) {
        Debug("Press <A> to overwrite existing file");
        while (true) {
            u32 pad_state = InputWait();
            if (pad_state & BUTTON_A) {
                break;
            } else if (pad_state & BUTTON_B) {
                Debug("(cancelled by user)");
                return 2;
            }
        }
    }
    
    return 0;
}

u32 InputFileNameSelector(char* filename, const char* basename, char* extension, u8* magic, u32 msize, u32 fsize) {
    char** fnptr = (char**) 0x20400000; // allow using 0x8000 byte
    char* fnlist = (char*) 0x20408000; // allow using 0x80000 byte
    u32 n_names = 0;
    
    // get the file list - try work directory first
    if (!GetFileList(WORK_DIR, fnlist, 0x80000, false, true, false) && !GetFileList("/", fnlist, 0x800000, false, true, false)) {
        Debug("Failed retrieving the file names list");
        return 1;
    }
    
    // get base name, extension
    char base[64] = { 0 };
    if (basename != NULL) {
        // build base name and extension
        strncpy(base, basename, 63);
        char* dotpos = strrchr(base, '.');
        if (dotpos) {
            *dotpos = '\0';
            if (!extension)
                extension = dotpos + 1;
        }
    }
    
    // limit magic number size
    if (msize > 0x200)
        msize = 0x200;
    
    // parse the file names list for usable entries
    for (char* fn = strtok(fnlist, "\n"); fn != NULL; fn = strtok(NULL, "\n")) {
        u8 data[0x200];
        char* dotpos = strrchr(fn, '.');
        if (strrchr(fn, '/'))
            fn = strrchr(fn, '/') + 1;
        if (strnlen(fn, 128) > 63)
            continue; // file name too long
        if ((basename != NULL) && !strstr(fn, base))
            continue; // basename check failed
        if ((extension != NULL) && (dotpos != NULL) && (strncmp(dotpos + 1, extension, strnlen(extension, 16))))
            continue; // extension check failed
        else if ((extension == NULL) != (dotpos == NULL))
            continue; // extension check failed
        if (!FileOpen(fn))
            continue; // file can't be opened
        if (fsize && (FileGetSize() != fsize)) {
            FileClose();
            continue; // file size check failed
        }
        if (msize) {
            if (FileRead(data, msize, 0) != msize) {
                FileClose();
                continue; // can't be read
            }
            if (memcmp(data, magic, msize) != 0) {
                FileClose();
                continue; // magic number does not match
            }
        }
        FileClose();
        // this is a match - keep it
        fnptr[n_names++] = fn;
        if (n_names * sizeof(char**) >= 0x8000)
            return 1;
    }
    if (n_names == 0) {
        Debug("No usable file found");
        return 1;
    }
    
    u32 index = 0;
    Debug("Use arrow keys and <A> to choose a file");
    while (true) {
        snprintf(filename, 63, "%s", fnptr[index]);
        Debug("\r%s", filename);
        u32 pad_state = InputWait();
        if (pad_state & BUTTON_DOWN) { // next filename
            index = (index + 1) % n_names;
        } else if (pad_state & BUTTON_UP) { // previous filename
            index = (index > 0) ? index - 1 : n_names - 1;
        } else if (pad_state & BUTTON_A) {
            Debug("%s", filename);
            break;
        } else if (pad_state & BUTTON_B) {
            Debug("(cancelled by user)");
            return 2;
        }
    }
    
    return 0;
}

u32 DumpNand(u32 param)
{
    char filename[64];
    u8* buffer = BUFFER_ADDRESS;
    u32 nand_size = getMMCDevice(0)->total_size * NAND_SECTOR_SIZE;
    u32 result = 0;

    Debug("Dumping %sNAND. Size (MB): %u", (param & N_EMUNAND) ? "Emu" : "Sys", nand_size / (1024 * 1024));
    
    if (OutputFileNameSelector(filename, "NAND.bin", NULL) != 0)
        return 1;
    if (!DebugFileCreate(filename, true))
        return 1;

    u32 n_sectors = nand_size / NAND_SECTOR_SIZE;
    for (u32 i = 0; i < n_sectors; i += SECTORS_PER_READ) {
        u32 read_sectors = min(SECTORS_PER_READ, (n_sectors - i));
        ShowProgress(i, n_sectors);
        ReadNandSectors(i, read_sectors, buffer);
        if(!DebugFileWrite(buffer, NAND_SECTOR_SIZE * read_sectors, i * NAND_SECTOR_SIZE)) {
            result = 1;
            break;
        }
    }

    ShowProgress(0, 0);
    FileClose();

    return result;
}

u32 RestoreNand(u32 param)
{
    char filename[64];
    u8* buffer = BUFFER_ADDRESS;
    u32 nand_size = getMMCDevice(0)->total_size * NAND_SECTOR_SIZE;
    u32 result = 0;
    u8 magic[4];

    if (!(param & N_NANDWRITE)) // developer screwup protection
        return 1;
        
    // User file select
    if (InputFileNameSelector(filename, "NAND.bin", NULL, NULL, 0, nand_size) != 0)
        return 1;
    
    if (!DebugFileOpen(filename))
        return 1;
    if (nand_size != FileGetSize()) {
        FileClose();
        Debug("NAND backup has the wrong size!");
        return 1;
    };
    if(!DebugFileRead(magic, 4, 0x100)) {
        FileClose();
        return 1;
    }
    if (memcmp(magic, "NCSD", 4) != 0) {
        FileClose();
        Debug("Not a proper NAND backup!");
        return 1;
    }
    
    Debug("Restoring %sNAND. Size (MB): %u", (param & N_EMUNAND) ? "Emu" : "Sys", nand_size / (1024 * 1024));

    u32 n_sectors = nand_size / NAND_SECTOR_SIZE;
    for (u32 i = 0; i < n_sectors; i += SECTORS_PER_READ) {
        u32 read_sectors = min(SECTORS_PER_READ, (n_sectors - i));
        ShowProgress(i, n_sectors);
        if(!DebugFileRead(buffer, NAND_SECTOR_SIZE * read_sectors, i * NAND_SECTOR_SIZE)) {
            result = 1;
            break;
        }
        WriteNandSectors(i, read_sectors, buffer);
    }

    ShowProgress(0, 0);
    FileClose();

    return result;
}
