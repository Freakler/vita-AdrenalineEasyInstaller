#include "main.h"
#include "init.h"
#include "file.h"
#include "sqlite-3.6.23.1/sqlite3.h"

/** Notes & TODO
* - remove PS1 official games from installer list
* - figure out how to update database manually so no more rebooting (would fix next bulletpoint too)
* - add warning for PSTV + USB Stick users
* - Addon-Installer?!
* -
* - clean up & and learn how to code properly
**/ 

/*
To add a new release:
- put files in new folder in [files/releases/xxxxxxx]
- add entry in "files_menu" (main.c)
- adjust version in [main.h] [livearea/template.xml]
- add EasyInstaller changelog to [livearea/changeinfo.xml]
- add Adrenaline changelog to [files/updatehistory.txt]
*/

/** Changelog
* v1.12
* - adjustments due to latest tai/config.txt location change
* 
* v1.11
* - updated for Adrenaline-4.1
* - added unsafe hombrew check
* 
* v1.10
* - updated for Adrenaline-4
* - added option to delete SaveStates
* 
* v1.09
* - updated for Adrenaline-3.1
*
* v1.08
* - updated for Adrenaline-3 fix
* - removed resetting of installed flash files when up/downgrading as its not needed
*
* v1.07
* - added all releases of Adrenaline to choose from
* - restructured the menu because of all new options
* - you can now up/downgrade without rebooting (with same basegame)
* - the used basegame for Adrenaline will show up green in selection menu
* - added Advanced LiveArea Theming Option (app.db)
* - added view Adrenaline Update-History as option
* - added firmware check warning for future hacks
* - added check for valid 661.PBP file (if not it will be deleted)
* - added check for existing PBOOTs in basegames
* - added check for changeinfo.xml installation
* - typical CFW files & folders will now be created automatically
* - added option to display taiHEN config.txt
* - added option to reload taiHEN config.txt
* - adjusted Bubble Name and livearea
* - removed blocking of PSButton again
* - removed unnecessary files & code
*   
* v1.061
* - updated for Adrenaline-2
* 
* v1.05
* - added 6.61 Update file download progress indicator
* - added update history info to livearea
* - while installing the Home button is now blocked
* - fixed a bug with wrongly formatted tai-config files
*
* v1.04
* - added PSP names in selection menu
* - changed method to update database (addressed to bugs reported)
* - added another safety dialog before actually installing Adrenaline
* - added more message screens
* - added option to install a small basegame (thx to CelesteBlue for the files)
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
* - bugs fixed
*
* Thanks to TheFloW, Team Molecule, gdljjrod, SMOKE and all others
*/


/// Main Menu /////////////
Menu recovery_menu[] = {
	//*title								//type				//*function					//*arg		//*arg2		//*message
	{"1. Adrenaline eCustomFirmware ➡", 	MENU_BLOCKED, 		installer_options,			"",			"",			"* Install / uninstall / up- & downgrade the Adrenaline eCustomFirmware" },
	{"2. LiveArea theming ➡", 				MENU_BLOCKED, 		theme_options,				"",			"",			"* Access theming options menu"										},
	{"3. Advanced options ➡", 				MENU_ACTIVE, 		more_options,				"",			"",			"* Access advanced options menu" 										},
	{"4. View Adrenaline Update-History", 	MENU_ACTIVE, 		option_show_changelogfile,	"",			"",			"* View the Adrenaline update-History (aka Changelog)"				},
	{"5. Exit",								MENU_EXIT, 			NULL,						"",			"",			"* Exit EasyInstaller" 												},
	{NULL,0,"","",""}
};

Menu installer_menu[] = {
	//*title										//type				//*function				//*arg			//*arg2 			//*message
	{"Install the latest Adrenaline eCFW release", 	MENU_ACTIVE, 		draw_psp_games,			"latest", 		"ux0:adrenaline", 	"* Install the latest Adrenaline eCFW release to a Basegame of your choice" },
	{"Manually update/downgrade Adrenaline", 		MENU_ACTIVE, 		files_option,			"", 			"",					"* Select any Adrenaline release & up or downgrade it easily" },
	{"Uninstall Adrenaline eCFW completely", 		MENU_BLOCKED, 		uninstall_adrenaline,	"", 			"", 				"* Remove the Adrenaline eCustomFirmware from this device" },
	{"Cancel", 										MENU_EXIT,			draw_psp_games,			"", 			"", 				"" 	},
	{NULL,0,"","",""}
};

