#include "main.h"
#include "init.h"
#include "file.h"


/* Notes & TODO
* - add sqlite method for the theme maybe?!
* - block PS button on install maybe?!
* - use actual game name instead of GAME_ID
* - add progress indicator to 661 download
* - add Option to delete installed flash maybe?!
* - 
*
* "Ultra-Secret-Debug-Info-Mesages"-combo is: LEFT + L-TRIGGER + R-TRIGGER + START 
*/ 

/** Changelog 
* v1.03
* - merged additional checks by Yoti
* - added automatic 661.PBP downloading
* - livearea template adjustments
* - adjusted debug text length for theme installing
* - code cleaned & bugs fixed
* -
*
* v1.02
* - bug fixes
*/




/// Main Menu /////////////
Menu recovery_menu[] = {
	//*title						//type				//*function					//*message
	{"1. Install Adrenaline"		, MENU_BLOCKED		, draw_psp_games			, "* Install 6.61 Adrenaline eCFW on this device" 						},
	{"2. Uninstall Adrenaline"		, MENU_BLOCKED		, uninstall_adrenaline		, "* Remove Adrenaline from this device" 								},
	{"3. Add Custom LiveArea Theme"	, MENU_BLOCKED		, install_theme				, "* Add a custom LiveArea Theme (optional)"							},
	{"4. Restore LiveArea Theme"	, MENU_BLOCKED		, uninstall_theme			, "* Remove the custom LiveArea Theme and restore the default look"		},
	{"5. Exit"						, MENU_EXIT 		, NULL						, "* Exit EasyInstaller" 												},
	{NULL,0,"",""}
};


	
int system_check() {
	
	psvDebugScreenInit();
	
	printf("EasyInstaller v%s\n", VERSION);
	printf("-------------------\n\n\n");
	
	/// Enter Button check
	printf("Enter Button = ");
	if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) printf("X\n\n");
	else printf("O\n\n");		
	
	/// taiHENkaku check
	printf("Checking for taiHENkaku.. ");
	if ( doesFileExist("ux0:tai/config.txt") ) {
		print_color("ux0:tai/config.txt found!!\n\n", GREEN);
		
	} else {
		print_color("Nothing found!!\n\n\n", RED);
		print_color("\n!!! Either you aren't running taiHENkaku, or you haven't enabled unsafe Homebrews in molecularShell yet!!!\n\n", RED);
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			printf("Press X to exit..\n\n");
		} else {
			printf("Press O to exit..\n\n");
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) 
				return 0;
		}
	}

	/// checking for activation
	printf("Checking for act.dat file.. ");
	if (doesFileExist("tm0:npdrm/act.dat"))
		print_color("OK\n\n", GREEN);
	else {
		print_color("Not found!!\n\n", RED);
		print_color("\n!!! You must activate your Vita first!!!\n\n", RED);

		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS)
			printf("Press X to exit...\n\n");
		else
			printf("Press O to exit...\n\n");

	
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) 
				return 0;
		}
	}

	/// installed PSP Content
	printf("Checking for PSP content.. ");	
	gamesfound = check_for_psp_content("ux0:pspemu/PSP/GAME/");
	if ( gamesfound > 0 ) { 
		print_color("Found!! ", GREEN);
		printf("(%i Games)\n\n", gamesfound );
		
	} else {
		print_color("No content!!\n\n", RED);
		print_color("\n!!! Please install any PSP Game/Demo first !!!\n\n", RED);
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			printf("Press X to exit..\n\n");
		} else {
			printf("Press O to exit..\n\n");
		}
		
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) 
				return 0;
		}
	}
	
	
	
	/// Adrenaline files	
	printf("Checking for Adrenaline files.. ");	
	if ( doesFileExist("ux0:pspemu/adrenaline/adrenaline.skprx") ) {
		print_color("Found!!\n\n", GREEN);	
		
		/// Adrenaline installed to PSP game check??	
		printf("Checking ux0:tai/config.txt for adrenaline.. ");
		printf("%s \n\n", get_id_of_psp_game_that_adrenaline_is_installed_to());
		
	} else {
		print_color("Not found!!\n\n", YELLOW);
	}
	

	printf("\n\nDone! Starting..");
	//sceKernelDelayThread(3000 * 1000); // 3 sec delay
	
	return 1;
}

	
void draw_recovery_menu() {

	int options = 0;		//available options
	int selection = 0;		//selected option
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
	char buffer[256];
	
	
		/// calculate number of options
		while(recovery_menu[ options ].title != NULL) options++;
		
		/// Error handling
		if ( showoptions > options ) showoptions = options; //Adjust showoptions (there can't be more "showoptions" than "options" itself)
		
		/// Add checks here to ACTIVATE or BLOCK options if needed
		if ( PSP_GAME_ID[0] == '\0' && check_for_psp_content("ux0:pspemu/PSP/GAME/") > 0 ) {
			recovery_menu[0].type = MENU_ACTIVE; //activate installing option
		} else {
			recovery_menu[1].type = MENU_ACTIVE; //activate uninstalling option
			recovery_menu[2].type = MENU_ACTIVE; //activate Theme Installing
			
			sprintf(buffer, "ur0:appmeta/%s/livearea/contents/bg0_.png", PSP_GAME_ID);
			if ( doesFileExist(buffer) ) {
				recovery_menu[3].type = MENU_ACTIVE; //activate Theme uninstalling
				recovery_menu[2].type = MENU_BLOCKED; //deactivate Theme Installing again
			}	
		}
		
		/// set marker to first active option
		while(recovery_menu[ selection ].type == MENU_BLOCKED) selection += 1;
		
		
	
	while (running) {
	
		readPad();
		
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		/// "Debug" messages
		if ( debug == 1 ) {
			vita2d_pgf_draw_textf(pgf, 620, 100, GREY, TEXT_SIZE, "PSP_GAME_ID = %s", PSP_GAME_ID);
			
			vita2d_pgf_draw_textf(pgf, 640, 125, GREY, 1.0f, "options = %i", options);
			vita2d_pgf_draw_textf(pgf, 640, 150, GREY, 1.0f, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 640, 175, GREY, 1.0f, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 640, 200, GREY, 1.0f, "showoptions = %i", showoptions);
			
			vita2d_pgf_draw_textf(pgf, 620, 250, GREY, TEXT_SIZE, "gamesfound = %i", gamesfound);
			i = 0;
			while ( i < gamesfound ) {
				vita2d_pgf_draw_textf(pgf, 620, 300+i*25, GREY, TEXT_SIZE, "array_buffer[%i] = %s", i, array_buffer[i]);
				i++;
			}
			
			//vita2d_draw_line(0,  70, 960,  70, GREY); //top line
			//vita2d_draw_line(0, 410, 960, 410, GREY); //bot line
			//vita2d_draw_line(100, 0, 100, 544, GREY); //left line
			//vita2d_draw_line(860, 0, 860, 544, GREY); //right line
		}
		
		/// draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2), 40, text_color, TEXT_SIZE_BIG, TITLE);
		
		/// draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	

		
		if ( selection < toption ) toption = selection;	//scrolling up with selection
		
		for(i=toption; i < showoptions+toption; i++, y += z) {
			if ( i == selection ) { //draw active selection
				vita2d_draw_rectangle(x-25, y-40, 760, 60, menu_active_color); //background
				vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s", recovery_menu[selection].title);
				vita2d_pgf_draw_textf(pgf, x, 465, text_color, TEXT_SIZE, "%s", recovery_menu[selection].message); //message
			} else { //draw all other inactive options
				if ( recovery_menu[i].type == MENU_BLOCKED ) { //when blocked adjust color
					vita2d_pgf_draw_textf(pgf, x, y, menu_text_inactive_color, TEXT_SIZE_BIG, "%s", recovery_menu[i].title);	
				} else { //none-blocked
					vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE_BIG, "%s", recovery_menu[i].title);	
				}	
			}
		}
		
		/// Button Controls 
		x = 15;  //horizontal start
		y = 525; //vertical start
		z = 25;  //spacing
		vita2d_draw_texture(button_enter, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Enter");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Enter")+z;
		vita2d_draw_texture(button_pad, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26, y, WHITE, TEXT_SIZE, "Navigate");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Navigate")+z;

		
		
		/// Button Input //////////////////////////////////////////////////////////////////////////////////
		
			/// Navigation UP
			if ( hold_buttons & SCE_CTRL_UP) {	
				
				if ( selection <= 0 ) {
					selection = options-1;
					toption = options-showoptions; //scroll
				} else selection -= 1;					
				
				while ( recovery_menu[selection].type == MENU_BLOCKED ) {
					if ( selection <= 0 ) {
					selection = options-1;
					toption = options-showoptions; //scroll
					} else selection -= 1;		
					
					if ( toption > 1 ) { //scroll
						if ( selection == 1 ) toption--; 
					}
				}
				if ( toption > 1 ) { //scroll
					if ( selection == 1 ) toption--; 
				}
			
			} 	
		
			/// Navigation DOWN
			if ( hold_buttons & SCE_CTRL_DOWN) {			
				
				if ( showoptions == 1 ) { //handling 1 option only

					if ( selection >= options-1 ) selection = 0;
					else selection += 1;
				
					while ( recovery_menu[selection].type == MENU_BLOCKED ) {			
						if ( selection >= options-1 ) selection = 0;
						else selection += 1;
					}
					toption = selection;
					
				} else {
				
					if ( selection >= options-1 ) selection = 0;
					else {
						selection += 1;
						
						if ( toption+showoptions < options ) { //scroll
							if ( selection == toption+showoptions-1 ) toption++; 
						}
					}	
				
					while ( recovery_menu[selection].type == MENU_BLOCKED ) {					
						if ( selection >= options-1 ) selection = 0;
						else {
							selection += 1;
							
							if ( toption+showoptions < options ) { //scroll
								if ( selection == toption+showoptions-1 ) toption++; 
							}
						}	
					}	
					
					if ( toption+showoptions < options ) { //scroll
						if ( selection == toption+showoptions-1 ) toption++; 
					}
				}		
			}
			
		/// "Ultra-Secret-Debug-Info" Combo
		if (hold_buttons & SCE_CTRL_LEFT && hold_buttons & SCE_CTRL_LTRIGGER && hold_buttons & SCE_CTRL_RTRIGGER && hold_buttons & SCE_CTRL_START) {
			if ( debug == 0 ) debug = 1;	
			else debug = 0;
		}	
		
		/// Exit Combo
		if (pressed_buttons & SCE_CTRL_SELECT && pressed_buttons & SCE_CTRL_START) break;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		void (* function)();
		
		if ( recovery_menu[selection].type == MENU_ACTIVE ) {
		
			if (pressed_buttons & SCE_CTRL_ENTER) {
				
				vita2d_end_drawing();
				vita2d_swap_buffers();
				sceDisplayWaitVblankStart();
				
					function = (void *)(recovery_menu[selection].function);
					function();
				
				vita2d_start_drawing();
				vita2d_clear_screen();
		
				
			}	
		
		} else if ( recovery_menu[selection].type == MENU_EXIT ) {
			
			if (pressed_buttons & SCE_CTRL_ENTER) running = 0;
			
		} else {	
			//Error
			///vita2d_pgf_draw_text(pgf, 350,  470, RED, 2.0f, "(null)");
		}
		
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
}






