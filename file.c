#include "file.h"
#include "graphics.h"

char *getEbootTitle(char *src_path) {
		
	unsigned char hex_buffer[16] = "";
	char hexstring[128];
	static char return_str[128];
	
	int paramsfo_start = 0x28; 	//should always be 0x28
	int category_block_start;
	int variables_block_start;
	int number_of_parameters;
	int i, j, a, b, e; // c, d,
	
	/// open the eboot file
	FILE *fp = NULL;
    if ( ( fp = fopen ( src_path, "rb")) == NULL) {
        return "Error opening EBOOT!?";
    }
	
	/// check if this is where the param.sfo starts (if valid)
	fseek(fp, paramsfo_start, SEEK_SET);
	fread( &hex_buffer, 8, 1, fp);
	if ( hex_buffer[0] == 0x00 && hex_buffer[1] == 0x50 && hex_buffer[2] == 0x53 && hex_buffer[3] == 0x46 ) { //because every PARAM.SFO starts with 00 50 53 46 01 01 00 00

		sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[3], hex_buffer[2], hex_buffer[1], hex_buffer[0]);
		category_block_start = (int)strtol(hexstring, NULL, 0); //0xF4 aka 244
		memset(hexstring,0,sizeof(hexstring)); //clear the string
		
		/// read in category_block_start / the offset where the "Category Block starts"
		fread( &hex_buffer, 4, 1, fp);	
		sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[3], hex_buffer[2], hex_buffer[1], hex_buffer[0]);
		category_block_start = (int)strtol(hexstring, NULL, 0); //0xF4 aka 244
		memset(hexstring,0,sizeof(hexstring)); //clear the string
		
		/// read in variables_block_start / the offset where the "Variables Block starts"
		fread( &hex_buffer, 4, 1, fp);	
		sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[3], hex_buffer[2], hex_buffer[1], hex_buffer[0]);
		variables_block_start = (int)strtol(hexstring, NULL, 0); //0x84 0x01 = 0x184 aka 388
		memset(hexstring,0,sizeof(hexstring)); //clear the string
		
		/// read in number of Parameters there are in the sfo
		fread( &hex_buffer, 4, 1, fp);
		sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[3], hex_buffer[2], hex_buffer[1], hex_buffer[0]);
		number_of_parameters = (int)strtol(hexstring, NULL, 0); //0x0E aka 14
		memset(hexstring,0,sizeof(hexstring)); //clear the string
		
		/// iterate through offset info for all parameters
		for ( i = 0; i < number_of_parameters; i++ ) {
			fseek(fp, paramsfo_start + 0x14 + 0x10*i, SEEK_SET); //0x14 start of block + blocksize * i (which one)
			fread( &hex_buffer, 16, 1, fp);
				//hex_buffer[0-1] -> from "category_block_start" to start of parameter title
				//hex_buffer[2-3] -> type of parameter (04 02 = UTF8 & 04 04 = int32)
				//hex_buffer[4-7] -> actually used in variable block (length kinda)
				//hex_buffer[8-11] -> length of parameter
				//hex_buffer[12-15] -> from "variables_block_start" to start of data
				
				//eg. 00 00 - 04 02 - 06 00 00 00 - 08 00 00 00 - 00 00 00 00 
				//    --a--   --b--   -----c-----   -----d-----   -----e-----
			
			sprintf(hexstring, "0x%02x%02x", hex_buffer[1], hex_buffer[0] );
			a = (int)strtol(hexstring, NULL, 0);
			memset(hexstring,0,sizeof(hexstring)); //clear the string
			
			sprintf(hexstring, "0x%02x%02x", hex_buffer[3], hex_buffer[2] );
			b = (int)strtol(hexstring, NULL, 0);
			memset(hexstring,0,sizeof(hexstring)); //clear the string
			
			if ( b == 0x204 ) { //because TITLE is 04 02 = UTF8 
				
				/// read in category block name
				fseek(fp, paramsfo_start + category_block_start + a, SEEK_SET);
				j = 1;
				while ( j ) {
					fread( &hex_buffer, 1, 1, fp);
					if ( hex_buffer[0] == 0 ) j = 0;
					sprintf(hexstring, "%s%c", hexstring, hex_buffer[0]);
				}
				
				if ( strcmp(hexstring, "TITLE") == 0 ) {
					memset(hexstring,0,sizeof(hexstring)); //clear the string
					
					/*sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[7], hex_buffer[6], hex_buffer[5], hex_buffer[4] );
					c = (int)strtol(hexstring, NULL, 0);
					memset(hexstring,0,sizeof(hexstring)); //clear the string
					
					sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[11], hex_buffer[10], hex_buffer[9], hex_buffer[8] );
					d = (int)strtol(hexstring, NULL, 0);
					memset(hexstring,0,sizeof(hexstring)); //clear the string*/
					
					sprintf(hexstring, "0x%02x%02x%02x%02x", hex_buffer[15], hex_buffer[14], hex_buffer[13], hex_buffer[12] );
					e = (int)strtol(hexstring, NULL, 0);
					memset(hexstring,0,sizeof(hexstring)); //clear the string
					
					
					/// read in the Eboots title! yay :)
					fseek(fp, paramsfo_start + variables_block_start + e, SEEK_SET);
					j = 1;
					while ( j ) {
						fread( &hex_buffer, 1, 1, fp);
						if ( hex_buffer[0] == 0 ) j = 0;
						sprintf(hexstring, "%s%c", hexstring, hex_buffer[0]);
					}
					strcpy(return_str, hexstring); //termination fix
					fclose(fp);
					return return_str;
				}
			}
		}
	} else {
		fclose(fp);
		return "Not a valid EBOOT?!";
	}
	
	fclose(fp);
	return "Could not find TITLE in EBOOT";
}


