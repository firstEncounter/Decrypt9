// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "fs.h"
#include "font.h"
#include "draw.h"
#include "console.h"

#define BG_COLOR   (RGB(0x00, 0x00, 0x00))
#define BG_M_COLOR (RGB(0x42, 0x22, 0x58))
#define FONT_COLOR (RGB(0xFF, 0xFF, 0xFF))


void ClearScreen(u8* screen, int color)
{
    for (int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); i++) {
        *(screen++) = color >> 16;  // B
        *(screen++) = color >> 8;   // G
        *(screen++) = color & 0xFF; // R
    }
}

void DrawPixel(int x, int y, int color, int screen){
    if(color != TRANSPARENT){
        int coord = (SCREEN_HEIGHT * BYTES_PER_PIXEL * x) + (SCREEN_HEIGHT - y) * BYTES_PER_PIXEL;
        u8* screenPos = (u8*) screen + coord;
        *(screenPos + 0) = (color >> 16) & 0xFF;
        *(screenPos + 1) = (color >>  8) & 0xFF;
        *(screenPos + 2) = (color >>  0) & 0xFF;
    }
}

void DrawCharacter(u8* screen, int character, int x, int y, int color, int bgcolor)
{
    for (int yy = 0; yy < 8; yy++) {
        int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
        int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
        u8* screenPos = screen + xDisplacement + yDisplacement;

        u8 charPos = font[character * 8 + yy];
        for (int xx = 7; xx >= 0; xx--) {
            if ((charPos >> xx) & 1) {
                *(screenPos + 0) = color >> 16;  // B
                *(screenPos + 1) = color >> 8;   // G
                *(screenPos + 2) = color & 0xFF; // R
            } else {
                *(screenPos + 0) = bgcolor >> 16;  // B
                *(screenPos + 1) = bgcolor >> 8;   // G
                *(screenPos + 2) = bgcolor & 0xFF; // R
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
        }
    }
}

void DrawString(u8* screen, const char *str, int x, int y, int color, int bgcolor)
{
    for (int i = 0; i < strlen(str); i++)
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringF(int x, int y, int color, int bgcolor, const char *format, ...) //Percentage display background
{
    char str[256] = {};
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, x, y, color, bgcolor);
    DrawString(TOP_SCREEN1, str, x, y, color, bgcolor);
}

void DebugInit()
{
    ConsoleSetBorderColor(PURPLE);
    ConsoleSetTextColor(WHITE);
    ConsoleSetBackgroundColor(BLACK);
}

void DebugSetTitle(const char* title)
{
    ConsoleSetTitle(title);
}

void DebugClear()
{
    ClearScreen(TOP_SCREEN0, BG_COLOR);
    ClearScreen(TOP_SCREEN1, BG_COLOR);
    ConsoleInit();
    ConsoleShow();
}

void Debug(const char *format, ...)
{
    char str[64] = {};
    va_list va;

    va_start(va, format);
    vsnprintf(str, 64, format, va);
    va_end(va);
    
    ConsoleAddText(str);
    ConsoleShow();
}

void ShowProgress(u32 current, u32 total)
{
    if (total > 0)
        DrawStringF(SCREEN_WIDTH - 40, SCREEN_HEIGHT - 20, FONT_COLOR, BG_COLOR, "%3i%%", current / (total/100));
    else
        DrawStringF(SCREEN_WIDTH - 40, SCREEN_HEIGHT - 20, FONT_COLOR, BG_COLOR, "    ");
}

void DrawSplash(const char* splash_file, u32 use_top_screen) {
    char path[256];
    
    snprintf(path, 256, "/3ds/%s/UI/%s", APP_TITLE, splash_file);
    if (!FileOpen(path)) {
        snprintf(path, 256, "/D9UI/%s", splash_file);
        if (!FileOpen(path))
            return;
    }
    if (use_top_screen) { // Load the spash image - top screen
        FileRead(TOP_SCREEN0, 400 * 240 * 3, 0);
        memcpy(TOP_SCREEN1, TOP_SCREEN0, 400 * 240 * 3);
    } else { // Load the spash image - bottom screen
        FileRead(BOT_SCREEN0, 320 * 240 * 3, 0);
        memcpy(BOT_SCREEN1, BOT_SCREEN0, 320 * 240 * 3);
    }
    FileClose();
}

void DrawSplashLogo(const char* msg)
{
    DrawSplash("menuTOP.bin", 1); // top screen
    if (msg != NULL)
        // adapt this and delete this comment
        DrawStringF(50, 200, FONT_COLOR, BG_M_COLOR, "%s", msg);
    #ifdef WORKDIR
    DrawStringF(50, 210, FONT_COLOR, BG_M_COLOR, "Working directory: %s", WORKDIR);
    #endif
    DrawStringF(50, 220, FONT_COLOR, BG_M_COLOR, "Remaining SD storage space: %llu MiB", RemainingStorageSpace() / 1024 / 1024);
}