Menu files_menu[] = {
	//*title									//type				//*function				//*arg				//*arg2 					//*message
	{"2017-04-18 : adrenaline_v4.1.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v4.1"	, "ux0:adrenaline"			, ""	},
	{"2017-04-18 : adrenaline_v4.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v4"	, "ux0:adrenaline"			, ""	},
	{"2017-04-17 : adrenaline_v3.1.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v3.1"	, "ux0:adrenaline"			, ""	},
	{"2017-04-16 : adrenaline_v3_fix.zip"		, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v3_fix", "ux0:adrenaline"			, ""	},
	{"2017-04-15 : adrenaline_v3.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v3"	, "ux0:adrenaline"			, ""	},
	{"2017-04-02 : adrenaline_v2.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_v2"	, "ux0:adrenaline"			, ""	},
	{"2016-11-18 : adrenaline_fix2.zip"			, MENU_ACTIVE		, draw_psp_games		,"adrenaline_fix2"	, "ux0:pspemu/adrenaline"	, ""	},
	{"2016-11-17 : adrenaline.zip"				, MENU_ACTIVE		, draw_psp_games		,"adrenaline"		, "ux0:pspemu/adrenaline"	, ""	},
	//{"Cancel"									, MENU_EXIT 		, draw_psp_games		,"0"				, ""						, "" 	},
	{NULL,0,"","",""}
};

Menu theme_menu[] = {
	//*title									//type				//*function					//*arg		//*arg2 //*message
	{"Add Custom LiveArea Theme"				, MENU_BLOCKED		, install_theme				,""			, ""	, "* Add a custom LiveArea Theme & Icon (reboot required)"						},
	{"Restore LiveArea"							, MENU_BLOCKED		, uninstall_theme			,""			, ""	, "* Remove the custom Theme and restore the default look (reboot required)"	},
	{"Add Style & Text frames to LiveArea"		, MENU_BLOCKED		, install_sqlite_theme		,""			, ""	, "* Add Style & Text frames to the LiveArea Screen (no reboot required)"		},
	{"Remove Style & Text frames from LiveArea"	, MENU_BLOCKED		, uninstall_sqlite_theme	,""			, ""	, "* Remove Style & Text frames from the LiveArea (no reboot required)"			},
	{"Cancel"									, MENU_EXIT 		, NULL						,""			, ""	, "" 	},
	{NULL,0,"","",""}
};

Menu more_menu[] = {
	//*title									//type				//*function					//*arg		//*arg2 //*message
	{"Install a small PSP Basegame if needed"	, MENU_BLOCKED		, install_pspgame			,""			, ""	, "" 	},
	{"Delete installed Adrenaline flash files"	, MENU_BLOCKED		, option_delete_flash		,""			, ""	, ""	},
	{"Delete 661.PBP update file"				, MENU_BLOCKED		, option_delete_pbp			,""			, ""	, ""	},
	{"Delete all Adrenaline SaveStates"			, MENU_BLOCKED		, option_delete_savestates	,""			, ""	, ""	},
	{"Display taiHENkaku config.txt file"		, MENU_ACTIVE		, option_show_taiconfig		,""			, ""	, ""	},
	//{"testing stuff"							, MENU_ACTIVE		, option_test				,""			, ""	, ""	},
	{"★ Reload taiHENkaku config"				, MENU_ACTIVE		, option_reloadTaiConfig	,""			, ""	, "" 	},
	{"★ Rebuild Database"						, MENU_ACTIVE		, option_rebuildDatabase	,""			, ""	, "" 	},
	{"★ Update Database"						, MENU_ACTIVE		, option_updateDatabase		,""			, ""	, "" 	},
	//{"Reboot System"							, MENU_ACTIVE		, option_reboot				,""			, ""	, "" 	},
	{"Cancel"									, MENU_EXIT 		, NULL						,""			, ""	, "" 	},
	{NULL,0,"","",""}
};
	



int system_check() {
	
	psvDebugScreenInit();
	
	printf("EasyInstaller v%s\n", VERSION);
	printf("-------------------\n\n\n");
	
	// Some system checks first ///////////////////////////////////////////////////////////////////////////////////////////////
	
		/// Unsafe Hombrew check
		if ( checkUnsafeHomebrew() ) {
			print_color("\n!! It seems you haven't enabled unsafe hombrews. !!\n\n\n", YELLOW);	
				
			if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) printf("Press X to open the HENkaku settings or O to exit..\n\n");
			else printf("Press O to open the HENkaku settings or X to exit..\n\n");
				
			while (1) {
				readPad();
				if (pressed_buttons & SCE_CTRL_CANCEL) sceKernelExitProcess(0);	
				if (pressed_buttons & SCE_CTRL_ENTER) {
					sceAppMgrLaunchAppByUri(0x20000, "settings_dlg:"); //launch flag 0x40000 | open flag 0x20000
					sceKernelExitProcess(0);	
				}
			}
		}
		
		/// Model Check
		printf("Model = ");
		if ( vshSblAimgrIsVITA() == 1 ) printf("Vita\n\n");
		else printf("PlayStationTV\n\n");
		
		/// Enter Button check
		printf("Enter Button = ");
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) printf("X\n\n");
		else printf("O\n\n");
		
		
		/// Firmware Check
		printf("Firmware = ");
		sprintf(buffer, "%s", getRealFirmwareVersion(0));
		printf("%s\n\n", buffer);
		if ( strcmp(buffer, "0x03600000") != 0 ) { //if a firmware different to 3.60 is detected there will be a warning
			print_color("!! This firmware version may not be supported !!\n\n", YELLOW);	
				
			if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) printf("Press X to continue anyways or O to exit..\n\n");
			else printf("Press O to continue anyways or X to exit..\n\n");
				
			while (1) {
				readPad();
				if (pressed_buttons & SCE_CTRL_CANCEL) sceKernelExitProcess(0);	
				if (pressed_buttons & SCE_CTRL_ENTER) break;
			}
		}
		
		/// Write changeinfo.xml for Livearea manually. (Does vpk promoting not feature this? [https://www.vitadevwiki.com/index.php?title=File:PKGdirstruct.png] Deleteing the bubble will delete patch/xxx though) 
		sprintf(buffer, "ux0:patch/%s/sce_sys/changeinfo/changeinfo.xml", TITLEID);
		if ( doesFileExist(buffer) ) {
			if ( getFileSize(buffer) < getFileSize("app0:sce_sys/changeinfo/changeinfo.xml") ) { //overwrite only when there is a newer (bigger) file
				goto install_xml;
			} else {
				printf("Changeinfo.xml is ");
				print_color("up-to-date!\n\n", GREEN);
			}		
		} else {
			install_xml:
			printf("Writing changeinfo.xml.. ");
			if ( writeChangeinfo(TITLEID) ) print_color("OK\n\n", GREEN);
			else print_color("Error\n\n", RED);
		}
	
	printf("\n");	


	// Now the Adrenaline needed checks ///////////////////////////////////////////////////////////////////////////////////////////////
	
		/// taiHENkaku check -> If not found the Installer won't start
		printf("Checking for taiHENkaku.. ");
		if ( doesFileExist(TAI_CONFIG) ) {
			print_color("OK\n\n", GREEN);
		} else {
			if ( doesFileExist("ur0:tai/config.txt") ) {
			sprintf(TAI_CONFIG, "ur0:tai/config.txt");	
			print_color("OK\n\n", GREEN);
			
			} else {
				print_color("Nothing found!!\n\n\n", RED);
				print_color("\n!!! Please (re)install taiHENkaku first and try again!!!\n\n", RED);
				goto error_exit;
			}
		}

		/// checking Device Activation (needed for PSP games to run -> Adrenaline to run)
		printf("Checking for tm0:npdrm/act.dat file.. ");
		if (doesFileExist("tm0:npdrm/act.dat")) print_color("OK\n\n", GREEN);
		else {
			print_color("Not found!!\n\n", RED);
			print_color("\n!!! You must activate your Vita first!!!\n\n", RED);
			goto error_exit;
		}

		/// checking for installed PSP Content
		printf("Checking for PSP content in /PSP/GAME/.. ");	
		gamesfound = check_for_psp_content("ux0:pspemu/PSP/GAME/"); //storing globally for later
		if ( gamesfound > 0 ) { //there is PSP content installed
			print_color("OK ", GREEN);
			printf("%i Game(s)\n\n", gamesfound );
			recovery_menu[0].type = MENU_ACTIVE; //since there are pspgames we can install adrenaline -> activate the option
			
			/// Check for DEFAULT_BASEGAME already installed 
			sprintf(buffer, "ux0:pspemu/PSP/GAME/%s", DEFAULT_BASEGAME); 
			if ( doesDirExist(buffer) == 0 ) {
				more_menu[0].type = MENU_ACTIVE; //activate PSP basegame installer
			}
		} else { //No content at all
			print_color("No content!\n\n", YELLOW);
			more_menu[0].type = MENU_ACTIVE; //activate PSP basegame installer
		}
		
		/// Are there Adrenaline prx files in the default folder
		sprintf(buffer, "%s/adrenaline.skprx", ADR_FOLDER);
		printf("Checking for installed Adrenaline prx files.. ");	
		if ( doesFileExist(buffer) ) {
			print_color("OK\n\n", GREEN);	
			ADR_PRX_INSTL = 1;
		} else {
			///checking here for the old Adrenaline installation
			if ( doesFileExist("ux0:pspemu/adrenaline/adrenaline.skprx") ) {
				sprintf(ADR_FOLDER, "ux0:pspemu/adrenaline");								// <-- the default ADR_FOLDER is switched here if an old installation is found
				print_color("OK\n\n", GREEN);
				ADR_PRX_INSTL = 1;
			} else {
				print_color("Not found!\n\n", YELLOW);
			}
		}
		
		/// read version file if existing (only there if installed via this installer)
		sprintf(buffer, "%s/version", ADR_FOLDER);
		if ( doesFileExist(buffer) ) {
			sprintf(ADR_VERSION, "%s", read_installed_adrenaline_version(buffer) );				// <-- ADR_VERSION is set here if file was found in ADR_FOLDER
		}
		
		/// Adrenaline installed to a PSP game in tai-config check?? ------- TODO (there could still be an ux0:pspemu/adr.. entry that wont be deleted automatically)
		printf("Checking ux0:tai/config.txt for adrenaline.. ");
		get_id_of_psp_game_that_adrenaline_is_installed_to(TAI_CONFIG, ADR_FOLDER);
			if ( PSP_GAME_ID[0] != '\0' ) { //a config installation was found!!

				printf("%s\n\n", PSP_GAME_ID);
				
				if ( ADR_PRX_INSTL ) { //if the prxs are installed we can continue with the checks..
					/// Does the game still exist though?!
					sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/EBOOT.PBP", PSP_GAME_ID); 
					printf("Checking for /PSP/GAME/%s.. ", PSP_GAME_ID);
					if ( !doesFileExist(buffer) ) { //game was deleted -> means everything in config is a leftover and wrong
						
						print_color("Not found!\n\n", RED);
						
						/// the game was deleted but adrenaline was still installed to it in the tai config.. clean up
						printf("Deleting old installation for %s.. ", PSP_GAME_ID);
						if ( delete_adrenaline_from_config(TAI_CONFIG, ADR_FOLDER, PSP_GAME_ID) == 0 ) print_color("OK\n\n", GREEN);
						else print_color("Error\n\n", RED);
						
						/// also delete adrenaline prx files although it doesn't matter
						sprintf(buffer, "%s/adrenaline.skprx", ADR_FOLDER);
						sceIoRemove(buffer);
						sprintf(buffer, "%s/adrenaline.suprx", ADR_FOLDER);
						sceIoRemove(buffer);
						
						PSP_GAME_ID[0] = 0; //clear it to continue
						
					} else { //installed to config and the game does exist
						print_color("OK\n\n", GREEN);
						ADR_INSTALLED = 1;																// <-- ADR_INSTALLED can now be set to 1 because everything needed is there and works
					}
				} else { //..if not we can delete from the config and don't have to care about the basgame aso
					
					/// delete from the config as the files aren't even installed
					printf("Removing old entry in ux0:tai/config.txt.. ");
					int ret = delete_adrenaline_from_config(TAI_CONFIG, ADR_FOLDER, PSP_GAME_ID);
					if ( ret == 0 )	print_color("OK\n\n", GREEN);
					else print_color("ERROR\n\n", YELLOW);
					
					PSP_GAME_ID[0] = 0; //clear it to continue
					
				}
			} else { //No Adrenaline stuff found in tai Confg.txt
				
				print_color("Not found!\n\n", YELLOW);
			}	
		
		
	// Now the Adrenaline semi-needed checks (for stuff in the EasyInstaller itself) ///////////////////////////////////////////////////////////////////////
	
		/// check for Adrenaline flash files	
		printf("Checking for installed Adrenaline flash files.. ");
		sprintf(buffer, "%s/flash0", ADR_FOLDER);
		if ( doesDirExist(buffer) ) {
			print_color("OK\n\n", GREEN);	
			more_menu[1].type = MENU_ACTIVE; //activate "Delete flash files"
		} else {
			print_color("Not found!\n\n", YELLOW);
		}

		/// check for 661.PBP
		sprintf(buffer, "%s/661.PBP", ADR_FOLDER);
		printf("Checking for %s.. ", buffer);
		if ( doesFileExist(buffer) ) {
			
			/// check for correct/complete/valid update file
			if ( getFileSize(buffer) == UPDATE_SIZE ) {
				more_menu[2].type = MENU_ACTIVE; //activate "Delete Update PBP"
				print_color("OK\n\n", GREEN);
			} else {
				print_color("Wrong Size! Deleting..\n\n", RED);
				sceIoRemove(buffer);
			}
		} else {
			print_color("Not found!\n\n", YELLOW);
		}
		
		/// check for Adrenaline savestates	
		//printf("Checking for installed Adrenaline flash files.. ");
		if ( doesDirExist("ux0:pspemu/PSP/SAVESTATE") ) {
			//print_color("OK\n\n", GREEN);	
			more_menu[3].type = MENU_ACTIVE; //activate 
		} else {
			//print_color("Not found!\n\n", YELLOW);
		}
		
		///check for sqlite theme installing activate or not
		sprintf(buffer, "ur0:appmeta/%s/temp", PSP_GAME_ID);
		if ( doesDirExist(buffer) ) {
			theme_menu[3].type = MENU_ACTIVE; //activate "Uninstall Theme sqlite"
		} else {
			///check if the livearea folder exists. Otherwise the games livearea needs to be opened once
			sprintf(buffer, "ur0:appmeta/%s/livearea", PSP_GAME_ID);
			if ( doesDirExist(buffer) ) {
				theme_menu[2].type = MENU_ACTIVE; //activate "Install Theme sqlite"
			}
		}
	
	
	
	// And finally : Activating or Blocking more options depending on the checks /////////////////////////////////////////////////////////////////////////		
			
		if ( ADR_INSTALLED ) {
			installer_menu[2].type = MENU_ACTIVE; 	//activate "Uninstall Adrenaline" option
			recovery_menu[1].type = MENU_ACTIVE; 	//activate "Theme Menu"
			
				/// check for theme backup file (means a livearea theme must be installed if found)
				sprintf(buffer, "ur0:appmeta/%s/icon0_.dds", PSP_GAME_ID);
				if ( doesFileExist(buffer) ) theme_menu[1].type = MENU_ACTIVE; //file found -> activate "Uninstall Theme" option
				else theme_menu[0].type = MENU_ACTIVE; //not found -> activate "Install Theme" option
				
				/// check if currently installed version is also the newest available
				if ( strcmp(ADR_VERSION, files_menu[0].arg ) == 0 ) 
				installer_menu[0].type = MENU_BLOCKED; //block "Install newest Adrenaline" option if is already newest 
		}	
		
	printf("\n\nDone! Starting..");

	if ( debug ) sceKernelDelayThread(5000 * 1000); // 5 sec delay
	
	return 1;


	error_exit: /// If the important checks fail you will end up here and the Installer will exit
	
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) printf("Press X to exit...\n\n");
		else printf("Press O to exit...\n\n");
	
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) break;
		}
	return 0;
}

	
	
	
void recovery_draw_main() {

	int options = 0;		//available options
	int selection = 0;		//selected option
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
	
		/// calculate number of options
		while(recovery_menu[options].title != NULL) options++;
		
		/// Error handling
		if ( showoptions > options ) showoptions = options; //Adjust showoptions (there can't be more "showoptions" than "options" itself)
		
		/// set cursor to first active option
		while(recovery_menu[selection].type == MENU_BLOCKED) selection += 1;
		
			
	while (running) {
	
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();	
		
		
		/// "Debug" messages
		if ( debug ) {
			vita2d_pgf_draw_textf(pgf, 640, 100, GREY, 1.0f, "options = %i", options);
			vita2d_pgf_draw_textf(pgf, 640, 125, GREY, 1.0f, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 640, 150, GREY, 1.0f, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 640, 175, GREY, 1.0f, "showoptions = %i", showoptions);
			
			vita2d_pgf_draw_textf(pgf, 600, 250, GREY, TEXT_SIZE, "PSP_GAME_ID = %s", PSP_GAME_ID);
			vita2d_pgf_draw_textf(pgf, 600, 275, GREY, TEXT_SIZE, "gamesfound = %i", gamesfound);
			
			vita2d_pgf_draw_textf(pgf, 550, 325, GREY, TEXT_SIZE, "ADR_FOLDER = %s", ADR_FOLDER);
			vita2d_pgf_draw_textf(pgf, 550, 350, GREY, TEXT_SIZE, "ADR_VERSION = %s", ADR_VERSION);
			vita2d_pgf_draw_textf(pgf, 550, 375, GREY, TEXT_SIZE, "ADR_INSTALLED = %i", ADR_INSTALLED);
			vita2d_pgf_draw_textf(pgf, 550, 400, GREY, TEXT_SIZE, "ADR_PRX_INSTL = %i", ADR_PRX_INSTL);
			
			vita2d_draw_line(0,  70, 960,  70, GREY); //top line
			vita2d_draw_line(0, 410, 960, 410, GREY); //bot line
			vita2d_draw_line(100, 0, 100, 544, GREY); //left line
			vita2d_draw_line(860, 0, 860, 544, GREY); //right line
		}
		
		
		/// draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, MAIN_TITLE)/2), 40, text_color, TEXT_SIZE_BIG, MAIN_TITLE);
		vita2d_pgf_draw_textf(pgf, SCREEN_WIDTH/2+(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, MAIN_TITLE)/2),  40, GREY, TEXT_SIZE, " v%s", VERSION);
		
		
		/// draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	
		if ( selection < toption ) toption = selection;	//for scrolling up with selection
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
		
		/// draw Button Controls 
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
			if ( debug ) debug = 0;	
			else debug = 1;
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
		} 
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
}






