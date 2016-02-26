#include "common.h"
#include "draw.h"
#include "fs.h"
#include "menu.h"
#include "i2c.h"
#include "decryptor/nand.h"

#define SUBMENU_START 1

MenuInfo menu[] =
{
    {
        "Backup Restore Options", 4,
        {
            { "NAND Backup",                  &DumpNand,              0 },
            { "NAND Restore",                 &RestoreNand,           N_NANDWRITE },
            { "EmuNAND Backup",               &DumpNand,              N_EMUNAND },
            { "EmuNAND Restore",              &RestoreNand,           N_NANDWRITE | N_EMUNAND | N_FORCENAND }
        }
    },
    {
        NULL, 0, {}, // empty menu to signal end
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
    ClearScreenFull(true, true);
    InitFS();

    u32 menu_exit = ProcessMenu(menu, SUBMENU_START);
    
    DeinitFS();
    (menu_exit == MENU_EXIT_REBOOT) ? Reboot() : PowerOff();
    return 0;
}
