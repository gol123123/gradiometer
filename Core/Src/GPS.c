#include "GPS.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GPS_t GPS;

char Rxbuff_UART1[BUFFSIZE] = {0,};
char Txbuff_UART1[BUFFSIZE] = {0,};

uint8_t	    UTC_Hour_Fix = 0x03;


///////////////////////////////////
//Fix Mode             /Talker ID//
///////////////////////////////////
//BDS single mode      / BD      //
//Galileo single mode  / GA      //
//GPS single mode      / GP      //
//GLONASS single mode  / GL      //
//GNSS multi-mode      / GN      //
///////////////////////////////////

///////////////////////////////////////////////////
//CAS-Set the Baud Rate of the UART              //
//Sentence Format $CCCAS,x        ,y*hh          //
//                       1: UART0  0: 4800   bps //
//                       2: UART1  1: 9600   bps //
//                                 2: 19200  bps //
//                                 3: 38400  bps //
//                                 4: 57600  bps //
//                                 5: 115200 bps //
//                                 6: 230400 bps //
///////////////////////////////////////////////////
	
char *BaudRate0[7] = { "$CCCAS,1,0*50",   // UART0_4800bps 
	                     "$CCCAS,1,1*51",   // UART0_9600bps
	                     "$CCCAS,1,2*52",   // UART0_19200bps 
	                     "$CCCAS,1,3*53",   // UART0_38400bps 
	                     "$CCCAS,1,4*54",   // UART0_57600bps
	                     "$CCCAS,1,5*55",   // UART0_115200bps
	                     "$CCCAS,1,6*56"    // UART0_230400bps 
	};
	
char *BaudRate1[7] = { "$CCCAS,2,0*50",   // UART1_4800bps 
	                     "$CCCAS,2,1*51",   // UART1_9600bps
	                     "$CCCAS,2,2*52",   // UART1_19200bps 
	                     "$CCCAS,2,3*53",   // UART1_38400bps 
	                     "$CCCAS,2,4*54",   // UART1_57600bps
	                     "$CCCAS,2,5*55",   // UART1_115200bps
	                     "$CCCAS,2,6*56"    // UART1_230400bps 
	};

///////////////////////////////////////////////////
//Restore to the Default Settings                //
//Sentence Format $CCDFT ,a           ,*hh       //
//                       0: restore              //
//                       all settings            //
///////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//Sentence Format   / $CCSIR,x,y*hh                                    /
////////////////////////////////////////////////////////////////////////	
//Sentence/ Set work mode and start control                            /
////////////////////////////////////////////////////////////////////////
//Field   / Symbol  / Field Description                                /
////////////////////////////////////////////////////////////////////////
// 1      / $CCSIR  /                                                  /
////////////////////////////////////////////////////////////////////////
// 2      / x       / x indicates working mode. The value can be       /
//        /         / 1: BDS                                           /
//        /         / 2: GPS                                           /
//        /         / 3: BDS+GPS                                       /
//        /         / 4: GLONASS                                       /
//        /         / 5: BDS +GLONASS                                  /
//        /         / 6: GPS +GLONASS                                  /
////////////////////////////////////////////////////////////////////////
// 3      / y       / 0: set to 0 if users cannot determine start mode /
//        /         /    (cold, hot, or warm).                         /
//        /         / The software will determine start mode           /
//        /         / automatically based on working mode. Start mode: /                
//        /         / 1: cold start                                    /
//        /         / 2: warm start                                    /
//        /         / 3: hot start                                     /
////////////////////////////////////////////////////////////////////////
// 4      / hh      / Checksum                                         /
////////////////////////////////////////////////////////////////////////
char *WorkingMode[6] = {"$CCSIR,6,1*4F",   // GPS+GLO cold start
	                      "$CCSIR,6,2*4C",   // GPS+GLO warm start
	                      "$CCSIR,6,3*4D",   // GPS+GLO hot start 
	                      "$CCSIR,2,1*4B",   // GPS cold start  
	                      "$CCSIR,2,2*48",   // GPS warm start
	                      "$CCSIR,2,3*49",   // GPS hot start
	};