void *uninstall_adrenaline() {
	
	sprintf(buffer, "Everything in \"%s/\" will be deleted!", ADR_FOLDER);
	int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to continue uninstalling Adrenaline?", buffer);
	if ( ret ) {
		closeVita2DLib(); //shutdown Vita2dLib
		//lock_psbutton();
		uninstall_adrenaline_files(PSP_GAME_ID);
	}
	
	return 0;
}

void *draw_psp_games(char *arg, char *arg2) {

	int options = gamesfound;	
	int selection = 0;		//selected option number
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
		if( strcmp(arg, "latest") == 0 ) { //set to latest adrenaline version
			arg = files_menu[0].arg; 	//name
			arg2 = files_menu[0].arg2;	//folder
		}
		
		if ( showoptions > options ) showoptions = options; //error adjust showoptions (there can't be more "showoptions" than "options" itself)	
	
	while (running) {
	
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();		
		
		
		/// Draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, "Select a basegame")/2), 40, text_color, TEXT_SIZE_BIG, "Select a basegame");
		
		
		/// "Debug" Messages
		if ( debug ) {
			vita2d_pgf_draw_textf(pgf, 640, 100, GREY, TEXT_SIZE, "options = %i", options);
			vita2d_pgf_draw_textf(pgf, 640, 125, GREY, TEXT_SIZE, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 640, 150, GREY, TEXT_SIZE, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 640, 175, GREY, TEXT_SIZE, "showoptions = %i", showoptions);
			
			vita2d_pgf_draw_textf(pgf, 640, 225, GREY, TEXT_SIZE, "arg = %s", arg);
			vita2d_pgf_draw_textf(pgf, 640, 250, GREY, TEXT_SIZE, "arg2 = %s", arg2);
			
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
				
				if ( strcmp(PSP_GAME_ID, content_array[selection].titleID ) == 0 ) { //make the basegame which is used for adrenaline colored
					vita2d_pgf_draw_textf(pgf, x, y, menu_text_basegame, TEXT_SIZE_BIG, "%s - %s", content_array[selection].titleID, content_array[selection].title);
				} else {	
					vita2d_pgf_draw_textf(pgf, x, y, menu_text_active_color, TEXT_SIZE_BIG, "%s - %s", content_array[selection].titleID, content_array[selection].title);
				}
			} else { //draw all other options
				
				if ( strcmp(PSP_GAME_ID, content_array[i].titleID ) == 0 ) { //make the basegame which is used for adrenaline colored
					vita2d_pgf_draw_textf(pgf, x, y, menu_text_basegame, TEXT_SIZE_BIG, "%s - %s", content_array[i].titleID, content_array[i].title);
				} else {
					vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE_BIG, "%s - %s", content_array[i].titleID, content_array[i].title);
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
			
				int ret;
				sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", content_array[selection].titleID);
				if ( doesFileExist(buffer) ) { /// existing PBOOT check
					sprintf(buffer, "This game already got a \"%s\" PBOOT.PBP installed!", getEbootTitle(buffer) );
					ret = recovery_draw_dialog(MAIN_TITLE, buffer, "Continue anyway and overwrite it?");
					if ( ret ) goto install;
					
				} else {
					ret = recovery_draw_dialog(MAIN_TITLE, content_array[selection].title, "Do you really want to continue installing to this Game?");
					if ( ret ) {
						install:
						closeVita2DLib(); //shutdown Vita2dLib
						//lock_psbutton();
						
						if ( ADR_INSTALLED ) { // if adrenaline is fully installed already
						
							if ( strcmp(arg2, ADR_FOLDER ) == 0 ) {
								change_adrenaline_files(content_array[selection].titleID, arg, arg2, 0); //simply update files (no reboot)	
							} else { // paths are different -> delete old stuff -> flag 1
								change_adrenaline_files(content_array[selection].titleID, arg, arg2, 1); //
							}
							
						} else {
							install_adrenaline_files(content_array[selection].titleID, arg); //full install + reboot
						}
					}
				}
		}
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return 0;
}

void *files_option(char *arg) {
	recovery_draw_submenu(files_menu, "Select Adrenaline Release", arg);
	return 0;
}

void *installer_options(char *arg) {
	recovery_draw_submenu(installer_menu, MAIN_TITLE, arg);
	return 0;
}


void *theme_options(char *arg) {
	recovery_draw_submenu(theme_menu, "Theming Options (optional)", arg);
	return 0;
}

	void *install_theme() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to continue installing the theme?", "A reboot will be required!");
		if ( ret ) {
			closeVita2DLib(); //shutdown Vita2dLib
			//lock_psbutton();
			install_theme_files(PSP_GAME_ID);
		}
		return 0;
	}

	void *uninstall_theme() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to restore the original livearea theme?", "A reboot will be required!");
		if ( ret ) {
			closeVita2DLib(); //shutdown Vita2dLib
			//lock_psbutton();
			uninstall_theme_files(PSP_GAME_ID);
		}
		return 0;
	}

	void *install_sqlite_theme() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to continue installing?", "");
		if ( ret ) {
			recovery_draw_statusmessage(MAIN_TITLE, "Installing.. Please wait..");
			ret = add_livearea_style(PSP_GAME_ID);
			if ( ret ) { //successfull
				theme_menu[3].type = MENU_ACTIVE; 
				theme_menu[2].type = MENU_BLOCKED;
				recovery_draw_message(MAIN_TITLE, "The LiveArea has been modified successfully!", "Okay");
			} else {
				recovery_draw_message(MAIN_TITLE, "An error occured!", "Okay");
			}
		}
		return 0;
	}

	void *uninstall_sqlite_theme() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to remove the LiveArea frames?", "");
		if ( ret ) {
			recovery_draw_statusmessage(MAIN_TITLE, "Removing.. Please wait..");
			ret = remove_livearea_style(PSP_GAME_ID);
			if ( ret ) { //successfull
				theme_menu[2].type = MENU_ACTIVE;
				theme_menu[3].type = MENU_BLOCKED;
				recovery_draw_message(MAIN_TITLE, "The LiveArea has been restored successfully!", "Okay");
			} else {
				recovery_draw_message(MAIN_TITLE, "An error occured!", "Okay");
			}
		}
		return 0;
	}


	
	
	
