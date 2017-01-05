#pragma once

typedef unsigned char u8;
typedef unsigned u32;
typedef u32 Color;

// allocates memory for framebuffer and initializes it
void psvDebugScreenInit();

// clears screen with a given color
void psvDebugScreenClear(int bg_color);

// printf to the screen
void psvDebugScreenPrintf(const char *format, ...);

// set foreground (text) color
Color psvDebugScreenSetFgColor(Color color);

// set background color
Color psvDebugScreenSetBgColor(Color color);

// printf to the screen with color
void print_color(const char *text, Color color);

void *psvDebugScreenGetVram();
int psvDebugScreenGetX();
int psvDebugScreenGetY();
void psvDebugScreenSetXY();

enum {
	RED     = 0xFF0000FF,
	GREEN   = 0xFF00FF00,
	BLUE    = 0xFFFF0000,
	
	CYAN    = 0xFFFFFF00,
	MAGENTA = 0xFFFF00FF,
	YELLOW  = 0xFF00FFFF,	
	
	AZURE	= 0xFFFF7F00,
	VIOLET	= 0xFFFF007F,
	ORANGE	= 0xFF007FFF,
	
	GREY    = 0xFF808080,
	WHITE   = 0xFFFFFFFF,
	BLACK   = 0xFF000000,
};