////////////////////////////////////////////////////////////////////////	
//Sentence/ Function Sentence output control                           /
////////////////////////////////////////////////////////////////////////
//Field   / Symbol  / Field Description                                /
////////////////////////////////////////////////////////////////////////
// 1      / $CCMSG  /                                                  /
////////////////////////////////////////////////////////////////////////
// 2      / xxx     / Indicates output sentence type. The value can be:/
//        /         / RMC, GGA, GSA, GSV, GLL, VTG, ZDA, DTM, GNS, GBS,/
//        /         / GRS, GST, TXT                                    /
////////////////////////////////////////////////////////////////////////
// 3      / y       / Indicates output port. The value can be:         /
//        /         / 1: UART0                                         /
//        /         / 2: UART1                                         /
//        /         / 3: SPI                                           /
//        /         / 4: I2C                                           /
////////////////////////////////////////////////////////////////////////
// 4      / z       / Fix result output frequency. The value can be:   /
//                  / 0: Close output                                  /
////////////////////////////////////////////////////////////////////////
char *SentenceOpen[13] =  {"$CCMSG,RMC,1,1,*05",   //Open RMC 
	                         "$CCMSG,GGA,1,1,*18",   //Open GGA 
	                         "$CCMSG,GSA,1,1,*0C",   //Open GSA 
	                         "$CCMSG,GSV,1,1,*1B",   //Open GSV 
	                         "$CCMSG,GLL,1,1,*1E",   //Open GLL 
                           "$CCMSG,VTG,1,1,*1C",   //Open VTG 
                           "$CCMSG,ZDA,1,1,*06",   //Open ZDA 
                           "$CCMSG,DTM,1,1,*04",   //Open DTM 
                           "$CCMSG,GNS,1,1,*03",   //Open GNS 
                           "$CCMSG,GBS,1,1,*0F",   //Open GBS 
                           "$CCMSG,GRS,1,1,*1F",   //Open GRS 
                           "$CCMSG,GST,1,1,*19",   //Open GST 
                           "$CCMSG,TXT,1,1,*01"};  //Open TXT 
                
char *SentenceClose[13] = {"$CCMSG,RMC,1,0,*05",   //Close RMC 
	                         "$CCMSG,GGA,1,0,*18",   //Close GGA 
	                         "$CCMSG,GSA,1,0,*0C",   //Close GSA 
	                         "$CCMSG,GSV,1,0,*1B",   //Close GSV 
	                         "$CCMSG,GLL,1,0,*1E",   //Close GLL 
                           "$CCMSG,VTG,1,0,*1C",   //Close VTG 
                           "$CCMSG,ZDA,1,0,*06",   //Close ZDA 
                           "$CCMSG,DTM,1,0,*04",   //Close DTM 
                           "$CCMSG,GNS,1,0,*03",   //Close GNS 
                           "$CCMSG,GBS,1,0,*0F",   //Close GBS 
                           "$CCMSG,GRS,1,0,*1F",   //Close GRS 
                           "$CCMSG,GST,1,0,*19",   //Close GST 
                           "$CCMSG,TXT,1,0,*01"};  //Close TXT 

double convertDegMinToDecDeg (float degMin)
{
  double min = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );
 
  return decDeg;
}

void GetData()
{
	GPS.rxIndex=0;
  HAL_UART_Receive_DMA(&GPS_UART, &GPS.rxTmp, 1);
}



void SetCommant()
{
  HAL_UART_Transmit_IT(&GPS_UART, (uint8_t*)GPS.rxBuffer, sizeof(GPS.rxBuffer));
}

void TxDataTerminal()
{
  HAL_UART_Transmit_IT(&huart1, (uint8_t*)GPS.rxBuffer, sizeof(GPS.rxBuffer));
}