void *more_options(char *arg) {

	recovery_draw_submenu(more_menu, "Advanced Options", arg);
	return 0;
}

	/// installs a small basegame
	void *install_pspgame() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to continue installing a PSP Game?", "After that you will be able to install Adrenaline to it!");
		if ( ret ) {
			closeVita2DLib(); //shutdown Vita2dLib
			//lock_psbutton();
			install_psp_basegame();
		}
	
		return 0;
	}

	/// This will delete the adrenaline flash files
	int *option_delete_flash() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to delete the flash files?","");
		if ( ret ) {
			recovery_draw_statusmessage(MAIN_TITLE, "Deleting flash files.. Please wait.."); //needs error catching TODO
				sprintf(buffer, "%s/adrenaline.bin", ADR_FOLDER);
				sceIoRemove(buffer);
				sprintf(buffer, "%s/flash0", ADR_FOLDER);
				removePath(buffer);
				sprintf(buffer, "%s/flash1", ADR_FOLDER);
				removePath(buffer);
				
			recovery_draw_message(MAIN_TITLE, "The flash files were deleted successfully!", "Okay");
			more_menu[1].type = MENU_BLOCKED; //deactivate again
		}
	
		return 0;
	}

	/// This will delete 661.PBP in ADR_FOLDER
	int *option_delete_pbp() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to delete the 6.61 Update file?", "This will save you about 32MB of diskspace.");
		if ( ret ) {
			recovery_draw_statusmessage(MAIN_TITLE, "Deleting.. Please wait..");
			sprintf(buffer, "%s/661.PBP", ADR_FOLDER);
			ret = sceIoRemove(buffer);
			if ( ret < 0 ) {
				recovery_draw_message(MAIN_TITLE, "An error occured!", "Okay");
			} else {
				recovery_draw_message(MAIN_TITLE, "The Update file has been deleted!", "Okay");
				more_menu[2].type = MENU_BLOCKED; //deactivate again
			}
		}
		return 0;
	}
	
	/// This will delete the savestates folder
	int *option_delete_savestates() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to delete all your SaveStates?", "(ux0:pspemu/PSP/SAVESTATE/)");
		if ( ret ) {
			recovery_draw_statusmessage(MAIN_TITLE, "Deleting.. Please wait..");
			ret = removePath("ux0:pspemu/PSP/SAVESTATE");
			if ( ret == 1 ) {
				recovery_draw_message(MAIN_TITLE, "All SaveStates have been deleted!", "Okay");
				more_menu[3].type = MENU_BLOCKED; //deactivate again
			} else {
				recovery_draw_message(MAIN_TITLE, "An error occured!", "Okay");	
			}
		}
		return 0;
	}
	
	int *option_show_changelogfile() {
		recovery_draw_textfile("Adrenaline Update-History", "app0:files/updatehistory.txt"); 
		return 0;
	}
	
	int *option_show_taiconfig() {
		recovery_draw_textfile(TAI_CONFIG, TAI_CONFIG);
		return 0;
	}
	
	
	
		//for testing purposes only
		int option_test() {
			closeVita2DLib(); //shutdown Vita2dLib			
			psvDebugScreenInit();
			psvDebugScreenClear(BLACK);	
			
			/**int res;
			int ret;
			int state;
		
			printf("trying to promote backup folder\n-------------------------------\n\n");	
			
			/// loadScePaf
			uint32_t ptr[0x100] = { 0 };
			ptr[0] = 0;
			ptr[1] = (uint32_t)&ptr[0];
			uint32_t scepaf_argp[] = { 0x400000, 0xEA60, 0x40000, 0, 0 };
			sceSysmoduleLoadModuleInternalWithArg(0x80000008, sizeof(scepaf_argp), scepaf_argp, ptr);
			
			/// Backup Promote
			ret = sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_PROMOTER_UTIL); //
			printf("sceSysmoduleLoadModuleInternal: 0x%x\n", ret);					//0x8002D003 -> SCE_KERNEL_ERROR_MODULEMGR_NO_LIB
			
			ret = scePromoterUtilityInit();
			printf("scePromoterUtilityInit: 0x%x\n", ret);

			printf("\n\n");
			
			ret = scePromoterUtilityCheckExist("VITASHELL", 0);
			printf("scePromoterUtilityCheckExist(\"VITASHELL\") 0x%x\n", ret);	// 0x80AF008F -> unknown error code
			
			ret = scePromoterUtilityCheckExist("PCSF00103", 0);
			printf("scePromoterUtilityCheckExist(\"PCSF00103\") 0x%x\n", ret);
			
			ret = scePromoterUtilityCheckExist("NPEZ00104", 0);
			printf("scePromoterUtilityCheckExist(\"NPEZ00104\") 0x%x\n", ret);
			
			printf("\n\n");
			
			ret = scePromoterUtilityPromoteBackup("ux0:test/NPUZ00097");
			printf("scePromoterUtilityPromoteBackup: 0x%x\n", ret);					//0x80101102 -> unknown error code
			
			printf("\n");
			
			state = 1;
			do	{
				ret = scePromoterUtilityGetState(&state);
				if (ret < 0) {
					printf("scePromoterUtilityGetState: error = 0x%x\n", ret);
					while(1);
				}
				printf("scePromoterUtilityGetState: status = 0x%x\n", ret);
				sceKernelDelayThread(1000000);
			} while (state);

			ret = scePromoterUtilityGetResult(&res);
			printf("scePromoterUtilityGetResult: ret=0x%x res=0x%x\n", ret, res);

			printf("\n");
			
			ret = scePromoterUtilityExit();
			printf("scePromoterUtilityExit: 0x%x\n", ret);

			ret = sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_PROMOTER_UTIL);
			printf("sceSysmoduleUnloadModuleInternal: 0x%x\n", ret);**/

	
	
			while(1);
			return 0;
		}
	

	
	/// This will trigger "Rebuilding Database" by deleting ur0:shell/db/app.db + magic!
	int *option_reloadTaiConfig() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to reload the taiHENkaku config?","");
		if ( ret ) {
			ret = taiReloadConfig();
			if ( ret == 0) recovery_draw_message(MAIN_TITLE, "Config reloaded successfully!", "Okay");
			else recovery_draw_message(MAIN_TITLE, "An error occured!", "Okay");
		}
			
		return 0;
	}
	
	/// This will trigger "Rebuilding Database" by deleting ur0:shell/db/app.db + magic!
	int *option_rebuildDatabase() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to rebuild the Database?","Your icon layout will be lost!");
		if ( ret ) {
			trigger_rebuild_database();
			scePowerRequestColdReset();
		}
		return 0;
	}


	/// This will trigger "Updating Database" by deleting ux0:id.dat from the Memory Card!
	int *option_updateDatabase() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to update the Database?","This will keep your icon layout unimpaired!");
		if ( ret ) {
			trigger_update_database();
			scePowerRequestColdReset();
		}
		return 0;
	}

	/// This will reboot the PS Vita System!
	int *option_reboot() {
		int ret = recovery_draw_dialog(MAIN_TITLE, "Do you really want to reboot the Vita System?","");
		if ( ret ) {
			scePowerRequestColdReset();
		}	
		return 0;
	}
	
	
	
	

void recovery_draw_submenu(Menu *sub_menu, const char* title, char *arg) {

	int options = 0;		//available options (initialized with 0)
	int selection = 0;		//selected option number
	int running = 1;		//loop variable
	int toption = 0;		//the option on top when there are more options than showoptions
	int showoptions = 5;	//the number of options that should be displayed at once [5 by Default]
	int i, x, y, z;
	
		while(sub_menu[options].title != NULL) {
			/// special cases here
			if ( strcmp(sub_menu[options].arg, ADR_VERSION) == 0 ) 
				sub_menu[options].type = MENU_BLOCKED; //block already installed adrenaline version for version selector

			options++; //calculate number of options
		}
		
		while(sub_menu[selection].type == MENU_BLOCKED) selection += 1; //get first Active option
	
		if ( showoptions > options ) showoptions = options; //Adjust showoptions (there can't be more "showoptions" than "options" itself)	
		
	
	while (running) {
	
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();
		
			while(sub_menu[selection].type == MENU_BLOCKED) selection += 1; //jump to next active option if it has been blocked and you returned here (this is save here as "back is always active")
		
			
		/// Draw the Title
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
		
		/// "Debug" Messages
		if ( debug ) {
			vita2d_pgf_draw_textf(pgf, 640, 100, GREY, TEXT_SIZE, "options = %i", options);
			vita2d_pgf_draw_textf(pgf, 640, 125, GREY, TEXT_SIZE, "selection = %i", selection);
			vita2d_pgf_draw_textf(pgf, 640, 150, GREY, TEXT_SIZE, "toption = %i", toption);
			vita2d_pgf_draw_textf(pgf, 640, 175, GREY, TEXT_SIZE, "showoptions = %i", showoptions);
			
			vita2d_pgf_draw_textf(pgf, 640, 225, GREY, TEXT_SIZE, "arg = %s", arg);
			
			vita2d_pgf_draw_textf(pgf, 550, 350, GREY, TEXT_SIZE, "ADR_VERSION = %s", ADR_VERSION);
			
			vita2d_draw_line(0,  70, 960,  70, GREY); //top line
			vita2d_draw_line(0, 410, 960, 410, GREY); //bot line
			vita2d_draw_line(100, 0, 100, 544, GREY); //left line
			vita2d_draw_line(860, 0, 860, 544, GREY); //right line
		}
			
		/// Draw the Menu		
		x = 125; //horizontal start of options (text)
		y = 110; //vertical start of options (text)
		z = 70;  //spacing	
		if ( selection < toption ) toption = selection;	 //scrolling up with selection
		while ( sub_menu[toption].type == MENU_BLOCKED && options - toption > showoptions ) toption++; //scroll down as much as possible if top option is blocked
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
					function(sub_menu[selection].arg, sub_menu[selection].arg2);
				
				vita2d_start_drawing();
				vita2d_clear_screen();
			}	
		} else if ( sub_menu[selection].type == MENU_EXIT ) {
			if (pressed_buttons & SCE_CTRL_ENTER) running = 0;
			
		}
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
}


