#ifndef __COMMAND_H
#define __COMMAND_H


#define CC_LS	0
#define CC_GET	1
#define CC_PUT  2
#define CC_EXIT 3

#define PL_TXT  0
#define PL_FILE 1

typedef struct CommantTag {
	int  code; 
	char arg[256];
} Command;

typedef struct PayloadTag {
	int code;
	int length;	
} Payload;

char* makeFileList(char* path);
int getFileSize(char* fName);
void sendFileOverSocket(char* fName,int chatSocket);
void receiveFileOverSocket(int sid,char* fname,char* ext,int fSize);

#endif