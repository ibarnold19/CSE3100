#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

char* makeFileList(char* path) {
	DIR* theCWD = opendir(path);
	struct dirent buf;
	struct dirent* cur = NULL;
	readdir_r(theCWD,&buf,&cur);
	int ttlBytes = 0;
	while(cur != NULL) {
		ttlBytes += strlen(cur->d_name) + 1;
		readdir_r(theCWD,&buf,&cur);
	}
	char* txt = malloc((ttlBytes + 10)*sizeof(char));
	rewinddir(theCWD);
	readdir_r(theCWD,&buf,&cur);
	*txt = 0;
	while(cur != NULL) {
		strcat(txt,cur->d_name);
		strcat(txt,"\n");
		readdir_r(theCWD,&buf,&cur);
	}
	closedir(theCWD);
	return txt;
}

int getFileSize(char* fName) {
	FILE* f = fopen(fName,"r");
	fseek(f,0,SEEK_END);
	long sz = ftell(f);
	fclose(f);
	return (int)sz;
}

void sendFileOverSocket(char* fName, int chatSocket) {
	char buf[1024];
	int fileSize = getFileSize(fName);
	int nbread = 0,nbRem = fileSize;
	FILE* fd = fopen(fName,"r");
	while(nbRem != 0) {
		nbread = fread(buf,sizeof(char),sizeof(buf),fd);
		int nbsent = 0;
		while (nbsent != nbread)
			nbsent += send(chatSocket,buf+nbsent,nbread-nbsent,0);
		nbRem -= nbread;
	}
	fclose(fd);	
}

void receiveFileOverSocket(int sid, char* fname, char* ext, int fSize) {
	int rec  = 0,rem = fSize;
	char* buf = malloc(sizeof(char)*fSize);
	while (rem != 0) {
		int nbrecv = recv(sid,buf+rec,rem,0);
		rec += nbrecv;
		rem -= nbrecv;
	}
	char fn[512];
	strcpy(fn,fname);
	strcat(fn,ext);
	FILE* fd = fopen(fn,"w");
	fwrite(buf,sizeof(char),fSize,fd);
	fclose(fd);
	free(buf);	
}
