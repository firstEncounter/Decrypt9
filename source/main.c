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
            { 0, 0, NULL, NULL, NULL, NULL }
        }
    },
    {
        "NAND Options",
        {
            { 0, 0, "NAND Backup", "Backup NAND", DumpNand, "menu9.bin" },
            { 1, 0, "NAND Restore", "Restore NAND", RestoreNand, "menu10.bin" },
            { 0, 0, "CTR Partitions Decryptor", "Decrypt CTR Partitions", DecryptCtrNandPartition, "menu11.bin" },
            { 1, 0, "CTR Partitions Injector", "Inject CTR Partitions", InjectCtrNandPartition, "menu12.bin" },
            { 0, 0, "TWL Partitions Decryptor", "Decrypt TWL Partitions", DecryptTwlNandPartition, "menu13.bin" },
            { 1, 0, "TWL Partitions Injector", "Inject TWL Partitions", InjectTwlNandPartition, "menu14.bin" },
            { 0, 0, NULL, NULL, NULL, NULL },
            { 0, 0, NULL, NULL, NULL, NULL },
            { 0, 0, NULL, NULL, NULL, NULL },
            { 0, 0, NULL, NULL, NULL, NULL }
        }
    }
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
    
    result = ProcessMenu(menu, sizeof(menu) / sizeof(MenuEntry));
    
    DeinitFS();
    (result == 1) ? Reboot() : PowerOff();
    return 0;
}
