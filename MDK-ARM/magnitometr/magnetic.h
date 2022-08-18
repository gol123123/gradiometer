#include "GPS.h"
#include "fatfs.h"
#include "FlashPROM.h"
#include "string.h"
//#include "HMC5883L.h"

#ifndef __MAGNETIC_H
#define __MAGNETIC_H

#define BUFFER_SIZE 128
#define FILE_NAME_LEN 7

extern FATFS fs;  // file system
extern FIL fil; // File
extern FILINFO fno;
extern FRESULT fresult;  // result
extern UINT br, bw;  // File read/write count
extern BYTE work[_MAX_SS];
extern FATFS *pfs;
extern DWORD fre_clust;
extern uint32_t capacity[2];

extern volatile uint16_t adc[3];

typedef struct File_Catalog
{
	char*    FileNames;
	uint8_t	FileNumber;	
	uint8_t	FileSize;	
  struct  File_Catalog *New_File;
}File_Catalog;


int bufsize (char *buf);
void clear_buffer(char *buf);
void send_uart (char *string);
void push(File_Catalog **head, int data);

void Card_Capacity(void);
void Card_Write(char *FileName);
void Card_Write_Buff(char *FileName, char *Buffer);
void Card_Read(char *FileName);
void Data_Package(void);
int File_Create(void);

#ifdef __cplusplus
}
#endif

#endif