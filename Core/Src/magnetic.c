#include "magnetic.h"

char buffer[BUFFER_SIZE];

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

  	/* Check free space */
  	fresult = f_getfree("", &fre_clust, &pfs);
	  if(fresult == FR_OK)
		{
		 capacity[0] = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  	 capacity[1] = (uint32_t)(fre_clust * pfs->csize * 0.5);
		}
  	
}

void Card_Write(char *FileName)
{
  /* Create second file with read write access and open it */
	if(f_open(&fil, FileName, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK)
	{
		if (f_lseek(&fil, f_size(&fil)) == FR_OK)
		{
			fresult = f_write(&fil, buffer, bufsize(buffer), &bw);
			f_close(&fil);
		}
		clear_buffer(buffer);
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
	  clear_buffer(buffer);
  	/* Open second file to read */
  	fresult = f_open(&fil, FileName, FA_READ);
  	/* Read data from the file
  	 * Please see the function details for the arguments */
  	f_read (&fil, buffer, f_size(&fil), &br);
  	/* Close file */
  	f_close(&fil);

  	
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

void push(File_Catalog **head, int data) {
    File_Catalog *tmp = (File_Catalog*) malloc(sizeof(File_Catalog));
	  tmp->FileNames = FileCatalog.FileNames;
	  tmp->FileNumber= data;
    tmp->New_File = (*head);
    (*head) = tmp;
}

void Data_Package()
{
//	if(GPS_SYNCING())
//  {
//		return;
//	} // ждём пока GPS синхронизируется, и будет получать широту и долготу
	sprintf (buffer, "$%2hhd%2hhd%f%c%f%c%f%i%i%i",&GPS.GPGGA.UTC_Hour,     &GPS.GPGGA.UTC_Min,
																										&GPS.GPGGA.Latitude,     &GPS.GPGGA.NS_Indicator,
																										&GPS.GPGGA.Longitude,    &GPS.GPGGA.EW_Indicator,
																										&GPS.GPGGA.MSL_Altitude, &adc[0],
		                                                &adc[1],                &adc[2]);
	HAL_Delay(1);

	Card_Write(FileCatalog.FileNames);
	Card_Read(FileCatalog.FileNames);

}