void *uninstall_adrenaline() {
	
	int ret = drawdialog("Do you really want to continue uninstalling Adrenaline?");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		uninstall_adrenaline_files(PSP_GAME_ID);
	}
	
	return 0;
}

void *install_theme() {
	
	int ret = drawdialog("Do you really want to continue installing the theme?");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		install_theme_files(PSP_GAME_ID);
	}
	
	return 0;
}

void *uninstall_theme() {
	
	int ret = drawdialog("Do you really want to restore the original livearea theme?");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		uninstall_theme_files(PSP_GAME_ID);
	}
	
	return 0;
}


void *draw_psp_games() {

	int options = gamesfound;	
	int selection = 0;		//selected option number
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
		
		if ( showoptions > options ) showoptions = options; //error adjust showoptions (there can't be more "showoptions" than "options" itself)	
	
	while (running) {
	
		readPad();
		
		vita2d_start_drawing();
		vita2d_clear_screen();		
		
		
		/// Draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, "Select a basegame")/2), 40, text_color, TEXT_SIZE_BIG, "Select a basegame");
		
		
		/// "Debug" Messages
		if ( debug == 1 ) {
			vita2d_pgf_draw_textf(pgf, 600, 100, GREY, TEXT_SIZE, "gamesfound/options = %i", options);
			vita2d_pgf_draw_textf(pgf, 600, 125, GREY, TEXT_SIZE, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 600, 150, GREY, TEXT_SIZE, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 600, 175, GREY, TEXT_SIZE, "showoptions = %i", showoptions);
		}
		
		
		/// Draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	

		
		if ( selection < toption ) toption = selection;	//scrolling up with selection
		
		for(i=toption; i < showoptions+toption; i++, y += z) {
			if ( i == selection ) { //draw active selection
				vita2d_draw_rectangle(x-25, y-40, 760, 60, menu_active_color); //background
				vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s", array_buffer[selection]);
			} else { //draw all other options
				vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE_BIG, "%s", array_buffer[i]);	
			}
		}
		
		/// Button Controls
		x = 15; //horizontal start
		y = 525; //vertical start
		z = 25;  //spacing
		vita2d_draw_texture(button_enter, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Enter");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Enter")+z;
		vita2d_draw_texture(button_cancel, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Cancel");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Cancel")+z;	
		vita2d_draw_texture(button_pad, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26, y, WHITE, TEXT_SIZE, "Navigate");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Navigate")+z;

		
		
		/// Button Input //////////////////////////////////////////////////////////////////////////////////
			
			if ( hold_buttons & SCE_CTRL_UP) {	
				if ( selection <= 0 ) {
					selection = options-1;
					toption = options-showoptions; //scroll
				} else selection -= 1;					

				if ( toption > 1 ) { //scroll
					if ( selection == 1 ) toption--; 
				}
			} 	
		
			if ( hold_buttons & SCE_CTRL_DOWN) {
				if ( showoptions == 1 ) { //hadnling 1 option only

					if ( selection >= options-1 ) selection = 0;
					else selection += 1;
				
					toption = selection;
					
				} else {
				
					if ( selection >= options-1 ) selection = 0;
					else {
						selection += 1;
						
						if ( toption+showoptions < options ) { //scroll
							if ( selection == toption+showoptions-1 ) toption++; 
						}
					}	

					if ( toption+showoptions < options ) { //scroll
						if ( selection == toption+showoptions-1 ) toption++; 
					}
				}		
			}
		
		if ( pressed_buttons & SCE_CTRL_CANCEL) running = 0; //back
		
		
		if ( pressed_buttons & SCE_CTRL_ENTER) {  //install
			vita2d_end_drawing();
			vita2d_swap_buffers();
			sceDisplayWaitVblankStart();

			closeVita2DLib(); //shutdown Vita2dLib	
			install_adrenaline_files(array_buffer[selection]);
			
			return 0;
		}
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return 0;
}




