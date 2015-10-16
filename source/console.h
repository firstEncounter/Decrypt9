#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_SPACING 2
#define CONSOLE_BORDER_WIDTH 2
#define CONSOLE_WIDTH (SCREEN_WIDTH - CONSOLE_BORDER_WIDTH)
#define CONSOLE_HEIGHT (SCREEN_HEIGHT - CONSOLE_BORDER_WIDTH)
#define CHAR_WIDTH 10
#define MAX_LINES (int) (CONSOLE_HEIGHT / CHAR_WIDTH - 4)
#define MAX_LINE_LENGTH (int) (CONSOLE_WIDTH / 8 - 3)
#define CONSOLE_X 0 // (SCREEN_WIDTH - CONSOLE_WIDTH) / 2
#define CONSOLE_Y 0 // (SCREEN_HEIGHT - CONSOLE_HEIGHT) / 2

void ConsoleInit();
void ConsoleShow();
void ConsoleFlush();
void ConsoleAddText(const char* str);
void ConsoleSetBackgroundColor(int color);
int ConsoleGetBackgroundColor();
void ConsoleSetBorderColor(int color);
int ConsoleGetBorderColor();
void ConsoleSetTextColor(int color);
int ConsoleGetTextColor();
void ConsoleSetXY(int x, int y);
void ConsoleGetXY(int *x, int *y);
void ConsoleSetWH(int width, int height);
void ConsoleSetTitle(const char *format, ...);
void ConsoleSetBorderWidth(int width);
int ConsoleGetBorderWidth(int width);
void ConsoleSetSpecialColor(int color);
int ConsoleGetSpecialColor();
void ConsoleSetSpacing(int space);
int ConsoleGetSpacing();
#endif