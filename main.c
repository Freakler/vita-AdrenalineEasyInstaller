#include "main.h"
#include "init.h"
#include "file.h"

/* Notes & TODO
* - add sqlite method for the theme maybe?!
* - block PS button on install maybe?!
* - add progress indicator to 661 download
* - add check for right size of 661 pbp
* -
*
* "Ultra-Secret-Debug-Info-Mesages"-combo is: LEFT + L-TRIGGER + R-TRIGGER + START 
*/ 

/** Changelog 
* v1.04
* - added PSP names in selection menu
* - changed method to update database (addressed to bugs reported)
* - added another safety dialog before actually installing Adrenaline
* - added more message screens
* - added option to install a small basegame (thx to CeleseteBlue for the files)
* - added option to delete adrenaline flash files
* - added option to delete 661 Update file
* - bugs fixed
*
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
	{"1. Install Adrenaline eCFW"		, MENU_BLOCKED		, draw_psp_games			, "* Install 6.61 Adrenaline eCFW to a Basegame of your choice" 		},
	{"2. Uninstall Adrenaline eCFW"		, MENU_BLOCKED		, uninstall_adrenaline		, "* Remove Adrenaline from this device" 								},
	{"3. Theming Options"				, MENU_BLOCKED		, theme_options				, "* Access theming options menu"										},
	{"4. Advanced Options"				, MENU_ACTIVE 		, more_options				, "* Access advanced options menu" 										},
	{"5. Exit"							, MENU_EXIT 		, NULL						, "* Exit EasyInstaller" 												},
	{NULL,0,"",""}
};

Menu theme_menu[] = {
	//*title							//type				//*function					//*message
	{"Add Custom LiveArea Theme"		, MENU_BLOCKED		, install_theme				, "* Add a custom LiveArea Theme (optional)"							},
	{"Restore LiveArea Theme"			, MENU_BLOCKED		, uninstall_theme			, "* Remove the custom LiveArea Theme and restore the default look"		},
	{"back"								, MENU_EXIT 		, NULL						, "" 	},
	{NULL,0,"",""}
};

Menu more_menu[] = {
	//*title									//type				//*function					//*message
	{"Install a small PSP Basegame if needed"	, MENU_BLOCKED		, install_pspgame			, "" 	},
	{"Delete installed Adrenaline flash files"	, MENU_BLOCKED		, option_delete_flash		, ""	},
	{"Delete 661.PBP update file"				, MENU_BLOCKED		, option_delete_pbp			, ""	},
	{"Rebuild Database"							, MENU_ACTIVE		, option_rebuildDatabase	, "" 	},
	{"Update Database"							, MENU_ACTIVE		, option_updateDatabase		, "" 	},
	//{"Reboot System"							, MENU_ACTIVE		, option_reboot				, "" 	},
	{"back"										, MENU_EXIT 		, NULL						, "" 	},
	{NULL,0,"",""}
};
	


	
int system_check() {

	char temp[256];
	
	psvDebugScreenInit();
	
	printf("EasyInstaller v%s\n", VERSION);
	printf("-------------------\n\n\n");
	
	/// Model Check
	printf("Model check: ");
	if ( vshSblAimgrIsVITA() == 1 ) printf("Vita\n\n");
	else {
		printf("PlayStationTV\n\n");
		//deactivate Game installing here
	}	
	
	/// Enter Button check
	printf("Enter Button = ");
	if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) printf("X\n\n");
	else printf("O\n\n");		
	
	printf("\n");		
	
	
	/// taiHENkaku check
	printf("Checking for taiHENkaku.. ");
	if ( doesFileExist("ux0:tai/config.txt") ) {
		print_color("ux0:tai/config.txt found!!\n\n", GREEN);
		
	} else {
		print_color("Nothing found!!\n\n\n", RED);
		print_color("\n!!! Either you aren't running taiHENkaku, or you haven't enabled unsafe Homebrews in molecularShell yet!!!\n\n", RED);
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) printf("Press X to exit..\n\n");
		else printf("Press O to exit..\n\n");
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) return 0;
		}
	}

	/// checking for activation
	printf("Checking for act.dat file.. ");
	if (doesFileExist("tm0:npdrm/act.dat"))
		print_color("OK\n\n", GREEN);
	else {
		print_color("Not found!!\n\n", RED);
		print_color("\n!!! You must activate your Vita first!!!\n\n", RED);

		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) printf("Press X to exit...\n\n");
		else printf("Press O to exit...\n\n");
	
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
		
		//Check for DEFAULT_BASEGAME already installed
		sprintf(temp, "ux0:pspemu/PSP/GAME/%s", DEFAULT_BASEGAME); 
		if ( !doesDirExist(temp) > 0 ) {
			more_menu[0].type = MENU_ACTIVE; //activate option
		}
		
	} else {
		print_color("No content!!\n\n", YELLOW);
		
		more_menu[0].type = MENU_ACTIVE; //unblock basegame install here
			
			/**old
			print_color("\n!!! Please install any PSP Game/Demo first !!!\n\n", RED);
		
			if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) printf("Press X to exit..\n\n");
			else printf("Press O to exit..\n\n");
		
			while (1) {
				readPad();
				if (pressed_buttons & SCE_CTRL_ENTER) return 0;
			}**/
	}
		
	
	/// Adrenaline prx files in ux0:pspemu/adrenaline
	printf("Checking for Adrenaline prx files.. ");	
	if ( doesFileExist("ux0:pspemu/adrenaline/adrenaline.skprx") ) {
		print_color("Found!!\n\n", GREEN);	
	} else {
		print_color("Not found!!\n\n", YELLOW);
	}
	
	
	/// Adrenaline installed to PSP game in config check??
	printf("Checking ux0:tai/config.txt for adrenaline.. ");
	printf("%s \n\n", get_id_of_psp_game_that_adrenaline_is_installed_to());
	if ( PSP_GAME_ID[0] != '\0' ) { //config installation found!!

		/// Does the game still exist though?!
		sprintf(temp, "ux0:pspemu/PSP/GAME/%s/EBOOT.PBP", PSP_GAME_ID); 
		printf("Checking for content with ID = %s.. ", PSP_GAME_ID);
		if ( !doesFileExist(temp) ) { //game was deleted -> means everything in config is a leftover and wrong
			
			print_color("Not found!!\n\n", RED);
			
			//its deleted but adrenaline was still installed to it in the tai config.. clean up
			printf("Deleting old installation for %s.. ", PSP_GAME_ID);
			if ( delete_adrenaline_from_config(PSP_GAME_ID) == 0 ) print_color("OK\n\n", GREEN);
			else print_color("Error\n\n", RED);
			
			//also delete "maybe" old adrenaline prx files although it doesn't matter
			sceIoRemove("ux0:pspemu/adrenaline/adrenaline.skprx");
			sceIoRemove("ux0:pspemu/adrenaline/adrenaline.suprx");
			
			PSP_GAME_ID[0] = 0; //clear it to continue
			
		} else {
			print_color("OK\n\n", GREEN);
		}
	}		
	
	/// check for Adrenaline flash files	
	printf("Checking for Adrenaline flash files.. ");	
	if ( doesDirExist("ux0:pspemu/adrenaline/flash0") ) {
		print_color("Found!!\n\n", GREEN);	
		more_menu[1].type = MENU_ACTIVE; //activate "Delete flash files"
		
	} else {
		print_color("Not found!!\n\n", YELLOW);
	}

	/// check for 661.PBP
	printf("Checking for 661.PBP.. ");	
	if ( doesFileExist("ux0:pspemu/adrenaline/661.PBP") ) {
		print_color("Found!!\n\n", GREEN);	
		more_menu[2].type = MENU_ACTIVE; //activate "Delete Update PBP"
		
	} else {
		print_color("Not found!!\n\n", YELLOW);
	}
	
	
	
		/// Activating or Disabling Options depending on the checks //////////////////////////
		if ( PSP_GAME_ID[0] == '\0' && gamesfound > 0 ) {
			recovery_menu[0].type = MENU_ACTIVE; //activate "Install Adrenaline" option
			
		} else if ( gamesfound == 0 ) {
			//keep everything blocked basically
			
		} else { //Adrenaline should be installed
			recovery_menu[1].type = MENU_ACTIVE; //activate "Uninstall Adrenaline" option
			recovery_menu[2].type = MENU_ACTIVE; //activate "Theme Menu"
				theme_menu[0].type = MENU_ACTIVE; //activate Theme Installing
			
			sprintf(temp, "ur0:appmeta/%s/livearea/contents/bg0_.png", PSP_GAME_ID);
			if ( doesFileExist(temp) ) { //theme backup files found
				theme_menu[1].type = MENU_ACTIVE; //activate "Uninstall Theme"
				theme_menu[0].type = MENU_BLOCKED; //deactivate "Install Theme" again
			}	
		}
		

	printf("\n\nDone! Starting..");
	//sceKernelDelayThread(5000 * 1000); // 5 sec delay
	
	return 1;
}

	
	
	
void draw_recovery_menu() {

	int options = 0;		//available options
	int selection = 0;		//selected option
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
	
		/// calculate number of options
		while(recovery_menu[ options ].title != NULL) options++;
		
		/// Error handling
		if ( showoptions > options ) showoptions = options; //Adjust showoptions (there can't be more "showoptions" than "options" itself)
		
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
			
			vita2d_draw_line(0,  70, 960,  70, GREY); //top line
			vita2d_draw_line(0, 410, 960, 410, GREY); //bot line
			vita2d_draw_line(100, 0, 100, 544, GREY); //left line
			vita2d_draw_line(860, 0, 860, 544, GREY); //right line
		}
		
		/// draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2), 40, text_color, TEXT_SIZE_BIG, TITLE);
		vita2d_pgf_draw_textf(pgf, SCREEN_WIDTH/2+(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2),  40, GREY, TEXT_SIZE, " v%s", VERSION);
		
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
	
	int ret = drawdialog("Do you really want to continue uninstalling Adrenaline?","");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		uninstall_adrenaline_files(PSP_GAME_ID);
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
			vita2d_pgf_draw_textf(pgf, 640, 100, GREY, TEXT_SIZE, "gamesfound/options = %i", options);
			vita2d_pgf_draw_textf(pgf, 640, 125, GREY, TEXT_SIZE, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 640, 150, GREY, TEXT_SIZE, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 640, 175, GREY, TEXT_SIZE, "showoptions = %i", showoptions);
			
			vita2d_draw_line(0,  70, 960,  70, GREY); //top line
			vita2d_draw_line(0, 410, 960, 410, GREY); //bot line
			vita2d_draw_line(100, 0, 100, 544, GREY); //left line
			vita2d_draw_line(860, 0, 860, 544, GREY); //right line
		}
		
		
		/// Draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	

		
		if ( selection < toption ) toption = selection;	//scrolling up with selection
		
		for(i=toption; i < showoptions+toption; i++, y += z) {
			if ( i == selection ) { //draw active selection
				vita2d_draw_rectangle(x-25, y-40, 760, 60, menu_active_color); //background
				vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s - %s", content_array[selection].titleID, content_array[selection].title);
			} else { //draw all other options
				vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE_BIG, "%s - %s", content_array[i].titleID, content_array[i].title);	
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
			
				int ret = drawdialog(content_array[selection].title, "Do you really want to continue installing to this Game?");
				if ( ret ) {
					closeVita2DLib(); //shutdown Vita2dLib	
					install_adrenaline_files(content_array[selection].titleID);
				}

			//return 0;
		}
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return 0;
}