void recovery_draw_textfile(const char* title, const char* path) { //very simple
	
	int running = 1;
	int current_line = 0;
	int showlines = 19;
	double i, j;
	int x, y, z;
	int lines = 0;
	
	FILE* textfile = fopen(path, "r");
	
	if (textfile == NULL) { //cannot read file
			while (running) {
				readPad();
				vita2d_start_drawing();
				vita2d_clear_screen();
			
				/// Draw the Title 
				vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
				
				/// draw error message
				vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE, "Error opening file")/2), SCREEN_HEIGHT/2, RED, TEXT_SIZE, "Error opening file!");
				
				
				/// Button Controls
				x = 15; //horizontal start
				y = 525; //vertical start
				z = 25;  //spacing
				vita2d_draw_texture(button_cancel, x, y-17);
				vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Back");	
				
				if ( pressed_buttons & SCE_CTRL_CANCEL) running = 0;	
				
				vita2d_end_drawing();
				vita2d_swap_buffers();
				sceDisplayWaitVblankStart();
			}

	} else { //file opened
		
		while (fgets(big_buffer, sizeof(big_buffer), textfile) != NULL) { //get file size (number of lines)
			lines++;
		}
		rewind(textfile);
		
			if ( lines < showlines ) showlines = lines; //fix
	
		while (running) {
			readPad();
			vita2d_start_drawing();
			vita2d_clear_screen();
		
			/// Draw the Title 
			vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
			
			/// "Debug" Messages
			if ( debug ) {
				vita2d_pgf_draw_textf(pgf, 680, 400, GREY, TEXT_SIZE, "lines = %i", lines); //draw Message //~180 in original menu
				vita2d_pgf_draw_textf(pgf, 680, 425, GREY, TEXT_SIZE, "current_line = %i", current_line); //draw Message //~180 in original menu
				vita2d_pgf_draw_textf(pgf, 680, 450, GREY, TEXT_SIZE, "showlines = %i", showlines); //draw Message //~180 in original menu
				
				vita2d_draw_line(0,  65, 960,  65, GREY); //top line
				vita2d_draw_line(0, 480, 960, 480, GREY); //bot line
				//vita2d_draw_line(100, 0, 100, 544, GREY); //left line
				//vita2d_draw_line(860, 0, 860, 544, GREY); //right line
			}
			
			/// Menu /////////////////////////////////////////////////////////////////////////////////////////////////////
			x = 45;
			y = 100;
			z = 20; //line spacing
			for(i=0; i < current_line; i++) fgets(big_buffer, sizeof(big_buffer), textfile); //using fseek might be smarter D: TODO
			for(i=current_line; i < showlines+current_line; i++, y += z) {
				fgets(big_buffer, sizeof(big_buffer), textfile);
				vita2d_pgf_draw_textf(pgf, x, y, text_color, TEXT_SIZE, "%s", big_buffer );
			}
			rewind(textfile);
			
			/// Scrollbar
			x = 915;
			y = 80;
			z = 400; //height
			j = z / 100 * ( showlines*100/lines); //height of cursor
			i = y + ( current_line * ( (400 - j) / (lines-showlines) )); //position of cursor
			
			if ( lines > showlines ) {
				vita2d_draw_rectangle(x, y, 8, z, menu_scrollbar_background); //x, y, width, height (z X-px is small math fix)
				vita2d_draw_rectangle(x, i, 8, j, menu_scrollbar); //x, y, width, height
			}
				if ( debug ) {
					vita2d_pgf_draw_textf(pgf, 680, 500, GREY, TEXT_SIZE, "i = %f", i); 
					vita2d_pgf_draw_textf(pgf, 680, 525, GREY, TEXT_SIZE, "j = %f", j); 
				}
				
			/// Button Controls
			x = 15; //horizontal start
			y = 525; //vertical start
			z = 25;  //spacing
			vita2d_draw_texture(button_cancel, x, y-17);
			vita2d_pgf_draw_text(pgf, x+26,  y, WHITE, TEXT_SIZE, "Back");	
			if ( lines > showlines ) {
				x += 26+vita2d_pgf_text_width(pgf, TEXT_SIZE, "Back")+z;	
				vita2d_draw_texture(button_pad, x, y-17);
				vita2d_pgf_draw_text(pgf, x+26, y, WHITE, TEXT_SIZE, "Navigate");
			}
			
				/// Navigation 
				if ( hold_buttons & SCE_CTRL_UP) {
					if( current_line > 0 ) current_line--;				
				}
				if ( hold_buttons & SCE_CTRL_DOWN) 
					if( lines - current_line > showlines ) current_line++;
				
				if ( pressed_buttons & SCE_CTRL_CANCEL) running = 0;	

					if ( debug ) {
						if ( pressed_buttons & SCE_CTRL_LTRIGGER) showlines--;
						if ( pressed_buttons & SCE_CTRL_RTRIGGER) showlines++;
					}
					
			vita2d_end_drawing();
			vita2d_swap_buffers();
			sceDisplayWaitVblankStart();
		}	
	}
	
	fclose(textfile);
}



int recovery_draw_dialog(const char* title, const char* message, const char* message2) {
	
	int running = 1;
	int choice = 0;
	int x, y, z;
		
	while (running) {
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
		
		
		/// Menu /////////////////////////////////////////////////////////////////////////////////////////////////////
		
		x = 310; //horizontal start of left button	//310 original
		y = 420; //vertical start of buttons		//450 original
		z = 290; //row spacing						//290
			
		vita2d_pgf_draw_textf(pgf, 960/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message)/2), 220, text_color, TEXT_SIZE_BIG, "%s", message); //draw Message //~180 in original menu
		vita2d_pgf_draw_textf(pgf, 960/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message2)/2), 265, text_color, TEXT_SIZE_BIG, "%s", message2); //draw Message //~180 in original menu
			
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
			
		if ( pressed_buttons & SCE_CTRL_ENTER) running = 0; //Ok	
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	
	if ( choice == 1 ) return 1;
	else return 0;
}




int recovery_draw_message(const char* title, const char* message, const char* button_text) { 
		
	int running = 1;
	int x, y, z, q;

	while (running) {
		readPad();
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
		
		
		/// Menu /////////////////////////////////////////////////////////////////////////////////////////////////////
		
		x = SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, button_text)/2); //for centered position
		y = 400; //vertical start of button	
		q = 60; //button add. space left & right
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
		
		/// Navigation		
		if ( pressed_buttons & SCE_CTRL_ENTER) running = 0;		
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	
	return 0;
}