int doesFileExist(const char* path) { 
	SceUID dir = sceIoOpen(path, SCE_O_RDONLY, 0777); 
 	if (dir >= 0) { 
 		sceIoClose(dir); 
 		return 1; 
 	} else { 
 		return 0; 
 	} 
} 

int doesDirExist(const char* path) { 
	SceUID dir = sceIoDopen(path); 
 	if (dir >= 0) { 
 		sceIoDclose(dir); 
 		return 1; 
 	} else { 
 		return 0; 
 	} 
} 

int copyFile(char *src_path, char *dst_path) { //thx Flow VitaShell
	// The source and destination paths are identical
	if (strcmp(src_path, dst_path) == 0) return -1;

	// The destination is a subfolder of the source folder
	int len = strlen(src_path);
	if (strncmp(src_path, dst_path, len) == 0 && dst_path[len] == '/') return -1;

	SceUID fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
	if (fdsrc < 0) return fdsrc;

	SceUID fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fddst < 0) {
		sceIoClose(fdsrc);
		return fddst;
	}

	void *buf = malloc(TRANSFER_SIZE);

	int read;
	while ((read = sceIoRead(fdsrc, buf, TRANSFER_SIZE)) > 0) {
		sceIoWrite(fddst, buf, read);
	}

	free(buf);

	sceIoClose(fddst);
	sceIoClose(fdsrc);

	return 0;
}

int removePath(char *path) {  //thx Flow VitaShell
	SceUID dfd = sceIoDopen(path);
	if (dfd >= 0) {
		int res = 0;

		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
					continue;

				char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
				snprintf(new_path, MAX_PATH_LENGTH, "%s/%s", path, dir.d_name);

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					int ret = removePath(new_path);
					if (ret <= 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				} else {
					int ret = sceIoRemove(new_path);
					if (ret < 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				}

				free(new_path);
			}
		} while (res > 0);

		sceIoDclose(dfd);

		int ret = sceIoRmdir(path);
		if (ret < 0)
			return ret;

	} else {
		int ret = sceIoRemove(path);
		if (ret < 0)
			return ret;
	}

	return 1;
}


