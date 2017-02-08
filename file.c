#include "file.h"
#include "graphics.h"

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


const char *get_id_of_psp_game_that_adrenaline_is_installed_to() { //return PSP GAME ID of which adrenaline is installed to
	
	char buffer[255];
	static char GAME_ID[32];
	
	FILE* f1 = fopen("ux0:tai/config.txt", "r");
	
	if (f1 == NULL){
		if (f1 != NULL) fclose(f1);
		
	} else {
		while (fscanf(f1, "%s", buffer) == 1) { 
		
			//read in the Title_ID starting with a *
			if( strstr(&buffer[0], "*") ) memmove(GAME_ID, buffer+1, sizeof(buffer));
			
			if( strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.suprx") ) {
				sprintf(PSP_GAME_ID, "%s", GAME_ID); //save global
				return GAME_ID;
			}	

		}	
		fclose(f1);
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
		
		while (fgets(buffer, 1024, file) != NULL) {
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
	
	
	///delete config.txt
	sceIoRemove("ux0:tai/config.txt");
	
	///rename (copy) temp.txt to config.txt
	copyFile("ux0:tai/temp.txt", "ux0:tai/config.txt");
	
	///delete temp.txt
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
		
		while (fgets(buffer, 1024, file) != NULL) {
			
			if( (strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.skprx")) != NULL )
				continue;
				
			if( (strstr(buffer, id)) != NULL )
				continue;
				
			if( (strstr(buffer, "vs0:sys/external/libpgf.suprx")) != NULL )
				continue;	
				
			if( (strstr(buffer, "ux0:pspemu/adrenaline/adrenaline.suprx")) != NULL )
				continue;	
			
			fprintf(temp, "%s", buffer);
		}			
	}
	fclose(file);
	fclose(temp);
	
	
	///delete config.txt
	sceIoRemove("ux0:tai/config.txt");
	
	///rename (copy) temp.txt to config.txt
	copyFile("ux0:tai/temp.txt", "ux0:tai/config.txt");
	
	///delete temp.txt
	sceIoRemove("ux0:tai/temp.txt");
	
	return 0;
}

int check_for_psp_content(char *path) { //returns number of valid PSP games in *path*
	SceUID dfd = sceIoDopen(path);
	static char pathbuff[256];
	
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
				
					strcpy(array_buffer[count], dir.d_name); //save in global array
					count++;
				}	
				
			}
		} while (res > 0);
		
		sceIoDclose(dfd);
		return count;
	} 

	return 0;
}