void *theme_options() {
	
	//
	
	draw_submenu(theme_menu, "Themeing Options");
	
	return 0;
}

void *install_theme() {
	
	int ret = drawdialog("Do you really want to continue installing the theme?","");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		install_theme_files(PSP_GAME_ID);
	}
	
	return 0;
}

void *uninstall_theme() {
	
	int ret = drawdialog("Do you really want to restore the original livearea theme?","");
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		uninstall_theme_files(PSP_GAME_ID);
	}
	
	return 0;
}





void *more_options() {
		
	draw_submenu(more_menu, "Advanced Options");
	
	return 0;
}

	/// installs a small basegame
	void *install_pspgame() {
		int ret = drawdialog("Do you really want to continue installing a PSP Game?","After that you will be able to install Adrenaline to it!");
		if ( ret ) {
			closeVita2DLib(); //shutdown Vita2dLib
			install_psp_basegame();
		}
	
		return 0;
	}

	/// This will delete the adrenaline flash files
	int *option_delete_flash() {
		int ret = drawdialog("Do you really want to delete the flash files?","");
		if ( ret ) {
			drawstatusmessage("Deleting flash files.. Please wait..");
				sceIoRemove("ux0:pspemu/adrenaline/adrenaline.bin");
				removePath("ux0:pspemu/adrenaline/flash0");
				removePath("ux0:pspemu/adrenaline/flash1");
			drawmessage("The flash files have been deleted!", "Okay");
			more_menu[1].type = MENU_BLOCKED; //deactivate again
		}
	
		return 0;
	}

	/// This will delete 661.PBP in ux0:pspemu/adrenaline/
	int *option_delete_pbp() {
		int ret = drawdialog("Do you really want to delete the 6.61 Update file?","");
		if ( ret ) {
			ret = sceIoRemove("ux0:pspemu/adrenaline/661.PBP");
			if ( ret < 0 ) {
				drawmessage("An error occured!", "Okay");
			} else {
				drawmessage("The Update file has been deleted!", "Okay");
				more_menu[2].type = MENU_BLOCKED; //deactivate again
			}
		}
	
		return 0;
	}
	
	/// This will trigger "Rebuilding Database" by deleting ur0:shell/db/app.db!
	int *option_rebuildDatabase() {
		int ret = drawdialog("Do you really want to rebuild the Database?","Your icon layout will be lost!");
		if ( ret ) {
			trigger_rebuild_database();
			scePowerRequestColdReset();
		}
	
		return 0;
	}


	/// This will trigger "Updating Database" by deleting ux0:id.dat from the Memory Card!
	int *option_updateDatabase() {
		int ret = drawdialog("Do you really want to update the Database?","This will keep your icon layout unimpaired!");
		if ( ret ) {
			trigger_update_database();
			scePowerRequestColdReset();
		}
	
		return 0;
	}

	/// This will reboot the PS Vita System!
	int *option_reboot() {
		int ret = drawdialog("Do you really want to reboot the Vita System?","");
		if ( ret ) {
			scePowerRequestColdReset();
		}
		
		return 0;
	}
	
	
	
	