int download_file(const char *src, const char *dst) { //thx molecule offline installer
	int ret;
	int tpl = sceHttpCreateTemplate("henkaku offline", 2, 1);
	if (tpl < 0) {
		//printf("sceHttpCreateTemplate: 0x%x\n", tpl);
		return tpl;
	}
	int conn = sceHttpCreateConnectionWithURL(tpl, src, 0);
	if (conn < 0) {
		//printf("sceHttpCreateConnectionWithURL: 0x%x\n", conn);
		return conn;
	}
	int req = sceHttpCreateRequestWithURL(conn, 0, src, 0);
	if (req < 0) {
		//printf("sceHttpCreateRequestWithURL: 0x%x\n", req);
		return req;
	}
	ret = sceHttpSendRequest(req, NULL, 0);
	if (ret < 0) {
		//printf("sceHttpSendRequest: 0x%x\n", ret);
		return ret;
	}
	unsigned char buf[4096] = {0};

	long long unsigned length = 0;
	ret = sceHttpGetResponseContentLength(req, &length);

	int fd = sceIoOpen(dst, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 6);
	int total_read = 0;
	if (fd < 0) {
		//printf("sceIoOpen: 0x%x\n", fd);
		return fd;
	}
	// draw progress bar background
	//fg_color = 0xFF666666;
	//draw_rect(psvDebugScreenGetX(), psvDebugScreenGetY(), PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT);
	//fg_color = 0xFFFFFFFF;
	while (1) {
		int read = sceHttpReadData(req, buf, sizeof(buf));
		
		if (read < 0) {
			//printf("sceHttpReadData error! 0x%x\n", read);
			return read;
		}
		
		if (read == 0) break; //done downloading
			
		ret = sceIoWrite(fd, buf, read);
		if (ret < 0 || ret != read) {
			//printf("sceIoWrite error! 0x%x\n", ret);
			if (ret < 0)
				return ret;
			return -1;
		}
		total_read += read;
		//draw_rect(psvDebugScreenGetX() + 1, psvDebugScreenGetY() + 1, (PROGRESS_BAR_WIDTH - 2) * total_read / length, PROGRESS_BAR_HEIGHT - 2);
	}
	printf("\n\n");
	ret = sceIoClose(fd);
	//if (ret < 0)
		//printf("sceIoClose: 0x%x\n", ret);

	return 0;
}


const char *get_id_of_psp_game_that_adrenaline_is_installed_to() { //return PSP GAME ID of which adrenaline is installed to (and save it globally)
	
	char buffer[255];
	static char GAME_ID[32];
	
	FILE* config = fopen("ux0:tai/config.txt", "r");
	
	if (config == NULL){
		if (config != NULL) fclose(config);
		
	} else {
		while (fscanf(config, "%s", buffer) == 1) { 
		
			//read in the Title_ID starting with a *
			if( strstr(&buffer[0], "*") ) memmove(GAME_ID, buffer+1, sizeof(buffer));
			
			if( strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.suprx") ) {
				sprintf(PSP_GAME_ID, "%s", GAME_ID); //save global
				return GAME_ID;
			}	

		}	
		fclose(config);
	}	
	
	return "Not found"; 
}


int write_adrenaline_to_config(char *id) {
	
	char buffer[1024];
	FILE *file = fopen("ux0:tai/config.txt", "r");
	FILE *temp = fopen("ux0:tai/temp.txt", "w");
	
	
	if (file == NULL){
		//Error: Couldn't open for writing
		return 1;
	} else {
		
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			fprintf(temp, "%s", buffer);
			
			if( (strstr(buffer, "*KERNEL")) != NULL ) {
				fprintf(temp, "ux0:pspemu/adrenaline/adrenaline.skprx\n");
			}
		}
		
		//end of file (append the rest)
		fprintf(temp, "*%s\n", id);
		fprintf(temp, "vs0:sys/external/libpgf.suprx\n");
		fprintf(temp, "ux0:pspemu/adrenaline/adrenaline.suprx\n");
			
	}
	fclose(file);
	fclose(temp);
	
	sceIoRemove("ux0:tai/config.txt");
	copyFile("ux0:tai/temp.txt", "ux0:tai/config.txt");
	sceIoRemove("ux0:tai/temp.txt");
	
	return 0;
}