int drawdialog(const char* message) {
	
	int running = 1;
	int choice = 0;
	int x, y, z;
		
	while (running) {
		readPad();
		
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2), 40, text_color, TEXT_SIZE_BIG, TITLE);
		
		
		/// Menu /////////////////////////////////////////////////////////////////////////////////////////////////////
		
			x = 310; //horizontal start of left button	//310 original
			y = 420; //vertical start of buttons		//450 original
			z = 290; //row spacing						//290
			
			vita2d_pgf_draw_textf(pgf, 960/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message)/2), 220, text_color, TEXT_SIZE_BIG, "%s", message); //draw Message //~180 in original menu
			
			if ( choice == 0 ) { //NO
				///left button
				vita2d_draw_rectangle(x-50, y-40, 150, 60, menu_active_color); //draw choice-background
				vita2d_pgf_draw_text(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "No");
				
				///right button
				vita2d_draw_rectangle(x+z-50, y-40, 150, 60, GREY); 
				vita2d_pgf_draw_text(pgf, x+z, y, text_color, TEXT_SIZE_BIG, "Yes");
				
			} else if ( choice == 1 ) { //YES
				///left button
				vita2d_draw_rectangle(x-50, y-40, 150, 60, GREY); 
				vita2d_pgf_draw_text(pgf, x, y, text_color, TEXT_SIZE_BIG, "No");
				
				///right button
				vita2d_draw_rectangle(x+z-50, y-40, 150, 60, menu_active_color); //draw choice-background
				vita2d_pgf_draw_text(pgf, x+z, y, menu_text_active_color, TEXT_SIZE_BIG, "Yes");
				
			} else {
				vita2d_pgf_draw_text(pgf, x, y+z, text_color, TEXT_SIZE_BIG, "ERROR");
			}
			
				
			
		/// Button Controls
		x = 15;  //horizontal start
		y = 525; //vertical start 
		z = 25;  //spacing
		vita2d_draw_texture(button_enter, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Enter");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Enter")+z;
		vita2d_draw_texture(button_cancel, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Cancel");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Cancel")+z;
		vita2d_draw_texture(button_pad, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26, y, WHITE, TEXT_SIZE, "Navigate");
			x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Navigate")+z;
			
		/// Navigation
			if ( pressed_buttons & SCE_CTRL_LEFT) choice = 0;			
			if ( pressed_buttons & SCE_CTRL_RIGHT) choice = 1;
			
			if ( pressed_buttons & SCE_CTRL_CANCEL) { //back
				running = 0;
				choice = 0;
			}	
			
			if ( pressed_buttons & SCE_CTRL_ENTER) { //Ok
				running = 0;
			}	
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	
	if ( choice == 1 ) return 1;
	else return 0;
}