void draw_submenu(Menu *sub_menu, const char* title) {

	//variables
	int options = 0;		//available options (initialized with 0)
	int selection = 0;		//selected option number
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
		while(sub_menu[ options ].title != NULL) options++; //calculate number of options
		while(sub_menu[ selection ].type == MENU_BLOCKED) selection += 1; //get first Active option
	
		if ( showoptions > options ) showoptions = options; //Adjust showoptions (there can't be more "showoptions" than "options" itself)	
		
	while (running) {
	
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();
		
			while(sub_menu[ selection ].type == MENU_BLOCKED) selection += 1; //jump to next active option if it has been blocked and you returned here (this is save here as "back is always active")
		
		/// Draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
		
		/// Draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	

		if ( selection < toption ) toption = selection;	 //scrolling up with selection
		
		for(i=toption; i < showoptions+toption; i++, y += z) {
			if ( i == selection ) { //draw active selection
				vita2d_draw_rectangle(x-25, y-40, 760, 60, menu_active_color); //background
				vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s", sub_menu[selection].title);
				vita2d_pgf_draw_textf(pgf, x, 465, text_color, TEXT_SIZE, "%s", sub_menu[selection].message); //message
			} else { //draw all other inactive options
				if ( sub_menu[i].type == MENU_BLOCKED ) { //when blocked adjust color
					vita2d_pgf_draw_textf(pgf, x, y, menu_text_inactive_color, TEXT_SIZE_BIG, "%s", sub_menu[i].title);	
				} else { //none-blocked
					vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE_BIG, "%s", sub_menu[i].title);	
				}	
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
		
		// Navigation UP ////////////////////////////		
			if ( hold_buttons & SCE_CTRL_UP) {	
				
				if ( selection <= 0 ) {
					selection = options-1;
					toption = options-showoptions; //scroll
				} else selection -= 1;					
				
				while ( sub_menu[selection].type == MENU_BLOCKED ) {
					if ( selection <= 0 ) {
					selection = options-1;
					toption = options-showoptions; //scroll
					} else selection -= 1;		
					//scroll
					if ( toption > 1 ) {
						if ( selection == 1 ) toption--; 
					}
				}
				//scroll
				if ( toption > 1 ) {
					if ( selection == 1 ) toption--; 
				}
			
			} 	
		
			// Navigation DOWN ////////////////////////////
			if ( hold_buttons & SCE_CTRL_DOWN) {			
				
				if ( showoptions == 1 ) { //hadnling 1 option only

					if ( selection >= options-1 ) selection = 0;
					else selection += 1;
				
					while ( sub_menu[selection].type == MENU_BLOCKED ) {			
						if ( selection >= options-1 ) selection = 0;
						else selection += 1;
					}
					toption = selection;
					
				} else {
				
					if ( selection >= options-1 ) selection = 0;
					else {
						selection += 1;
						//scroll
						if ( toption+showoptions < options ) {
							if ( selection == toption+showoptions-1 ) toption++; 
						}
					}	
				
					while ( sub_menu[selection].type == MENU_BLOCKED ) {					
						if ( selection >= options-1 ) selection = 0;
						else {
							selection += 1;
							//scroll
							if ( toption+showoptions < options ) {
								if ( selection == toption+showoptions-1 ) toption++; 
							}
						}	
					}	
					//scroll
					if ( toption+showoptions < options ) {
						if ( selection == toption+showoptions-1 ) toption++; 
					}
				}		
			}
		
		//back
		if ( pressed_buttons & SCE_CTRL_CANCEL) running = 0;
		
		//Exit Combo
		if (pressed_buttons & SCE_CTRL_SELECT && pressed_buttons & SCE_CTRL_START) break;	
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		void (* function)();
		
		if ( sub_menu[selection].type == MENU_ACTIVE ) {
		
			if (pressed_buttons & SCE_CTRL_ENTER) {
				
				vita2d_end_drawing();
				vita2d_swap_buffers();
				sceDisplayWaitVblankStart();
				
					function = (void *)(sub_menu[selection].function);
					function();
				
				vita2d_start_drawing();
				vita2d_clear_screen();
		
				
			}	
		
		} else if ( sub_menu[selection].type == MENU_EXIT ) {
			
			if (pressed_buttons & SCE_CTRL_ENTER) running = 0;
			
		} else {	
			//Error
			///vita2d_pgf_draw_text(font, 350,  470, RED, 2.0f, "(null)");
		}
		
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
}



int drawdialog(const char* message, const char* message2) {
	
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
			
			vita2d_pgf_draw_textf(pgf, 960/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message2)/2), 260, text_color, TEXT_SIZE_BIG, "%s", message2); //draw Message //~180 in original menu
			
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




int drawmessage(const char* message, const char* button_text) { 
		
	//variables
	int running = 1;
	int x, y, z, q;

	while (running) {
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2), 40, text_color, TEXT_SIZE_BIG, TITLE);
		
		
		/// Menu /////////////////////////////////////////////////////////////////////////////////////////////////////
		
		x = SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, button_text)/2); //for centered position
		y = 420; //vertical start of button	
		q = 30; //button add. space left & right
		z = vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, button_text)+q; //size of background of button
		
		
		
			/// draw Message
			vita2d_pgf_draw_textf(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message)/2), 220, text_color, TEXT_SIZE_BIG, "%s", message);
						
			/// draw button
			vita2d_draw_rectangle(x-q/2, y-40, z, 60, menu_active_color); //draw choice-background
			vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s", button_text);
		
			
			
		/// Button Controls
		x = 15; //horizontal start
		y = 525; //vertical start
		vita2d_draw_texture(button_enter, x, y-17);
		vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Enter");
			
			if ( pressed_buttons & SCE_CTRL_ENTER) running = 0;		
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	
	return 0;
}

