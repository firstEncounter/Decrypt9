#include "common.h"
#include "draw.h"
#include "fs.h"
#include "menu.h"
#include "i2c.h"
#include "decryptor/features.h"


MenuInfo menu[] =
{
    {
        "Decryption",
        {
            { 0, 0, "NCCH Xorpad Generator", "NCCH Padgen", NcchPadgen, "menu0.bin" },
            { 0, 0, "SD Xorpad Generator", "SD Padgen", SdPadgen, "menu1.bin" },
            { 0, 0, "CTR ROM Decryptor", "Decrypt ROM(s)", DecryptNcsdNcchBatch, "menu2.bin" },
            { 0, 0, "Titlekey Decryptor (file)", "Titlekey Decryption", DecryptTitlekeysFile, "menu3.bin" },
            { 0, 0, "Titlekey Decryptor (NAND)", "Titlekey Decryption", DecryptTitlekeysNand, "menu4.bin" },
            { 0, 0, "Ticket Dumper", "Dump Ticket", DumpTicket, "menu5.bin" },
            { 0, 0, "NAND FAT16 Xorpad Generator", "CTRNAND Padgen", CtrNandPadgen, "menu6.bin" },
            { 0, 0, "TWLN FAT16 Xorpad Generator", "TWLN Padgen", TwlNandPadgen, "menu7.bin" },
            { 0, 0, "FIRM0 Xorpad Generator", "FIRM Padgen", FirmPadgen, "menu8.bin" },
            { 0, 0, NULL, NULL, NULL, NULL } //menu9.bin placeholder not used yet
        }
    },
    {
        "NAND Options",
        {
            { 0, 0, "NAND Backup", "Backup NAND", DumpNand, "menu10.bin" },
            { 1, 0, "NAND Restore", "Restore NAND", RestoreNand, "menu11.bin" },
            { 0, 0, "CTR Partitions Decryptor", "Decrypt CTR Partitions", DecryptCtrNandPartition, "menu12.bin" },
            { 1, 0, "CTR Partitions Injector", "Inject CTR Partitions", InjectCtrNandPartition, "menu13.bin" },
            { 0, 0, "TWL Partitions Decryptor", "Decrypt TWL Partitions", DecryptTwlNandPartition, "menu14.bin" },
            { 1, 0, "TWL Partitions Injector", "Inject TWL Partitions", InjectTwlNandPartition, "menu15.bin" },
            { 0, 0, "Dump movable.sed", "", DumpMovableSed, "menu16.bin" },
            { 0, 0, "Dump SecureInfo_A", "", DumpSecureInfoA, "menu17.bin" },
            { 1, 0, "Inject movable.sed", "", InjectMovableSed, "menu18.bin" },
            { 1, 0, "Inject SecureInfo_A", "", InjectSecureInfoA, "menu19.bin" }
        }
    },
    {
        "EmuNAND  File Options",
        {
            { 0, 1, "Dump ticket_emu.db", "Dump ticket", DumpTicket, "menu20.bin" },
            { 0, 1, "Dump movable.sed", "Dump movable.sed", DumpMovableSed, "menu21.bin" },
            { 0, 1, "Dump SecureInfo_A", "Dump SecureInfo_A", DumpSecureInfoA, "menu22.bin" },
            { 1, 1, "Inject movable.sed", "Inject movable.sed", InjectMovableSed, "menu23.bin" },
            { 1, 1, "Inject SecureInfo_A", "Inject SecureInfo_A", InjectSecureInfoA, "menu24.bin" },
            { 0, 1, "Dump seedsave.bin", "Dump seedsave.bin", DumpSeedsave, "menu25.bin" },
            { 0, 1, "Update SeedDB", "Update SeedDB", UpdateSeedDb, "menu26.bin" },
            { 0, 1, "Titlekey Decrypt(EMU)", "Titlekey Decryption", DecryptTitlekeysNand, "menu27.bin" },
            { 0, 1, NULL, NULL, NULL, NULL }, //menu28.bin placeholder not used yet
            { 0, 1, NULL, NULL, NULL, NULL } //menu29.bin placeholder not used yet
        }
    },
#ifdef EXPERIMENTAL
    {
        "EmuNAND Options", //Not Implemented in menu system yet
        {
            { 0, 1, "EmuNAND Backup", "Backup EmuNAND", DumpNand, NULL },
            { 0, 1, "All Partitions Dump", "Dump Partitions", DecryptAllNandPartitions, NULL },
            { 0, 1, "TWLNAND Partition Dump", "Decrypt TWL Partitions", DecryptTwlNandPartition, NULL },
            { 0, 1, "CTRNAND Partition Dump", "Decrypt CTR Partitions", DecryptCtrNandPartition, NULL },
            { 1, 1, "EmuNAND Restore", "Restore EmuNAND", RestoreNand, NULL },
            { 1, 1, "All Partitions Inject", "Inject Partitions", InjectAllNandPartitions, NULL },
            { 1, 1, "TWLNAND Partition Inject", "TWLNAND Partition Inject", InjectTwlNandPartition, NULL },
            { 0, 1, "CTRNAND Partition Inject", "CTRNAND Partition Inject", InjectCtrNandPartition, NULL },
            { 0, 1, NULL, NULL, NULL, NULL },
            { 0, 1, NULL, NULL, NULL, NULL }
        }
    }
#endif
};

void Reboot()
{
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
    while(true);
}

void PowerOff()
{
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);
    while (true);
}

int main()
{
    u32 result;
    
    InitFS();
    DebugInit();
    
    result = ProcessMenu(menu, sizeof(menu) / sizeof(MenuInfo));
    
    DeinitFS();
    (result == 1) ? Reboot() : PowerOff();
    return 0;
}
