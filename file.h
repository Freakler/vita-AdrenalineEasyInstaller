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

#define TRANSFER_SIZE 512 * 1024
#define MAX_PATH_LENGTH 1024

char PSP_GAME_ID[32];
char array_buffer[256][16];


int doesFileExist(const char* path);
int doesDirExist(const char* path);
int copyFile(char *src_path, char *dst_path);

int download_file(const char *src, const char *dst);

const char *get_id_of_psp_game_that_adrenaline_is_installed_to();
int write_adrenaline_to_config(char *id);
int delete_adrenaline_from_config(char *id);
int check_for_psp_content(char *path);

#endif