int drawstatusmessage(const char* message) { 
	int i;
	
	for(i = 0; i < 3; i++) {
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, TITLE)/2), 40, text_color, TEXT_SIZE_BIG, TITLE);
		
			/// draw Message
			vita2d_pgf_draw_textf(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message)/2), 220, text_color, TEXT_SIZE_BIG, "%s", message);		
			
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return 0;
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
		ret = download_file(UPDATE_LINK, "ux0:pspemu/adrenaline/661.PBP");
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
			trigger_update_database(); //for triggering "updating database"
			scePowerRequestColdReset();
		}	
		if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
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
				trigger_update_database(); //for triggering "updating database"
				scePowerRequestColdReset();
			}	
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
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
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}


}


/*void install_sqlite_test(char *id) { // This is no priority.. just for later. 
	
	tbl_appinfo
	where key = 572932585
		set value = "Adrenaline"
	where key = 4007071428	
		set value = "Adrenaline"

	tbl_livearea
		where titleId = XXXX
			set style = psmobile
			
			
	tbl_livearea_frame
		add frame1
		<liveitem><text valign="bottom" align="left" text-align="left" text-valign="bottom" line-space="3" ellipsis="on"><str color="#ffffff" size="50" bold="on" shadow="on">6.61 Adrenaline</str></text></liveitem>
			
		add frame2
		<liveitem><text valign="top" align="left" text-align="left" text-valign="top" line-space="2" ellipsis="on"><str size="22" shadow="on">by The_FloW</str></text></liveitem>
		
		add frame3
		<liveitem><text align="right" text-align="right" word-wrap="off" ellipsis="on"><str size="18" color="#ffffff" shadow="on">v0.01</str></text></liveitem>
		
}*/



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
				trigger_update_database(); //for triggering "updating database"
				scePowerRequestColdReset();
			}	
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}


}


