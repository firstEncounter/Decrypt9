// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "common.h"

#define BYTES_PER_PIXEL 3
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 400

#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_HEIGHT * SCREEN_WIDTH)

#define RGB(r,g,b) (r<<24|b<<16|g<<8|r)

#ifdef EXEC_GATEWAY
    #define TOP_SCREEN0 (u8*)(*(u32*)((uint32_t)0x080FFFC0 + 4 * (*(u32*)0x080FFFD8 & 1)))
    #define BOT_SCREEN0 (u8*)(*(u32*)0x080FFFD4)
    #define TOP_SCREEN1 TOP_SCREEN0
    #define BOT_SCREEN1 BOT_SCREEN0
#elif defined(EXEC_BOOTSTRAP)
    #define TOP_SCREEN0 (u8*)(0x20000000)
    #define TOP_SCREEN1 (u8*)(0x20046500)
    #define BOT_SCREEN0 (u8*)(0x2008CA00)
    #define BOT_SCREEN1 (u8*)(0x200C4E00)
#else
    #error "Unknown execution method"
#endif

//Colors Macros
#define BLACK       RGB(0x00, 0x00, 0x00)
#define WHITE       RGB(0xFF, 0xFF, 0xFF)
#define RED         RGB(0xFF, 0x00, 0x00)
#define GREEN       RGB(0x00, 0xFF, 0x00)
#define BLUE        RGB(0xFF, 0x00, 0xFF)
#define GREY        RGB(0x77, 0x77, 0x77)
#define TRANSPARENT RGB(0xFF, 0x00, 0xFF)
#define PURPLE      RGB(0x66, 0x00, 0xFF)

void ClearScreen(unsigned char *screen, int color);
void DrawPixel(int x, int y, int color, int screen);
void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor);

void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor);
void DrawStringF(int x, int y, int color, int bgcolor, const char *format, ...);

void DebugInit();
void DebugSetTitle(const char* title);
void DebugClear();
void Debug(const char *format, ...);

void ShowProgress(u32 current, u32 total);

void DrawSplash(const char* splash_file, u32 use_top_screen);
void DrawSplashLogo(const char* msg);
