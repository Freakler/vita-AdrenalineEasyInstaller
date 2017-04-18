#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>					//for button input
#include <psp2/shellutil.h>				//for blocking home button

#include <vita2d.h>
#include <psp2/display.h>				//for sceDisplayWaitVblankStart();

#include "graphics.h"					//for debug text

	#include <psp2/promoterutil.h> 
	
#define printf psvDebugScreenPrintf

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

#define TEXT_SIZE_BIG 1.5f
#define TEXT_SIZE 1.0f


#define MAIN_TITLE "Adrenaline EasyInstaller"
#define TITLEID "ADRINSTAL"
#define VERSION "1.10"

#define DEFAULT_BASEGAME "NPEZ00104" //coming with integrated installer

#define UPDATE_LINK "http://du01.psp.update.playstation.org/update/psp/image/us/2014_1212_6be8878f475ac5b1a499b95ab2f7d301/EBOOT.PBP"
#define UPDATE_SIZE 32580549

#define LIVEAREA_frame2_text "6.61 Adrenaline"
#define LIVEAREA_frame3_text "eCustomFirmware"
#define LIVEAREA_frame4_text "by TheFloW"

SceCtrlData pad;
uint32_t old_buttons, current_buttons, pressed_buttons, hold_buttons, hold2_buttons, released_buttons;

extern vita2d_pgf *pgf;

extern vita2d_texture *button_enter;
extern vita2d_texture *button_cancel;
extern vita2d_texture *button_pad;

int debug = 0;	//toggle debug mode here

int ADR_PRX_INSTL = 0;
int ADR_INSTALLED = 0;
char ADR_FOLDER[128] = "ux0:adrenaline";
char ADR_VERSION[128] = "unknown";


int gamesfound = 0;
char buffer[256];
char big_buffer[1024];


/// Color definitions //////////////////////////////////////////
uint32_t text_color = WHITE;
uint32_t background_color = BLACK;
uint32_t menu_active_color = RGBA8(0, 120, 255, 255); //BLUE
uint32_t menu_text_active_color = BLACK;
uint32_t menu_text_inactive_color = GREY;
uint32_t menu_text_basegame = GREEN;
uint32_t menu_scrollbar = RGBA8(0, 120, 255, 255); //BLUE;
uint32_t menu_scrollbar_background = GREY;


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
	char *arg;
	char *arg2;
	char *message;
} Menu;

	

/// Functions /////////////////////////////////////////////////////////////////	
int taiReloadConfig();
int vshSblAimgrIsVITA();
void scePowerRequestColdReset();

void readPad();
void lock_psbutton();
void unlock_psbutton();

//int delete_old_adrenaline();
int option_test();
	
int system_check();

void recovery_draw_main();

	//recovery_menu
	void *draw_psp_games();
	void *installer_options(char *arg);
	void *files_option(char *arg);
	void *uninstall_adrenaline();
	void *theme_options(char *arg);
	void *more_options(char *arg);

	
void recovery_draw_submenu(Menu *sub_menu, const char* title, char *arg);

	//theme_menu
	void *install_theme();
	void *uninstall_theme();
	void *install_sqlite_theme();
	void *uninstall_sqlite_theme();
	
	//more_menu
	void *install_pspgame();
	int *option_delete_pbp();
	int *option_delete_savestates();
	int *option_delete_flash();
	int *option_show_changelogfile();
	int *option_show_taiconfig();
	int *option_reloadTaiConfig();
	int *option_rebuildDatabase();
	int *option_updateDatabase();
	int *option_reboot();	
	

int recovery_draw_dialog(const char* title, const char* message, const char* message2);
int recovery_draw_message(const char* title, const char* message, const char* button_text);
int recovery_draw_statusmessage(const char* title, const char* message);
void recovery_draw_textfile(const char* title, const char* path);

void install_psp_basegame();
void createMS0structure();

void install_adrenaline_files(char *id, char *arg);
void change_adrenaline_files(char *id, char *arg, char *arg2, int mode);
void uninstall_adrenaline_files(char *path);

void install_theme_files(char *id);
void uninstall_theme_files(char *id);

int add_livearea_style(char *id);
int remove_livearea_style(char *id);

#endif