int delete_adrenaline_from_config(char *id) {

	char buffer[1024];
	FILE *file = fopen("ux0:tai/config.txt", "r");
	FILE *temp = fopen("ux0:tai/temp.txt", "w");
	
	
	if (file == NULL){
		//Error: Couldn't open for writing
		return 1;
	} else {
		
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			
			if( (strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.skprx")) != NULL ) continue;	
			if( (strstr(buffer, id)) != NULL ) continue;
			if( (strstr(buffer, "vs0:sys/external/libpgf.suprx")) != NULL )	continue;		
			if( (strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.suprx")) != NULL ) continue;	
			
			fprintf(temp, "%s", buffer);
		}			
	}
	fclose(file);
	fclose(temp);
	
	sceIoRemove("ux0:tai/config.txt");
	copyFile("ux0:tai/temp.txt", "ux0:tai/config.txt");
	sceIoRemove("ux0:tai/temp.txt");
	
	return 0;
}

int check_for_psp_content(char *path) { //returns number of valid PSP games in *path* and saves title id and name in global arrays
	SceUID dfd = sceIoDopen(path);
	char pathbuff[256];
	
	if (dfd >= 0) {
		int res = 0;
		int count = 0;
		
		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));
			
			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
					continue;
					
				sprintf(pathbuff, "%s%s%s", path, dir.d_name, "/__sce_ebootpbp");
				if ( doesFileExist(pathbuff) ) {
				
					//save TitleID in global array
					strcpy(content_array[count].titleID, dir.d_name);
					
					//save Eboot Name in global array
					sprintf(pathbuff, "%s%s/EBOOT.PBP", path, dir.d_name );				
					strcpy(content_array[count].title, getEbootTitle(pathbuff));

					count++;
				}	
				
			}
		} while (res > 0);
		
		sceIoDclose(dfd);
		return count;
	} 

	return 0;
}


void trigger_update_database() { //deleting or creating a null-byte makes no difference it seems

	/// 1) delete id.dat -> System is confused which card it should use? (internal/external on SLim/PSTV -> Change location Dialog will appear)
		//sceIoRemove("ux0:id.dat");
	
	
	/// 2) delete id.dat and write empty id.dat -> same result as 1
		/*sceIoRemove("ux0:id.dat");
		SceUID id = sceIoOpen("ux0:id.dat", SCE_O_WRONLY|SCE_O_CREAT, 0777);
		sceIoWrite(id, NULL, 0x0);
		sceIoClose(id);*/
		
		
	/// 3) delete one row in id.dat (or write INVALID_XXX=)
		/// - delete MID row -> triggers update and regenerates and writes MID in the last row
		/// - delete SVR row -> no update dialog but will regenerate the line
		/// Writing a replacement "MID=" without data works too and keeps right order of id.dat
		
		static char datbuff[512];
		static char mid_string[] = "MID=";	
	
		FILE *dat = fopen("ux0:id.dat", "r");
		FILE *tempdat = fopen("ux0:temp.dat", "w");
	
		if (dat == NULL){
			//Error: Couldn't open for writing
		} else {
		
			while (fgets(datbuff, 512, dat) != NULL) {
				if( strstr(datbuff, mid_string) ) fprintf(tempdat, "%s\n", mid_string);
				else fprintf(tempdat, "%s", datbuff);
			}			
		}
		fclose(dat);
		fclose(tempdat);
		
		sceIoRemove("ux0:id.dat");
		copyFile("ux0:temp.dat", "ux0:id.dat"); 
		sceIoRemove("ux0:temp.dat");
		
		
	/// 4) delete all rows and write empty "MID=" "DIG=" ..... - triggers change location for savedata too
		/*sceIoRemove("ux0:id.dat");
		SceUID id = sceIoOpen("ux0:id.dat", SCE_O_WRONLY|SCE_O_CREAT, 0777);
		sceIoWrite(id, "MID=\nDIG=\nDID=\nAID=\nOID=\nSVR=", 31);
		sceIoClose(id);*/	
		
		
	/// 5) replace id.dat with folder called id.dat -> EVIL
}

void trigger_rebuild_database() { 

	/**
	* Simply removing app.db still shows up as "updating database" but rebuilds app.db obviously with icon layout and everything.
	* The CAFEBABE method does not affect the icon layout and does not really rebuild everything.. (eg appmeta resources still there) 
	* BUT shows up as "rebuild"..
	*
	* -> Soooo we simply combine them :)
	*/

	char magik[] = { 0xBE, 0xBA, 0xFE, 0xCA };
		
	SceUID id;
	id = sceIoOpen("ur0:shell/db/dbr.db-err", SCE_O_WRONLY|SCE_O_CREAT, 0777);

	sceIoWrite(id, magik, 4);
	sceIoClose(id);
		
	sceIoRemove("ur0:shell/db/app.db");
}
