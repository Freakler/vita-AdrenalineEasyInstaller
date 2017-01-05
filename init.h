#ifndef __INIT_H__
#define __INIT_H__

#include <psp2/types.h> 		//SceINT etc

#include <psp2/apputil.h>
#include <psp2/appmgr.h>
#include <psp2/system_param.h>

#include <psp2/ctrl.h> 			//buttons
#include <stdio.h>
#include <string.h>				//memset
#include <stdlib.h>

#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>		//for load net module

#include <vita2d.h>

extern int language;
extern char username;
extern int SCE_CTRL_ENTER, SCE_CTRL_CANCEL;

void netInit();
void httpInit();

void initSceAppUtil();
void finishSceAppUtil();

void initVita2DLib();
void closeVita2DLib();

int readInEnterButton();
char *getUsername();
int getSystemLanguageCode();

#endif