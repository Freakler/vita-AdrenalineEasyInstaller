#ifndef __FILE_H__
#define __FILE_H__

#include <psp2/io/dirent.h> 
#include <psp2/io/fcntl.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

#define printf psvDebugScreenPrintf

#define TRANSFER_SIZE 64 * 1024 //64kb
#define MAX_PATH_LENGTH 1024


char PSP_GAME_ID[32];

struct PSPContent {
	char titleID[16]; 	//TITLE_ID
	char title[128];	//TITLE
};
struct PSPContent content_array[256]; //array with infos


char *getEbootTitle(char *src_path);

int doesFileExist(const char* path);
int doesDirExist(const char* path);

int copyFile(char *src_path, char *dst_path);

int makePath(const char *dir);
int removePath(char *path);

int download_file(const char *src, const char *dst);

const char *get_id_of_psp_game_that_adrenaline_is_installed_to();
int write_adrenaline_to_config(char *id);
int delete_adrenaline_from_config(char *id);
int check_for_psp_content(char *path);

void trigger_update_database();
void trigger_rebuild_database();

int writeChangeinfo(const char* id);
#endif