#include "draw.h"
#include "hid.h"
#include "menu.h"

#define GFX_WARNING     "warning.bin"
#define GFX_PROGRESS    "progress.bin"
#define GFX_DONE        "done.bin"
#define GFX_FAILED      "failed.bin"

#ifdef NAND_SWITCH
#define LOGO_MSG ((use_emunand) ? "Current NAND (X): EmuNAND" : "Current NAND (X): SysNAND")
static bool use_emunand = false;
#else
#define LOGO_MSG NULL
#endif

void ProcessEntry(MenuEntry* entry)
{
    if (entry->isDangerous) { // warning graphic (if dangerous)
        DrawSplash(GFX_WARNING, 0);
        while (true) {
            u32 pad_state = InputWait();
            if (pad_state & BUTTON_B)
                return;
            else if (pad_state & BUTTON_A)
                break;
        }
    }
    
    // progress graphic
    DrawSplash(GFX_PROGRESS, 0);
    
    DebugSetTitle(entry->longTitle);
    DebugClear();
    #ifdef NAND_SWITCH
    SetNand(use_emunand);
    #endif
    if (entry->function() == 0) {
        Debug("%s: %s!", entry->shortTitle, "succeeded");
        DrawSplash(GFX_DONE, 0);
    } else {
        Debug("%s: %s!", entry->shortTitle, "failed");
        DrawSplash(GFX_FAILED, 0);
    }
    Debug("Press B to exit");
    while (!(InputWait() & BUTTON_B));
    DrawSplashLogo(LOGO_MSG);
}

u32 ProcessMenu(MenuInfo* info, u32 nMenus)
{
    MenuInfo* menu = info;
    u32 menu_idx = 0;
    u32 pad_state;
    
    DrawSplashLogo(LOGO_MSG);
    
    while(true) {
        // draw bottom graphics
        DrawSplash(menu->entries[menu_idx].gfxMenu, 0); // bottom
        
        pad_state = InputWait();
        if (pad_state & BUTTON_START) { 
            return 1;
        } else if (pad_state & BUTTON_SELECT) {
            return 2;
        } else if (pad_state & (BUTTON_DOWN) && menu_idx < 10) {
            if (menu->entries[menu_idx + 1].function != NULL)
                menu_idx++; // move down
        } else if (pad_state & (BUTTON_UP) && menu_idx != 0) {
            menu_idx--; // move up
        } else if (pad_state & (BUTTON_R1 | BUTTON_RIGHT)) {
            if (++menu - info >= nMenus) menu--; // next menu
            else menu_idx = 0;
        } else if (pad_state & (BUTTON_L1 | BUTTON_LEFT)) {
            if (--menu < info) menu = info; // previous menu
            else menu_idx = 0;
        } else if (pad_state & BUTTON_A) { // process action
            ProcessEntry(&(menu->entries[menu_idx]));
        }
        #ifdef NAND_SWITCH
        else if (pad_state & BUTTON_X) { // switch NAND
            use_emunand = !use_emunand;
            DrawSplashLogo(LOGO_MSG);
        }
        #endif
    }
    
    return 0;
}
