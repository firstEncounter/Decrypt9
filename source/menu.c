#include "draw.h"
#include "hid.h"
#include "menu.h"
#include "decryptor/features.h"

#define GFX_WARNING     "warning.bin"
#define GFX_PROGRESS    "progress.bin"
#define GFX_DONE        "done.bin"
#define GFX_FAILED      "failed.bin"


void ProcessEntry(MenuEntry* entry)
{
    
    DebugSetTitle(entry->longTitle); // set console title
    if (entry->isDangerous) { // warning graphic (if dangerous)
        u32 unlockSequence[] = { BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_UP, BUTTON_A };
        u32 unlockLvlMax = sizeof(unlockSequence) / sizeof(u32);
        u32 unlockLvl = 0;
        DrawSplash(GFX_WARNING, 0);
        DebugClear();
        Debug("You selected \"%s\".", entry->shortTitle);
        Debug("This feature is potentially dangerous!");
        Debug("If you understand and wish to proceed, enter:");
        Debug("<Left>, <Right>, <Down>, <Up>, <A>");
        Debug("");
        Debug("(B to return, START to reboot)");
        while (true) {
            ShowProgress(unlockLvl, unlockLvlMax);
            if (unlockLvl == unlockLvlMax)
                break;
            u32 pad_state = InputWait();
            if (!(pad_state & BUTTON_ANY))
                continue;
            else if (pad_state & unlockSequence[unlockLvl])
                unlockLvl++;
            else if (pad_state & (BUTTON_B | BUTTON_SELECT | BUTTON_START))
                break;
            else if (unlockLvl == 0 || !(pad_state & unlockSequence[unlockLvl-1]))
                unlockLvl = 0;
        }
        ShowProgress(0, 0);
        if (unlockLvl < unlockLvlMax) {
            DrawSplashLogo();
            return;
        }
    }
    
    // progress graphic
    DrawSplash(GFX_PROGRESS, 0);
    
    DebugClear();
    if ((SetNand(entry->emunand) == 0) && (entry->function() == 0)) {
        Debug("%s: %s!", entry->shortTitle, "succeeded");
        DrawSplash(GFX_DONE, 0);
    } else {
        Debug("%s: %s!", entry->shortTitle, "failed");
        DrawSplash(GFX_FAILED, 0);
    }
    Debug("Press B to exit");
    while (!(InputWait() & BUTTON_B));
    DrawSplashLogo();
}

u32 ProcessMenu(MenuInfo* info, u32 nMenus)
{
    MenuInfo* menu = info;
    u32 menu_idx = 0;
    u32 pad_state;
    
    DrawSplashLogo();
    
    while(true) {
        // draw bottom graphics
        DrawSplash(menu->entries[menu_idx].gfxMenu, 0); // bottom
        
        pad_state = InputWait();
        if (pad_state & BUTTON_START) { 
            return 1;
        } else if (pad_state & BUTTON_SELECT) {
            return 2;
        } else if (pad_state & BUTTON_DOWN) {
            if ((menu_idx < 9 ) && (menu->entries[menu_idx + 1].function != NULL))
                menu_idx++; // move down
            else menu_idx = 0; // wrap around
        } else if (pad_state & BUTTON_UP) {
            if (menu_idx != 0)
                menu_idx--; // move up
            else { // wrap around
                for (menu_idx = 9; menu->entries[menu_idx].function == NULL; menu_idx--);
            }
        } else if (pad_state & (BUTTON_R1 | BUTTON_RIGHT)) {
            if (++menu - info >= nMenus) menu = info; // next menu
            else menu_idx = 0;
        } else if (pad_state & (BUTTON_L1 | BUTTON_LEFT)) {
            if (--menu < info) menu = info + nMenus - 1; // previous menu
            else menu_idx = 0;
        } else if (pad_state & BUTTON_A) { // process action
            ProcessEntry(&(menu->entries[menu_idx]));
        }
    }
    
    return 0;
}