int recovery_draw_statusmessage(const char* title, const char* message) { 
	int i;
	for(i = 0; i < 3; i++) {
		vita2d_start_drawing();
		vita2d_clear_screen();
	
		/// Draw the Title 
		vita2d_pgf_draw_text(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, title)/2), 40, text_color, TEXT_SIZE_BIG, title);
		
		/// draw Message
		vita2d_pgf_draw_textf(pgf, SCREEN_WIDTH/2-(vita2d_pgf_text_width(pgf, TEXT_SIZE_BIG, message)/2), 220, text_color, TEXT_SIZE_BIG, "%s", message);		
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
		sceDisplayWaitVblankStart();
	}
	return 0;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void install_adrenaline_files(char *id, char *arg) {

	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
		
	printf("Installing Adrenaline\n---------------------\n\n");	
	printf("release vers.: %s\n", arg);
	printf("installing to: %s\n\n\n", id);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
		/* Changing installation folder if older version */
		if ( strcmp(arg, "adrenaline") == 0 || strcmp(arg, "adrenaline_fix2") == 0 ) 
			sprintf(ADR_FOLDER, "ux0:pspemu/adrenaline");
	
	
	/// Checking dir
	printf("Checking folder %s/.. ", ADR_FOLDER);
	ret = doesDirExist(ADR_FOLDER);
	if ( ret == 1 )	print_color("Found!\n\n", GREEN);	
	else {	
		print_color("Not found!\n\n", YELLOW);	
		
		/// make dir
		printf("Creating folder %s/.. ", ADR_FOLDER);
		ret = sceIoMkdir(ADR_FOLDER, 0777);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	}
	
	/// adrenaline.suprx
	sprintf(buffer, "%s/adrenaline.suprx", ADR_FOLDER);
	sprintf(big_buffer, "app0:files/releases/%s/adrenaline.suprx", arg);
	printf("Writing %s.. ", buffer);
	ret = copyFile(big_buffer, buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// adrenaline.skprx
	sprintf(buffer, "%s/adrenaline.skprx", ADR_FOLDER);
	sprintf(big_buffer, "app0:files/releases/%s/adrenaline.skprx", arg);
	printf("Writing %s.. ", buffer);
	ret = copyFile(big_buffer, buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// usbdevice.skprx
	sprintf(buffer, "%s/usbdevice.skprx", ADR_FOLDER);
	sprintf(big_buffer, "app0:files/releases/%s/usbdevice.skprx", arg);
	if( doesFileExist(big_buffer) ) {
		printf("Writing %s.. ", buffer);
		ret = copyFile(big_buffer, buffer);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
	}
	
	/// PBOOT.PBP
	sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
	printf("Writing %s.. ", buffer);
	ret = copyFile("app0:files/PBOOT.PBP", buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// writing to tai config
	printf("Editing ux0:tai/config.txt.. ");
	ret = write_adrenaline_to_config(TAI_CONFIG, ADR_FOLDER, id);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// write version file to adrenaline folder
	sprintf(buffer, "%s/version", ADR_FOLDER);
	printf("Writing %s.. ", buffer);
	ret = write_installed_adrenaline_version(buffer, arg);
	if ( ret ) print_color("OK\n\n", GREEN);
	else print_color("ERROR!\n\n", RED);
	
	/// 661.PBP
	sprintf(buffer, "%s/661.PBP", ADR_FOLDER);
	printf("Checking for %s.. ", buffer);
	ret = doesFileExist(buffer);
	if (ret == 1) print_color("OK\n\n", GREEN);
	else {
		print_color("Not found!\n\n", YELLOW);
						
		///not found and not copied from older folder -> download file 
		printf("Downloading 661.PBP.. ");
		ret = download_file(UPDATE_LINK, buffer);
		if (ret == 0) print_color("OK\n\n", GREEN);
		else {			
			print_color("ERROR!\n\n", RED);
				
			if ( doesFileExist(buffer) ) sceIoRemove(buffer); //cleaning broken file
			
			sprintf(buffer, "\nPlace a 661.PBP to %s/661.PBP manually!\n\n", ADR_FOLDER);
			print_color(buffer, YELLOW);
		}
	}
	
	
	/// create folders & stock stuff
	printf("Creating eCFW folders and files for you.. ");
	createMS0structure();
	printf("Done\n\n");		
	
	//unblock_psbutton();
	
	/// This is for PSTV with uma0 mounted as ux0
	/*if ( vshSblAimgrIsVITA() == 1 ) { //its a PSTV
		psvDebugScreenClear(BLACK);	
		printf("Are you using an external USB device as memory card?\n");
		printf("X = Yes   O = No\n\n");
		
		while (1) {
		readPad();
		if (pressed_buttons & SCE_CTRL_CROSS) {
			//sceKernelDelayThread(1000 * 1000); // 1 sec
			//reload config.txt
			//exit
		} else if (pressed_buttons & SCE_CTRL_CIRCLE) {
			break;
		}
		if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}		
	}*/
	
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



void change_adrenaline_files(char *id, char *arg, char *arg2, int mode) {

	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
	printf("Updating Adrenaline\n-------------------\n\n");	
	printf("release vers.: %s\n", arg);
	printf("installing to: %s\n\n\n", id);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
		
	// Different Adrenaline path	
	if ( mode ) { //older adrenaline installation is in different folder DELETE old first
		
		/// make new dir if it shouldnd exist already 
		sceIoMkdir(arg2, 0777);
		
		/// deleting old installation from tai config
		printf("Removing old entry in ux0:tai/config.txt.. ");
		ret = delete_adrenaline_from_config(TAI_CONFIG, ADR_FOLDER, PSP_GAME_ID);
		if ( ret == 0 )	print_color("OK\n", GREEN);
		else print_color("ERROR\n", RED);
		
		/// 661.PBP
		sprintf(buffer, "%s/661.PBP", ADR_FOLDER);
		sprintf(big_buffer, "%s/661.PBP", arg2);
		printf("Checking for %s.. ", buffer);
		ret = doesFileExist(buffer);
		if (ret == 1) {
			print_color("OK\n", GREEN);
			
			/// copy to new adren folder
			printf("Copying to %s.. ", big_buffer);
			ret = copyFile(buffer, big_buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);
			
		} else {
			print_color("Not found!\n", YELLOW);
		}
		
		/// Removing old adrenaline folder
		printf("Removing %s.. ", ADR_FOLDER);
		ret = removePath(ADR_FOLDER);
		if ( ret == 1 )	print_color("OK\n", GREEN);
		else print_color("ERROR\n", RED);
		
		/// Changing installation folder here
		sprintf(ADR_FOLDER, arg2);
		
		printf("\n");
							
	}
	
	// Different Basegame
	if ( strcmp(PSP_GAME_ID, id) != 0 ) { //if the basegame has changed
		
		/// delete PBOOT.PBP from old basegame if existing
		sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", PSP_GAME_ID);
		if ( doesFileExist(buffer) ) {		
			printf("Deleting %s.. ", buffer);
			ret = sceIoRemove(buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
		}
				
		/// PBOOT.PBP
		sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
		printf("Writing %s.. ", buffer);
		ret = copyFile("app0:files/PBOOT.PBP", buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		if ( mode == 0) {
			/// deleting old installation from tai config if not already done in "mode"
			printf("Editing ux0:tai/config.txt.. ");
			ret = delete_adrenaline_from_config(TAI_CONFIG, ADR_FOLDER, PSP_GAME_ID);
			if ( ret == 0 )	print_color("OK\n", GREEN);
			else print_color("ERROR\n", RED);
		}
		
		printf("\n");
	}	
	
	/// Checking dir
	printf("Checking folder %s/.. ", ADR_FOLDER);
	ret = doesDirExist(ADR_FOLDER);
	if ( ret == 1 )	print_color("Found!\n", GREEN);	
	else { //this should never be reachable though	
		print_color("Error!\n", RED);	
		
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nPress X to exit..\n", YELLOW); 
		else print_color("\n\nPress O to eixt..\n", YELLOW);

		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				sceKernelExitProcess(0);
			}	
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}		
	}
	
	// Delete everything that needs to be deleted (incompatible stuff between versions)
	
		/// adrenaline.bin
		sprintf(buffer, "%s/adrenaline.bin", ADR_FOLDER);
		if ( doesFileExist(buffer) ) {
			printf("Deleting %s.. ", buffer);
			ret = sceIoRemove(buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
		}	
		
		
		/* Not (maybe yet) needed
		/// flash0 folder
		sprintf(buffer, "%s/flash0", ADR_FOLDER);
		if ( doesDirExist(buffer) ) {
			printf("Deleting %s.. ", buffer);
			ret = removePath(buffer);
			if ( ret )	print_color("OK\n", GREEN);
			else print_color("ERROR\n", RED);
		}	
		
		/// flash1 folder
		sprintf(buffer, "%s/flash1", ADR_FOLDER);
		if ( doesDirExist(buffer) ) {
			printf("Deleting %s.. ", buffer);
			ret = removePath(buffer);
			if ( ret )	print_color("OK\n", GREEN);
			else print_color("ERROR\n", RED);
		}*/
		
		
	// Overwrite files
			
		/// adrenaline.suprx
		sprintf(buffer, "%s/adrenaline.suprx", ADR_FOLDER);
		sprintf(big_buffer, "app0:files/releases/%s/adrenaline.suprx", arg);
		printf("Writing %s.. ", buffer);
		ret = copyFile(big_buffer, buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);	
		
		/// adrenaline.skprx
		sprintf(buffer, "%s/adrenaline.skprx", ADR_FOLDER);
		sprintf(big_buffer, "app0:files/releases/%s/adrenaline.skprx", arg);
		printf("Writing %s.. ", buffer);
		ret = copyFile(big_buffer, buffer);
		if ( ret == 0 )	print_color("OK\n", GREEN);	
		else print_color("ERROR\n", RED);

		/// usbdevice.skprx
		sprintf(buffer, "%s/usbdevice.skprx", ADR_FOLDER);
		sprintf(big_buffer, "app0:files/releases/%s/usbdevice.skprx", arg);
		if( doesFileExist(big_buffer) ) {
			printf("Writing %s.. ", buffer);
			ret = copyFile(big_buffer, buffer);
			if ( ret == 0 )	print_color("OK\n\n", GREEN);	
			else print_color("ERROR\n\n", RED);	
		} else {
			if( doesFileExist(buffer) ) {
				printf("Deleting %s.. ", buffer);
				ret = sceIoRemove(buffer);
				if ( ret == 0 )	print_color("OK\n", GREEN);	
				else print_color("ERROR\n", RED);	
			}	
		}		
		
		/// write version file to adrenaline folder
		sprintf(buffer, "%s/version", ADR_FOLDER);
		printf("Writing %s file.. ", buffer);
		ret = write_installed_adrenaline_version(buffer, arg);
		if ( ret ) print_color("OK\n", GREEN);
		else print_color("ERROR!\n", RED);
	
		/// 661.PBP
		sprintf(buffer, "%s/661.PBP", ADR_FOLDER);
		printf("Checking for %s.. ", buffer);
		ret = doesFileExist(buffer);
		if (ret == 1) print_color("OK\n", GREEN);
		else {
			print_color("Not found!\n", YELLOW);
							
			///not found and not copied from older folder -> download file 
			printf("Downloading 661.PBP.. ");
			ret = download_file(UPDATE_LINK, buffer);
			if (ret == 0) print_color("OK\n", GREEN);
			else {			
				print_color("ERROR!\n", RED);
					
				if ( doesFileExist(buffer) ) sceIoRemove(buffer); //cleaning broken file
				
				sprintf(buffer, "\nPlace a 661.PBP to %s/661.PBP manually!\n\n", ADR_FOLDER);
				print_color(buffer, YELLOW);
			}
			
		}
		
	printf("\n");
	
	if ( mode | ( strcmp(PSP_GAME_ID, id) != 0 ) ) { //since the path and or basegame changed in tai/config.txt we need to reload it
		
		/// writing to tai config
		printf("Editing ux0:tai/config.txt.. ");
		ret = write_adrenaline_to_config(TAI_CONFIG, ADR_FOLDER, id);
		if ( ret == 0 )	print_color("OK\n", GREEN);
		else print_color("ERROR\n", RED);		
	}
	
	/// and reload it (although we might need to reboot anyways)
	printf("Reloading taiHEnkaku config.. ");
	ret = taiReloadConfig();
	if ( ret == 0) print_color("OK\n", GREEN);
	else print_color("ERROR\n", RED);	
		
	//unblock_psbutton();
	
	
	if ( strcmp(PSP_GAME_ID, id) != 0 ) { //since the basegame changed we need to refresh database to make the PBOOT take effect on the new game (theme will be deleted automatically this way)
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
		
	} else { // exiting is enough
		
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nDone! Press X to exit..\n", GREEN); 
		else print_color("\n\nDone! Press O to exit..\n", GREEN);

		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				sceKernelExitProcess(0);
			}	
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}	
	}		
}


void createMS0structure() {
	
	//ISO folders
	if ( !doesDirExist("ux0:pspemu/ISO") ) 	makePath("ux0:pspemu/ISO");
	if ( !doesDirExist("ux0:pspemu/ISO/VIDEO") ) makePath("ux0:pspemu/ISO/VIDEO");
	
	//Theme folder (.ptf)
	if ( !doesDirExist("ux0:pspemu/PSP/THEME") ) makePath("ux0:pspemu/PSP/THEME"); 

	//seplugins folder + Empty files
	if ( !doesDirExist("ux0:pspemu/seplugins") ) makePath("ux0:pspemu/seplugins");
	if ( !doesFileExist("ux0:pspemu/seplugins/VSH.TXT") ) createEmptyFile("ux0:pspemu/seplugins/VSH.TXT");
	if ( !doesFileExist("ux0:pspemu/seplugins/GAME.TXT") ) createEmptyFile("ux0:pspemu/seplugins/GAME.TXT");
	if ( !doesFileExist("ux0:pspemu/seplugins/POPS.TXT") ) createEmptyFile("ux0:pspemu/seplugins/POPS.TXT");
}


void uninstall_adrenaline_files(char *id) {

	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);
		
	printf("Uninstalling Adrenaline\n-----------------------\n\n");	
	printf("from: %s\n\n\n", id);
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
	
		/// Idea: Backing up 661 Update file to app0: of EasyInstaller for later? (not working I know)
		/*sprintf(buffer, "ux0:app/%s/files", TITLEID);
		if ( doesDirExist(buffer) ) {
			sprintf(buffer, "ux0:app/%s/files/661.PBP", TITLEID);
			if ( !doesFileExist(buffer) ) {
				printf("Backing up 661.PBP file to app0.. ");
				sprintf(big_buffer, "%s/661.PBP", ADR_FOLDER);
				ret = copyFile(big_buffer, buffer);
				if ( ret == 0 )	print_color("OK\n\n", GREEN);
				else print_color("ERROR\n\n", RED);
			}
		}*/
	
		/// Delete EVERYTHING (delete whole adrenaline folder) 
		printf("Deleting %s.. ", ADR_FOLDER);
		ret = removePath(ADR_FOLDER);
		if ( ret ) print_color("OK\n\n", GREEN);
		else print_color("ERROR\n\n", RED);
		
		
		/// PBOOT.PBP
		sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
		if ( doesFileExist(buffer) ) {		
			printf("Deleting %s.. ", buffer);
			ret = sceIoRemove(buffer);
			if ( ret == 0 )	print_color("OK\n\n", GREEN);	
			else print_color("ERROR\n\n", RED);	
		}
		
		/// writing to tai config
		printf("Editing ux0:tai/config.txt.. ");
		ret = delete_adrenaline_from_config(TAI_CONFIG, ADR_FOLDER, id);
		if ( ret == 0 )	print_color("OK\n\n", GREEN);
		else print_color("ERROR\n\n", RED);
	
		
		
		//unblock_psbutton();
	
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			print_color("\n\nDone! Press X to reboot..\n", GREEN); 
		} else {
			print_color("\n\nDone! Press O to reboot..\n", GREEN);
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				trigger_update_database(); 
				scePowerRequestColdReset();
			}	
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		}
}


