#include "magnetic.h"
#define BUFFMAX 100000
char buffer[BUFFER_SIZE];
myBuf_t redbuffer[BUFFER_SIZE];
extern uint32_t res_addr;

typedef struct DATAOUT
{	
char DataBuffer[BUFFMAX];
int movepoint;
int savepoint;	
}DataOut;

DataOut data = {"\0", 0, 0};
File_Catalog FileCatalog = {"A0.txt", 0, 0};

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer(char *buf)
{
	for (int i=0; i<strlen(buf); i++) buf[i] = '\0';
}

void send_uart (char *string)
{
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart1, (uint8_t *) string, len, HAL_MAX_DELAY);  // transmit in blocking mode
}

void Card_Capacity()
{
	/*************** Card capacity details ********************/
if(f_mount(&fs, "/", 1) == FR_OK)
{
   	/* Check free space */
  	fresult = f_getfree("", &fre_clust, &pfs);
	  if(fresult == FR_OK)
		{
		 capacity[0] = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  	 capacity[1] = (uint32_t)(fre_clust * pfs->csize * 0.5);
		}
    // Отмантировать диск
    f_mount(NULL, "/", 1);
}

  	
}

void Card_Write(char *FileName)
{
	if(f_mount(&fs, "/", 1) == FR_OK)
 {
		/* Create second file with read write access and open it */
		if(f_open(&fil, FileName, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK)
		{
			if (f_lseek(&fil, f_size(&fil)) == FR_OK)
			{
				fresult = f_write(&fil, buffer, 128, &bw);
				fresult = f_write(&fil, "\n", 1, &bw);
				f_close(&fil);
			}
			clear_buffer(buffer);
		}
		f_mount(NULL, "/", 1);
 }
  /* Writing text */ 
}

void Card_Write_Buff(char *FileName, char *Buffer)
{
  	/* Create second file with read write access and open it */
  	fresult = f_open(&fil, FileName, FA_CREATE_ALWAYS | FA_WRITE);
  	/* Writing text */
  	fresult = f_write(&fil, Buffer, bufsize(Buffer), &bw);
  	/* Close file */
  	f_close(&fil);
	  clear_buffer(buffer);
}
void Card_Read(char *FileName)
{
 if(f_mount(&fs, "/", 1) == FR_OK)
 {
	  clear_buffer(buffer);
  	/* Open second file to read */
  	fresult = f_open(&fil, FileName, FA_READ);
  	/* Read data from the file
  	 * Please see the function details for the arguments */
  	f_read (&fil, buffer, f_size(&fil), &br);
  	/* Close file */
  	f_close(&fil);

  	f_mount(NULL, "/", 1);
 }	
}


int File_Create()
{
	uint8_t FileNumber = FileCatalog.FileNumber;
	
//	File_Catalog *HEAD = NULL;
//	
//  HEAD = malloc(sizeof(File_Catalog));
//  if (HEAD == NULL) { return 1;}
//  HEAD->FileNames   = "A0.txt";
//	HEAD->FileNumber = 1;
//	HEAD->FileSize   = 0;
//  HEAD->New_File   = NULL;
	
	sprintf(FileCatalog.FileNames, "A%2d.txt",&FileCatalog.FileNumber);
	FileCatalog.FileNumber++;
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}


void push(File_Catalog **head, int data) {
    File_Catalog *tmp = (File_Catalog*) malloc(sizeof(File_Catalog));
	  tmp->FileNames = FileCatalog.FileNames;
	  tmp->FileNumber= data;
    tmp->New_File = (*head);
    (*head) = tmp;
}

void Data_Package()
{
	sprintf (buffer, "$%2hhd,%2hhd,%f,%c,%f,%c,%f,%d,%d,%d",   GPS.GPGGA.UTC_Hour,     GPS.GPGGA.UTC_Min,
																								         	 	 GPS.GPGGA.Latitude,     GPS.GPGGA.NS_Indicator,
																								         		 GPS.GPGGA.Longitude,    GPS.GPGGA.EW_Indicator,
																								         		 GPS.GPGGA.MSL_Altitude, adc[0],
		                                                         adc[1],                 adc[2]);


//  //////////////////////// ИЩЕМ ПУСТЫЕ ЯЧЕЙКИ ОТ НАЧАЛА ПАМЯТИ ////////////////////////
//  res_addr = flash_search_adress(STARTADDR, BUFFSIZE * DATAWIDTH);
//  /////////////////////////////////////////////////////////////////////////////////////
//  myBuf_t wdata[60]; // массив из четырёх полезных значений и завершающего нуля
//	int j = 0;
//	for(int i = 0; i < 61; i++,j++)
//	{
//	  wdata[j] = (buffer[i]<<8 | buffer[i+1]);
//		redbuffer[j] = wdata[j];
//		i++;
//	}
//	write_to_flash(wdata);
  
	Card_Write(FileCatalog.FileNames);
}