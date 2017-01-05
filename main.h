#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>					//for button input

#include <vita2d.h>
#include <psp2/display.h>				//for sceDisplayWaitVblankStart();

#include "graphics.h"					//for debug text


#define printf psvDebugScreenPrintf

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

#define TEXT_SIZE_BIG 1.5f
#define TEXT_SIZE 1.0f

#define TITLE "Adrenaline EasyInstaller"
#define VERSION "1.03"


SceCtrlData pad;
uint32_t old_buttons, current_buttons, pressed_buttons, hold_buttons, hold2_buttons, released_buttons;

extern vita2d_pgf *pgf;

extern vita2d_texture *button_enter;
extern vita2d_texture *button_cancel;
extern vita2d_texture *button_pad;


int debug = 0;
int gamesfound = 0;


/// Color definitions //////////////////////////////////////////
uint32_t text_color = WHITE;
uint32_t background_color = BLACK;
uint32_t menu_active_color = RGBA8(0, 120, 255, 255); //BLUE
uint32_t menu_text_active_color = BLACK;
uint32_t menu_text_inactive_color = GREY;


/// Options Menu Stuff //////////////////////////////////////////		
enum {
	MENU_DUMMY,
	MENU_ACTIVE, 
	MENU_BLOCKED,	
	MENU_EXIT,
};

typedef struct {
	char *title;
	int type;
	void *function;
	char *message;
} Menu;

	

/// Functions /////////////////////////////////////////////////////////////////	
void readPad();
void scePowerRequestColdReset();
	
int system_check();
void draw_recovery_menu();

void *draw_psp_games();
void *uninstall_adrenaline();
void *install_theme();
void *uninstall_theme();

int drawdialog(const char* message);

void install_adrenaline_files(char *id);
void uninstall_adrenaline_files(char *path);

void install_theme_files(char *id);
void uninstall_theme_files(char *id);

#endif