void install_theme_files(char *id) {

	char temp_buffer2[256];
	int ret;
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
		
	printf("Installing Adrenaline Theme\n---------------------------\n\n");	
	printf("installing to: %s\n\n\n", id);	
	
	sceKernelDelayThread(1000 * 1000); // 1 sec
		
		
	/// Checking dir
	sprintf(buffer, "ur0:appmeta/%s/livearea", id);
	printf("Checking folder %s.. ", buffer);
	ret = doesDirExist(buffer);
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
		
		//unblock_psbutton();
		
		sprintf(buffer, "psgm:play?titleid=%s", id);
		sceAppMgrLaunchAppByUri(0x20000, buffer); //launch flag 0x40000 | open flag 0x20000
		
		sceKernelDelayThread(1000 * 1000); // 1 sec
		
		if ( SCE_CTRL_ENTER == SCE_CTRL_CROSS ) {
			printf("Press X to continue the installation..\n\n");
		} else {
			printf("Press O to continue the installation..\n\n");
		}
		
		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) {
				//lock_psbutton();
				break;
			}	
		}
		
	} else {
		print_color("Found!\n\n", GREEN);	
	}
	
	/// icon0.dds -> icon0_.dds (backup file for checks)
	sprintf(buffer, "ur0:appmeta/%s/icon0.dds", id);
	sprintf(temp_buffer2, "ur0:appmeta/%s/icon0_.dds", id);
	printf("Backing up /%s/icon0.dds.. ", id);  //printf("Backing up %s.. ", buffer);
	ret = copyFile(buffer, temp_buffer2);
	sceIoRemove(buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);
	else print_color("ERROR\n\n", RED);
	
	/// icon0.dds
	sprintf(buffer, "ur0:appmeta/%s/icon0.dds", id);
	printf("Writing /%s/icon0.dds.. ", id);  //printf("Writing %s.. ", buffer);
	ret = copyFile("app0:files/icon0.dds", buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);
		
	/// bg0.png
	sprintf(buffer, "ur0:appmeta/%s/livearea/contents/bg0.png", id);
	printf("Writing /%s/livearea/contents/bg0.png..", id); //printf("Writing %s.. ", buffer);
	ret = copyFile("app0:files/bg0.png", buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
	
	/// startup.png
	sprintf(buffer, "ur0:appmeta/%s/livearea/contents/startup.png", id);
	printf("Writing /%s/livearea/contents/startup.png.. ", id); //printf("Writing %s.. ", buffer);
	ret = copyFile("app0:files/startup.png", buffer);
	if ( ret == 0 )	print_color("OK\n\n", GREEN);	
	else print_color("ERROR\n\n", RED);	
		
	/// Checking PBOOT
	sprintf(buffer, "ux0:pspemu/PSP/GAME/%s/PBOOT.PBP", id);
	printf("Checking for PSP/GAME/%s/PBOOT.PBP.. ", id); //printf("Checking for %s.. ", buffer);
	ret = doesFileExist(buffer);
	if ( ret == 1 )	print_color("Found!\n\n", GREEN);
	else {	
		print_color("Not found!\n", YELLOW);	
		print_color("It seems you haven't installed Adrenaline via this app!\nThe bubble's name won't change!\n\n", YELLOW);	
	}	
	
	//unblock_psbutton();
		
		sceKernelDelayThread(1000 * 1000); // 1 sec
			
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


void uninstall_theme_files(char *id) {
	
	int ret;
	
	
	psvDebugScreenInit();
	psvDebugScreenClear(BLACK);	
	
	printf("Uninstalling Adrenaline Theme\n-----------------------------\n\n");	
	printf("from: %s\n\n\n", id);	
	
		///remove ur0 appmeta
		sprintf(buffer, "ur0:appmeta/%s", id);
		printf("Deleting %s.. ", buffer); 
		ret = removePath(buffer);
		if ( ret == 1 )	print_color("OK\n\n", GREEN);	
		else print_color("ERROR\n\n", RED);	
		
		
		sceKernelDelayThread(1000 * 1000); // 1 sec
	
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
		
		/// install in bgdl
		printf("Creating folder %s.. ", bgdl);
		ret = makePath(bgdl);	
		if ( ret == 0 )	{
			print_color("OK\n\n", GREEN);	
	
			/// d0.pdb
			sprintf(buffer, "%s/d0.pdb", bgdl);
			printf("Writing %s.. ", buffer);
			ret = copyFile("app0:files/basegame/bgdl/d0.pdb", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
		
			/// d1.pdb
			sprintf(buffer, "%s/d1.pdb", bgdl);
			printf("Writing %s.. ", buffer);
			ret = copyFile("app0:files/basegame/bgdl/d1.pdb", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);
			else print_color("ERROR\n", RED);
			
			/// f0.pdb
			sprintf(buffer, "%s/f0.pdb", bgdl);
			printf("Writing %s.. ", buffer);
			ret = copyFile("app0:files/basegame/bgdl/f0.pdb", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
		
			/// icon.png
			sprintf(buffer, "%s/icon.png", bgdl);
			printf("Writing %s.. ", buffer);
			ret = copyFile("app0:files/basegame/bgdl/icon.png", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
		
		} else print_color("ERROR\n\n", RED);	
		
		
		
		/// install in PSPemu
		printf("\nCreating folders in %s.. ", pspemu);
		
		sprintf(buffer, "%s/PCSC80018/USRDIR/CONTENT", pspemu);
		makePath(buffer);
		
		sprintf(buffer, "%s/PCSC80018/sce_sys/package", pspemu);
		ret = makePath(buffer);
			
		if ( ret == 0 )	{
			print_color("OK\n\n", GREEN);
			
			/// PCSC80018/sce_sys/package/body.bin
			sprintf(buffer, "%s/PCSC80018/sce_sys/package/body.bin", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/sce_sys/package/body.bin.. ");
			ret = copyFile("app0:files/basegame/pspemu/body.bin", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
			
			/// PCSC80018/sce_sys/package/head.bin
			sprintf(buffer, "%s/PCSC80018/sce_sys/package/head.bin", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/sce_sys/package/head.bin.. ");
			ret = copyFile("app0:files/basegame/pspemu/head.bin", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
			
			/// PCSC80018/sce_sys/package/stat.bin
			sprintf(buffer, "%s/PCSC80018/sce_sys/package/stat.bin", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/sce_sys/package/stat.bin.. ");
			ret = copyFile("app0:files/basegame/pspemu/stat.bin", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
			
			/// PCSC80018/sce_sys/package/tail.bin
			sprintf(buffer, "%s/PCSC80018/sce_sys/package/tail.bin", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/sce_sys/package/tail.bin.. ");
			ret = copyFile("app0:files/basegame/pspemu/tail.bin", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);	
			
			/// PCSC80018/sce_sys/package/work.bin
			sprintf(buffer, "%s/PCSC80018/sce_sys/package/work.bin", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/sce_sys/package/work.bin.. ");
			ret = copyFile("app0:files/basegame/pspemu/work.bin", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);
			
			/// PCSC80018/USRDIR/CONTENT/EBOOT.PBP
			sprintf(buffer, "%s/PCSC80018/USRDIR/CONTENT/EBOOT.PBP", pspemu);
			//printf("Writing %s.. ", buffer);
			printf("Writing /PCSC80018/USRDIR/CONTENT/EBOOT.PBP.. ");
			ret = copyFile("app0:files/basegame/pspemu/EBOOT.PBP", buffer);
			if ( ret == 0 )	print_color("OK\n", GREEN);	
			else print_color("ERROR\n", RED);

		} else print_color("ERROR\n\n", RED);	
		
		
		
		//unblock_psbutton();
	
		if (SCE_CTRL_ENTER == SCE_CTRL_CROSS) print_color("\n\nDone! Press X to reboot..\n", GREEN);
		else print_color("\n\nDone! Press O to reboot..\n", GREEN); 

		while (1) {
			readPad();
			if (pressed_buttons & SCE_CTRL_ENTER) scePowerRequestColdReset();
			if (pad.buttons & SCE_CTRL_SELECT && pad.buttons & SCE_CTRL_START) sceKernelExitProcess(0);
		} 
		
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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


void lock_psbutton() {
    sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
}

void unlock_psbutton() {
    sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
}


void sql_simple_exec(sqlite3 *db, const char *sql) {
	char *error = NULL;
	//int ret = 
	sqlite3_exec(db, sql, NULL, NULL, &error);
	if (error) {
		printf("Failed to execute %s: %s\n", sql, error);
		sqlite3_free(error);
		goto fail;
	}
	return;
fail:
	sqlite3_close(db);
}

int add_livearea_style(char *id) {
	sqlite3 *db;
	int ret = sqlite3_open("ur0:/shell/db/app.db", &db);
	if (ret) {
		printf("Failed to open the database: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	
	/* Thanks SMOKE! :) Workaround for sql error add_rm_list caused by triggers in app.db Removes problematic triggers -> modifies tables -> recreates triggers For some reason PSTV doesnt like this - Probably need a better way*/
	sql_simple_exec(db, "DROP TRIGGER tgr_livearea_del_img");
	sql_simple_exec(db, "DROP TRIGGER tgr_livearea_frame_del_item");
		
		sprintf(big_buffer, "DELETE FROM 'tbl_livearea' WHERE titleId='%s'", id);
		sql_simple_exec(db, big_buffer);
		sprintf(big_buffer, "INSERT INTO 'tbl_livearea' VALUES ('%s', 'ur0:appmeta/%s/livearea/contents', 'psmobile', '01.00', '1', 'bg0.png', 'startup.png', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0' , '2' )", id, id);
		sql_simple_exec(db, big_buffer);
		
		sprintf(big_buffer, "DELETE FROM 'tbl_livearea_frame' WHERE titleId='%s'", id);
		sql_simple_exec(db, big_buffer);
		sprintf(big_buffer, "INSERT INTO 'tbl_livearea_frame' VALUES ('%s', 'frame2', '1', '0', '0', '0', '<liveitem><text valign=\"bottom\" align=\"left\" text-align=\"left\" text-valign=\"bottom\" line-space=\"3\" ellipsis=\"on\"><str color=\"#ffffff\" size=\"45\" bold=\"on\" shadow=\"on\">%s</str></text></liveitem>', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)", id, LIVEAREA_frame2_text);
		sql_simple_exec(db, big_buffer);
		sprintf(big_buffer, "INSERT INTO 'tbl_livearea_frame' VALUES ('%s', 'frame3', '1', '0', '0', '0', '<liveitem><text valign=\"top\" align=\"left\" text-align=\"left\" text-valign=\"top\" line-space=\"2\" ellipsis=\"on\"><str size=\"22\" shadow=\"on\">%s</str></text></liveitem>', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)", id, LIVEAREA_frame3_text);
		sql_simple_exec(db, big_buffer);
		sprintf(big_buffer, "INSERT INTO 'tbl_livearea_frame' VALUES ('%s', 'frame4', '1', '0', '0', '0', '<liveitem><text align=\"right\" text-align=\"right\" word-wrap=\"off\" ellipsis=\"on\"><str size=\"18\" color=\"#ffffff\" shadow=\"on\">%s</str></text></liveitem>', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)", id, LIVEAREA_frame4_text);
		sql_simple_exec(db, big_buffer);
		
	sql_simple_exec(db, "CREATE TRIGGER tgr_livearea_del_img AFTER DELETE ON tbl_livearea WHEN OLD.gate_startupImage LIKE ' %' OR OLD.background_image LIKE ' %' BEGIN UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.gate_startupImage AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.background_image AS INTEGER);END");
	sql_simple_exec(db, "CREATE TRIGGER tgr_livearea_frame_del_item BEFORE DELETE ON tbl_livearea_frame WHEN OLD.item0_bg_path LIKE ' %' OR OLD.item0_img_path LIKE ' %' OR OLD.item1_bg_path LIKE ' %' OR OLD.item1_img_path LIKE ' %' OR OLD.item2_bg_path LIKE ' %' OR OLD.item2_img_path LIKE ' %' OR OLD.item3_bg_path LIKE ' %' OR OLD.item3_img_path LIKE ' %' OR OLD.item4_bg_path LIKE ' %' OR OLD.item4_img_path LIKE ' %' OR OLD.item5_bg_path LIKE ' %' OR OLD.item5_img_path LIKE ' %' OR OLD.item6_bg_path LIKE ' %' OR OLD.item6_img_path LIKE ' %' OR OLD.item7_bg_path LIKE ' %' OR OLD.item7_img_path LIKE ' %' OR OLD.item8_bg_path LIKE ' %' OR OLD.item8_img_path LIKE ' %' OR OLD.item9_bg_path LIKE ' %' OR OLD.item9_img_path LIKE ' %' BEGIN UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item0_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item0_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item1_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item1_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item2_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item2_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item3_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item3_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item4_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item4_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item5_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item5_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item6_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item6_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item7_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item7_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item8_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item8_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item9_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item9_img_path AS INTEGER);END");

	sqlite3_close(db);
	db = NULL;
	
	//my indicator folder -> TODO make a better solution (yes this was kinda rushed)
	sprintf(buffer, "ur0:appmeta/%s/temp", id);
	makePath(buffer); 
	
	return 1;
}

int remove_livearea_style(char *id) {
	sqlite3 *db;
	int ret = sqlite3_open("ur0:/shell/db/app.db", &db);
	if (ret) {
		printf("Failed to open the database: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	/* Thanks SMOKE! :) Workaround for sql error add_rm_list caused by triggers in app.db Removes problematic triggers -> modifies tables -> recreates triggers For some reason PSTV doesnt like this - Probably need a better way*/
	sql_simple_exec(db, "DROP TRIGGER tgr_livearea_del_img");
	sql_simple_exec(db, "DROP TRIGGER tgr_livearea_frame_del_item");

	
		sprintf(big_buffer, "DELETE FROM 'tbl_livearea' WHERE titleId='%s'", id);
		sql_simple_exec(db, big_buffer);
		sprintf(big_buffer, "INSERT INTO 'tbl_livearea' VALUES ('%s', 'ur0:appmeta/%s/livearea/contents', 'pspemu', '01.00', '1', 'bg0.png', 'startup.png', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0' , '2' )", id, id);
		sql_simple_exec(db, big_buffer);
		
		sprintf(big_buffer, "DELETE FROM 'tbl_livearea_frame' WHERE titleId='%s'", id);
		sql_simple_exec(db, big_buffer);
		
	sql_simple_exec(db, "CREATE TRIGGER tgr_livearea_del_img AFTER DELETE ON tbl_livearea WHEN OLD.gate_startupImage LIKE ' %' OR OLD.background_image LIKE ' %' BEGIN UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.gate_startupImage AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.background_image AS INTEGER);END");
	sql_simple_exec(db, "CREATE TRIGGER tgr_livearea_frame_del_item BEFORE DELETE ON tbl_livearea_frame WHEN OLD.item0_bg_path LIKE ' %' OR OLD.item0_img_path LIKE ' %' OR OLD.item1_bg_path LIKE ' %' OR OLD.item1_img_path LIKE ' %' OR OLD.item2_bg_path LIKE ' %' OR OLD.item2_img_path LIKE ' %' OR OLD.item3_bg_path LIKE ' %' OR OLD.item3_img_path LIKE ' %' OR OLD.item4_bg_path LIKE ' %' OR OLD.item4_img_path LIKE ' %' OR OLD.item5_bg_path LIKE ' %' OR OLD.item5_img_path LIKE ' %' OR OLD.item6_bg_path LIKE ' %' OR OLD.item6_img_path LIKE ' %' OR OLD.item7_bg_path LIKE ' %' OR OLD.item7_img_path LIKE ' %' OR OLD.item8_bg_path LIKE ' %' OR OLD.item8_img_path LIKE ' %' OR OLD.item9_bg_path LIKE ' %' OR OLD.item9_img_path LIKE ' %' BEGIN UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item0_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item0_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item1_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item1_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item2_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item2_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item3_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item3_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item4_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item4_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item5_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item5_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item6_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item6_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item7_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item7_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item8_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item8_img_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item9_bg_path AS INTEGER);UPDATE tbl_livearea_file SET refcnt=refcnt-1 WHERE rowid=CAST(OLD.item9_img_path AS INTEGER);END");

	sqlite3_close(db);
	db = NULL;
	
	//remove folder indicator
	sprintf(buffer, "ur0:appmeta/%s/temp", id);
	removePath(buffer); 
		
	return 1;
}




int main() {
	
	initSceAppUtil();
		readInEnterButton();
		//getSystemLanguageCode();
		//getUsername();
	finishSceAppUtil();
		
	netInit();
	httpInit();
		
	sceShellUtilInitEvents(0); //for blocking buttons
	
		if ( system_check() ) {
			initVita2DLib(); //initiate Vita2dLib
			recovery_draw_main();
		}	
	
	sceKernelExitProcess(0);
	return 0;
}