void GPS_CallBack(void)
{
	GPS.LastTime=HAL_GetTick();
	if(GPS.rxIndex < sizeof(GPS.rxBuffer)-2)
	{
		GPS.rxBuffer[GPS.rxIndex] = GPS.rxTmp;
		GPS.rxIndex++;
	}
	HAL_UART_Receive_DMA(&GPS_UART,&GPS.rxTmp,1);
}


void GPS_Process(void)
{
	if( (HAL_GetTick()-GPS.LastTime>50) && (GPS.rxIndex>0))
	{
		char	*str;
		#if (_GPS_DEBUG==1)
//		printf("%s",GPS.rxBuffer);
		#endif
		str=strstr((char*)GPS.rxBuffer,"$GPGGA,");
		if(str!=NULL)
		{
			memset(&GPS.GPGGA,0,sizeof(GPS.GPGGA));
			sscanf(str,"$GPGGA,%2hhd%2hhd%2hhd.%3hd,%f,%c,%f,%c,%hhd,%hhd,%f,%f,%c,%hd,%s,*%2s\r\n",&GPS.GPGGA.UTC_Hour,&GPS.GPGGA.UTC_Min,&GPS.GPGGA.UTC_Sec,&GPS.GPGGA.UTC_MicroSec,&GPS.GPGGA.Latitude,&GPS.GPGGA.NS_Indicator,&GPS.GPGGA.Longitude,&GPS.GPGGA.EW_Indicator,&GPS.GPGGA.PositionFixIndicator,&GPS.GPGGA.SatellitesUsed,&GPS.GPGGA.HDOP,&GPS.GPGGA.MSL_Altitude,&GPS.GPGGA.MSL_Units,&GPS.GPGGA.AgeofDiffCorr,GPS.GPGGA.DiffRefStationID,GPS.GPGGA.CheckSum);
			if(GPS.GPGGA.NS_Indicator==0)
				GPS.GPGGA.NS_Indicator='-';
			if(GPS.GPGGA.EW_Indicator==0)
				GPS.GPGGA.EW_Indicator='-';
			if(GPS.GPGGA.Geoid_Units==0)
				GPS.GPGGA.Geoid_Units='-';
			if(GPS.GPGGA.MSL_Units==0)
				GPS.GPGGA.MSL_Units='-';
			GPS.GPGGA.LatitudeDecimal=convertDegMinToDecDeg(GPS.GPGGA.Latitude);
			GPS.GPGGA.LongitudeDecimal=convertDegMinToDecDeg(GPS.GPGGA.Longitude);
			GPS.GPGGA.UTC_Hour = GPS.GPGGA.UTC_Hour + UTC_Hour_Fix;
		}		
		memset(GPS.rxBuffer,0,sizeof(GPS.rxBuffer));
		GPS.rxIndex=0;
	}
	HAL_UART_Receive_DMA(&GPS_UART,&GPS.rxTmp,1);
}

int GPS_SYNCING()
{
	uint8_t counter = 0;
	
	if(GPS.GPGGA.Latitude==0){
		counter++;
		//send_uart("latitude not received");
	}	
	if(GPS.GPGGA.Longitude==0){
		counter++;
		//send_uart("Longitude not received");
	}
	if(GPS.GPGGA.MSL_Altitude==0){
	  counter++;
		//send_uart("Altitude not received");
  }
	if(counter == 0){
				//send_uart("GPS is working successfully");
		    return 1;
	}else{
				//send_uart("GPS syncs");
		    return 0;
	}		
}

int Get_UTC_Hour_Fix()
{
	return UTC_Hour_Fix;

}
int Send_UTC_Hour_Fix(uint8_t send_UTC_Hour_Fix)
{
	if(send_UTC_Hour_Fix == 1)
	UTC_Hour_Fix++; 
	if(send_UTC_Hour_Fix == 2)
	UTC_Hour_Fix--; 
}