#include "init.h"

extern unsigned char _binary_materials_button_pad_png_start;
extern unsigned char _binary_materials_button_cross_png_start;
extern unsigned char _binary_materials_button_circle_png_start;

vita2d_pgf *pgf = NULL;

vita2d_texture *button_pad = NULL;
vita2d_texture *button_enter = NULL;
vita2d_texture *button_cancel = NULL;

int SCE_CTRL_ENTER = SCE_CTRL_CROSS;
int SCE_CTRL_CANCEL = SCE_CTRL_CIRCLE;

/// Vita2DLib /////////////////////////////////////////////////////////////////////////////////////////////

void initVita2DLib() {
	vita2d_init();
	pgf = vita2d_load_default_pgf();
	
	button_pad = vita2d_load_PNG_buffer(&_binary_materials_button_pad_png_start);
	
	if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
		button_enter = vita2d_load_PNG_buffer(&_binary_materials_button_cross_png_start);
		button_cancel = vita2d_load_PNG_buffer(&_binary_materials_button_circle_png_start);
	} else {
		button_enter = vita2d_load_PNG_buffer(&_binary_materials_button_circle_png_start);
		button_cancel = vita2d_load_PNG_buffer(&_binary_materials_button_cross_png_start);
	}	
}


void closeVita2DLib() {
	vita2d_free_texture(button_pad);
	vita2d_free_texture(button_enter);
	vita2d_free_texture(button_cancel);
	
	vita2d_free_pgf(pgf);
	vita2d_fini();
}




/// NET /////////////////////////////////////////////////////////////////////////////////////////////

void netInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	
	SceNetInitParam netInitParam;
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	sceNetCtlInit();
}

void httpInit() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
	sceHttpInit(1*1024*1024);
}




/// SceAppUtil /////////////////////////////////////////////////////////////////////////////////////////////

void initSceAppUtil() {
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;
	memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);

}

void finishSceAppUtil() {
	sceAppUtilShutdown();
}




int readInEnterButton() {
	int enter_button = 0;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, &enter_button);
	
	if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE) { //switch butons
		SCE_CTRL_ENTER = SCE_CTRL_CIRCLE;
		SCE_CTRL_CANCEL = SCE_CTRL_CROSS;
	} 
	
	
	return enter_button;
}

char *getUsername() {
	static SceChar8 username[SCE_SYSTEM_PARAM_USERNAME_MAXSIZE];
	sceAppUtilSystemParamGetString(SCE_SYSTEM_PARAM_ID_USERNAME, username, SCE_SYSTEM_PARAM_USERNAME_MAXSIZE);
	
	return (char *)username;
}

int getSystemLanguageCode() {
	int systemlanguage = 0;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &systemlanguage);
	
	return systemlanguage;
}