void install_adrenaline_files(char *id) {

	char temp_buffer[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
		
	printf("Installing Adrenaline\n---------------------\n\n");	
	printf("installing to: %s\n\n\n", id);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
	
	/// Checking dir
	printf("Checking folder ux0:pspemu/adrenaline/.. ");
	ret = doesDirExist("ux0:pspemu/adrenaline");
	if ( ret == 1 )	print_color("Found!\n\n", GREEN);	
	else {	
		print_color("Not found!\n\n", YELLOW);	
		
		/// make dir
		printf("Creating folder ux0:pspemu/adrenaline/.. ");
		ret = sceIoMkdir("ux0:pspemu/adrenaline", 0777);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	}
	
	/// adrenaline.suprx
	printf("Writing ux0:pspemu/adrenaline/adrenaline.suprx.. ");
	ret = copyFile("app0:files/adrenaline.suprx", "ux0:pspemu/adrenaline/adrenaline.suprx");
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// adrenaline.skprx
	printf("Writing ux0:pspemu/adrenaline/adrenaline.skprx.. ");
	ret = copyFile("app0:files/adrenaline.skprx", "ux0:pspemu/adrenaline/adrenaline.skprx");
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// PBOOT.PBP
	sprintf(temp_buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
	printf("Writing %s.. ", temp_buffer);
	ret = copyFile("app0:files/PBOOT.PBP", temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// writing to tai config
	printf("Editing ux0:tai/config.txt.. ");
	ret = write_adrenaline_to_config(id);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
		
	/// 661.PBP
	printf("Checking for ux0:pspemu/adrenaline/661.PBP.. ");
	ret = doesFileExist("ux0:pspemu/adrenaline/661.PBP");
	if (ret == 1) print_color("OK\n\n", GREEN);
	else {
		print_color("Not found!\n\n", YELLOW);
				
		///download file 
		printf("Downloading 661.PBP.. ");
		ret = download_file("http://du01.psp.update.playstation.org/update/psp/image/us/2014_1212_6be8878f475ac5b1a499b95ab2f7d301/EBOOT.PBP", "ux0:pspemu/adrenaline/661.PBP");
		if (ret == 0) print_color("OK\n\n", GREEN);
		else {			
			print_color("ERROR!\n\n", RED);
				
			if ( doesFileExist("ux0:pspemu/adrenaline/661.PBP") ) sceIoRemove("ux0:pspemu/adrenaline/661.PBP"); //cleaning broken file
			
			print_color("\nPlace a 661.PBP to ux0:pspemu/adrenaline/661.PBP manually!\n\n", YELLOW);
		}			
	}
	
	if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nDone! Press X to reboot..\n", GREEN); //or O to exit normally..\n", GREEN);
	else print_color("\n\nDone! Press O to reboot..\n", GREEN); //or X to exit normally..\n", GREEN);

	while (1) {
		readPad();

		if (pressed_buttons & SCE_CTRL_ENTER) {
			sceIoRemove("ux0:id.dat"); //for triggering "updating database"
			scePowerRequestColdReset();
		}	
	}		
}

void uninstall_adrenaline_files(char *id) {

	char temp_buffer[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);
		
	printf("Uninstalling Adrenaline\n-----------------------\n\n");	
	printf("from: %s\n\n\n", id);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
	/// adrenaline.suprx
	printf("Deleting ux0:pspemu/adrenaline/adrenaline.suprx.. ");
	ret = sceIoRemove("ux0:pspemu/adrenaline/adrenaline.suprx");
	if ( ret == 0 )	print_color("OK\n\n", GREEN);
	else print_color("ERROR\n\n", RED);
	
	/// adrenaline.skprx
	printf("Deleting ux0:pspemu/adrenaline/adrenaline.skprx.. ");
	ret = sceIoRemove("ux0:pspemu/adrenaline/adrenaline.skprx");
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// PBOOT.PBP
	sprintf(temp_buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
	printf("Deleting %s.. ", temp_buffer);
	ret = sceIoRemove(temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// writing to tai config
	printf("Editing ux0:tai/config.txt.. ");
	ret = delete_adrenaline_from_config(id);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);
	else print_color("ERROR\n\n", RED);
	
	
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			print_color("\n\nDone! Press X to reboot..\n", GREEN); // or O to exit normally..\n", GREEN);
		} else {
			print_color("\n\nDone! Press O to reboot..\n", GREEN); // or X to exit normally..\n", GREEN);
		}
		
		while (1) {
			readPad();

			if (pressed_buttons & SCE_CTRL_ENTER) {
				sceIoRemove("ux0:id.dat"); //for triggering "updating database"
				scePowerRequestColdReset();
			}	
			if (pressed_buttons & SCE_CTRL_CANCEL) sceKernelExitProcess(0);
		}
		
}

void install_theme_files(char *id) {

	char temp_buffer[256];
	char temp_buffer2[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
		
	printf("Installing Adrenaline Theme\n---------------------------\n\n");	
	printf("installing to: %s\n\n\n", id);	
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
		
		
	/// Checking dir
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea", id);
	printf("Checking folder %s.. ", temp_buffer);
	ret = doesDirExist(temp_buffer);
	if ( ret != 1 )	{	
		print_color("Not found!\n\n", YELLOW);	
		
		/// We need to have the livearea page opened at least once so that we can backup the images ///		
		print_color("!! Press CANCEL on the following dialog and return here. !!\n\n", YELLOW);	 //Its easier to describe for noobs to press CANCEL -> flag 0x20000	
			sceKernelDelayThread(3000 * 1000); // 3 sec
			print_color("3..", YELLOW);
			sceKernelDelayThread(1000 * 1000); // 1 sec
			print_color("2..", YELLOW);
			sceKernelDelayThread(1000 * 1000); // 1 sec
			print_color("1.. ", YELLOW);
			sceKernelDelayThread(1000 * 1000); // 1 sec
			
		sprintf(temp_buffer, "psgm:play?titleid=%s", id);
		sceAppMgrLaunchAppByUri(0x20000, temp_buffer); //launch flag 0x40000 | open flag 0x20000
		
		sceKernelDelayThread(1000 * 1000); // 1 sec
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			printf("Press X to continue the installation..\n\n");
		} else {
			printf("Press O to continue the installation..\n\n");
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) break;
		}
		
	} else {
		print_color("Found!\n\n", GREEN);	
	}
	
	/// icon0.dds -> icon0_.dds (backup)
	sprintf(temp_buffer, "ur0:appmeta/%s/icon0.dds", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/icon0_.dds", id);
	printf("Backing up /%s/icon0.dds.. ", id);  //printf("Backing up %s.. ", temp_buffer);
	ret = copyFile(temp_buffer, temp_buffer2);
	sceIoRemove(temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
	
	/// icon0.dds
	sprintf(temp_buffer, "ur0:appmeta/%s/icon0.dds", id);
	printf("Writing /%s/icon0.dds.. ", id);  //printf("Writing %s.. ", temp_buffer);
	ret = copyFile("app0:files/icon0.dds", temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
	
	/// bg0.png -> bg0_.png (backup)
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/bg0.png", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/livearea/contents/bg0_.png", id);
	printf("Backing up /%s/livearea/contents/bg0.png.. ", id); //printf("Backing up %s.. ", temp_buffer);
	ret = copyFile(temp_buffer, temp_buffer2);
	sceIoRemove(temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
	
	/// bg0.png
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/bg0.png", id);
	printf("Writing /%s/livearea/contents/bg0.png..", id); //printf("Writing %s.. ", temp_buffer);
	ret = copyFile("app0:files/bg0.png", temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// startup.png -> startup_.png (backup)
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/startup.png", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/livearea/contents/startup_.png", id);
	printf("Backing up /%s/livearea/contents/startup.png.. ", id); //printf("Backing up %s.. ", temp_buffer);
	ret = copyFile(temp_buffer, temp_buffer2);
	sceIoRemove(temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
	
	/// startup.png
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/startup.png", id);
	printf("Writing /%s/livearea/contents/startup.png.. ", id); //printf("Writing %s.. ", temp_buffer);
	ret = copyFile("app0:files/startup.png", temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// template.xml
	/*sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/template.xml", id);
	printf("Writing %s.. ", temp_buffer);
	ret = copyFile("app0:files/template.xml", temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);*/
	
	/// Checking PBOOT
	sprintf(temp_buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
	printf("Checking for PSP/GAME/%s/PBOOT.PBP.. ", id); //printf("Checking for %s.. ", temp_buffer);
	ret = doesFileExist(temp_buffer);
	if ( ret == 1 )	print_color("Found!\n\n", GREEN);
	else {	
		print_color("Not found!\n", YELLOW);	
		print_color("It seems you haven't installed Adrenaline via this app!\nThe bubble's name won't change!\n\n", YELLOW);	
	}	
	
		sceKernelDelayThread(1000 * 1000); // 1 sec	for pad conflict maybe
			
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			print_color("\nDone! Press X to reboot..", GREEN); //or O to exit normally..\n", GREEN);
		} else {
			print_color("\nDone! Press O to reboot..", GREEN); // or X to exit normally..\n", GREEN);
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) scePowerRequestColdReset();
			if (pressed_buttons & SCE_CTRL_CANCEL) sceKernelExitProcess(0);
		}


}

void uninstall_theme_files(char *id) {

	char temp_buffer[256];
	char temp_buffer2[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
		
	printf("Uninstalling Adrenaline Theme\n-----------------------------\n\n");	
	printf("from: %s\n\n\n", id);	
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
	
	/// icon0.dds
	sprintf(temp_buffer, "ur0:appmeta/%s/icon0.dds", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/icon0_.dds", id);
	printf("Restoring %s.. ", temp_buffer);
	ret = copyFile(temp_buffer2, temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);
	else print_color("ERROR\n\n", RED);
		/// remove icon0_.dds
		printf("Deleting %s.. ", temp_buffer2);
		ret = sceIoRemove(temp_buffer2);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	
	/// bg0.png
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/bg0.png", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/livearea/contents/bg0_.png", id); //vs0:app/NPXS10028/sce_sys/psp_livearea/contents/bg0.png
	printf("Restoring /%s/livearea/contents/bg0.png.. ", id); //printf("Restoring %s.. ", temp_buffer);
	ret = copyFile(temp_buffer2, temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
		/// remove bg0_.dds
		printf("Deleting /%s/livearea/contents/bg0_.png.. ", id); //printf("Deleting backup %s.. ", temp_buffer2);
		ret = sceIoRemove(temp_buffer2);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	
	/// startup.png
	sprintf(temp_buffer, "ur0:appmeta/%s/livearea/contents/startup.png", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/livearea/contents/startup_.png", id);
	printf("Restoring /%s/livearea/contents/startup.png.. ", id); //printf("Restoring %s.. ", temp_buffer);
	ret = copyFile(temp_buffer2, temp_buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
		/// remove startup_.png
		printf("Deleting /%s/livearea/contents/startup_.png.. ", id); //printf("Deleting backup %s.. ", temp_buffer2);
		ret = sceIoRemove(temp_buffer2);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	
		
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			print_color("\n\nDone! Press X to reboot..\n", GREEN); // or O to exit normally..\n", GREEN);
		} else {
			print_color("\n\nDone! Press O to reboot..\n", GREEN); // or X to exit normally..\n", GREEN);
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				sceIoRemove("ux0:id.dat"); //for triggering "updating database"
				scePowerRequestColdReset();
			}	
			//if (pressed_buttons & SCE_CTRL_CANCEL) sceKernelExitProcess(0);
		}


}



void readPad() { //thx TheFlow
	static int hold_n = 0;

	memset(&pad, 0, sizeof(SceCtrlData));
	sceCtrlPeekBufferPositive(0, &pad, 1);

	current_buttons = pad.buttons;
	pressed_buttons = current_buttons & ~old_buttons;
	hold_buttons = pressed_buttons;
	released_buttons = ~current_buttons & old_buttons;

	if (old_buttons == current_buttons) {
		hold_n++;
		if (hold_n >= 10) {
			hold_buttons = current_buttons;
			hold_n = 4;	//speed (10 = max)
		}
	} else {
		hold_n = 0;
		old_buttons = current_buttons;
	}
}



int main() {
	
	initSceAppUtil();
		readInEnterButton();
		//getSystemLanguageCode();
		//getUsername();
	finishSceAppUtil();
		
	netInit();
	httpInit();
	
		if ( system_check() ) {
			initVita2DLib(); //initiate Vita2dLib
			draw_recovery_menu();
		}	
	
	sceKernelExitProcess(0);
	return 0;
}