void install_psp_basegame() {
		
	char bgdl[] = "ux0:/bgdl/t/00000001";
	char pspemu[] = "ux0:/pspemu/bgdl/00000001";
	char temp_buffer[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
		
	printf("Installing PSP Basegame\n-----------------------\n\n");	
	printf("ID: %s (thanks to CelesteBlue for the files!)\n\n\n", DEFAULT_BASEGAME);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
	/// Checking for existing folder (means there is something in download queue!!)
	printf("Checking download queue.. ");
	if ( doesDirExist(bgdl) ) {
		print_color("Content found!\n\n", RED);	
			
		//something is in download queue
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) {
			print_color("\nPress X to delete content and continue..\n", YELLOW); 
			print_color("Press O to abort installation..\n\n", YELLOW); 
		} else {
			print_color("\nPress O to delete content and continue..\n", YELLOW);
			print_color("Press X to abort installation..\n\n", YELLOW);
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				
				if ( doesDirExist(pspemu) ) {
					printf("Deleting content in %s.. ", pspemu);
					ret = removePath(pspemu);
					if ( ret == 1 )	{ //deleting successful
						print_color("OK\n", GREEN);
					} else { //deleting failed
						print_color("ERROR\n", RED);
						break;
					}	
				}	
				
				printf("Deleting content in %s.. ", bgdl);
				ret = removePath(bgdl);
				if ( ret == 1 )	{ //deleting successful
					print_color("OK\n\n", GREEN);
					goto continue_install;
				} else { //deleting failed
					print_color("ERROR\n\n", RED);	
					break;	
				}
				
			}			
			if (pressed_buttons & SCE_CTRL_CANCEL) { 
				printf("-> Please finish your downloads first, reboot and retry!!\n");
				
				if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nPress X to exit..\n", YELLOW); 
				else print_color("\n\nPress O to exit..\n", YELLOW);
		
				while (1) {
					readPad();
					if (pressed_buttons & SCE_CTRL_ENTER) sceKernelExitProcess(0);
					if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
				} 
			}
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		} 
		
		
	} else {
		
		print_color("Nothing found!\n\n", GREEN);
		
			continue_install:
		
		/// make dir
		printf("Creating folder %s.. ", bgdl);
		ret = sceIoMkdir(bgdl, 0777);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
			
		/// d0.pdb
		sprintf(temp_buffer, "%s/d0.pdb", bgdl);
		printf("Writing %s.. ", temp_buffer);
		ret = copyFile("app0:files/basegame/bgdl/d0.pdb", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
	
		/// d1.pdb
		sprintf(temp_buffer, "%s/d1.pdb", bgdl);
		printf("Writing %s.. ", temp_buffer);
		ret = copyFile("app0:files/basegame/bgdl/d1.pdb", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// f0.pdb
		sprintf(temp_buffer, "%s/f0.pdb", bgdl);
		printf("Writing %s.. ", temp_buffer);
		ret = copyFile("app0:files/basegame/bgdl/f0.pdb", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
	
		/// icon.png
		sprintf(temp_buffer, "%s/icon.png", bgdl);
		printf("Writing %s.. ", temp_buffer);
		ret = copyFile("app0:files/basegame/bgdl/icon.png", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/*/// temp.dat
		sprintf(temp_buffer, "%s/temp.dat", bgdl);
		printf("Writing %s.. ", temp_buffer);
		ret = copyFile("app0:files/basegame/bgdl/temp.dat", temp_buffer);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);*/
		
		
		/// pspemu bgdl ///////
		
		/// make dirs
		printf("\nCreating folders in %s.. ", pspemu);
		
		sceIoMkdir("ux0:pspemu/bgdl", 0777); //make bgdl (unlikely but necessary)
		sceIoMkdir(pspemu, 0777); //make 00000001
		
			sprintf(temp_buffer, "%s/PCSC80018", pspemu);
			sceIoMkdir(temp_buffer, 0777); //make 00000001/PCSC80018
		
				sprintf(temp_buffer, "%s/PCSC80018/sce_sys", pspemu);
				sceIoMkdir(temp_buffer, 0777); //make 00000001/PCSC80018/sce_sys
		
					sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package", pspemu);
					sceIoMkdir(temp_buffer, 0777); //make 00000001/PCSC80018/sce_sys/package
		
				sprintf(temp_buffer, "%s/PCSC80018/USRDIR", pspemu);
				sceIoMkdir(temp_buffer, 0777); //make 00000001/PCSC80018/USRDIR
		
					sprintf(temp_buffer, "%s/PCSC80018/USRDIR/CONTENT", pspemu);
					ret = sceIoMkdir(temp_buffer, 0777); //make 00000001/PCSC80018/USRDIR/CONTENT
			
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
		
		/// PCSC80018/sce_sys/package/body.bin
		sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package/body.bin", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/sce_sys/package/body.bin.. ");
		ret = copyFile("app0:files/basegame/pspemu/body.bin", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// PCSC80018/sce_sys/package/head.bin
		sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package/head.bin", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/sce_sys/package/head.bin.. ");
		ret = copyFile("app0:files/basegame/pspemu/head.bin", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// PCSC80018/sce_sys/package/stat.bin
		sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package/stat.bin", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/sce_sys/package/stat.bin.. ");
		ret = copyFile("app0:files/basegame/pspemu/stat.bin", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// PCSC80018/sce_sys/package/tail.bin
		sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package/tail.bin", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/sce_sys/package/tail.bin.. ");
		ret = copyFile("app0:files/basegame/pspemu/tail.bin", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// PCSC80018/sce_sys/package/work.bin
		sprintf(temp_buffer, "%s/PCSC80018/sce_sys/package/work.bin", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/sce_sys/package/work.bin.. ");
		ret = copyFile("app0:files/basegame/pspemu/work.bin", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);
		
		/// PCSC80018/USRDIR/CONTENT/EBOOT.PBP
		sprintf(temp_buffer, "%s/PCSC80018/USRDIR/CONTENT/EBOOT.PBP", pspemu);
		//printf("Writing %s.. ", temp_buffer);
		printf("Writing /PCSC80018/USRDIR/CONTENT/EBOOT.PBP.. ");
		ret = copyFile("app0:files/basegame/pspemu/EBOOT.PBP", temp_buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		
	
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nDone! Press X to reboot..\n", GREEN); //or O to exit normally..\n", GREEN);
		else print_color("\n\nDone! Press O to reboot..\n", GREEN); //or X to exit normally..\n", GREEN);

		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) scePowerRequestColdReset();
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		} 
		
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

/*void lock_psbutton() {
    sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
}

void unlock_psbutton() {
    sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
}*/


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