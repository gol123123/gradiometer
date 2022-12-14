#include "displays.h"


volatile uint8_t flag_start  = 1;
volatile uint8_t flag_stop   = 1;
volatile uint8_t flag_adc    = 1;
volatile uint8_t flag_uart   = 0;
volatile uint8_t flag_toggle = 1;
volatile uint8_t Counter     = 1;
volatile uint8_t Enter       = 0;
volatile uint8_t Exit        = 1;
volatile uint8_t flag_exit   = 0;
volatile uint8_t flag_enter  = 0;
volatile uint8_t flag_draw   = 1;

extern volatile uint16_t adc[3];
extern uint32_t capacity[2];

void Menu_Blick(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const char* str1, const char* str2, FontDef font)
{
	/*if(flag_toggle){
	ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, 0x1414);
	ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
	}else
  { ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, ST7735_BLACK);}*/
	uint8_t Counter2=Counter; //запоминает значения
ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, 0x1414);
ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
if(Counter2<Counter) {
	ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
	return;
}
 //проверка увеличения указателя
	HAL_Delay(200);
	if(Counter2<Counter) {
		ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
		return;
	}
	HAL_Delay(200);
ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, ST7735_BLACK);
HAL_Delay(200);
	if(Counter2<Counter) {
		ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
		return;
	}
	HAL_Delay(200);
}

void MainMenu_Parent()
{
	MainMenu_Draw();
	if(Counter >= 5)      {	 Counter = 1; }
	else if(Counter <=0)  {  Counter = 4; }

	if(Counter == 1)
	{
		MainMenu_Start();
	}
	else if(Counter == 2)
	{
		MainMenu_Setting();
	}
	else if(Counter == 3)
	{
		MainMenu_Help();
	}
	else if(Counter == 4)
	{
		MainMenu_Exit();
	}
}

void MainMenu_Draw()
{
    if(flag_draw == 1)
		{
			
			flag_draw = 0;
			Counter   = 1;
			Exit      = 1;
			ST7735_FillScreen(ST7735_BLACK);

			ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
			ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

			ST7735_DrawString(10, 42,  "1",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(30, 42,  "start",    Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 62,  "2",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(30, 62,  "settings", Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 82,  "3",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(30, 82,  "help",     Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 102, "4",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(30, 102, "exit",     Font_11x18, ST7735_BLUE, ST7735_BLACK);
		}
}

void MainMenu_Start()
{
	Menu_Blick(10, 42, 30, 42, "1" , "start" ,Font_11x18);  
	if(Enter == 1)
	{
		MainMenu_File();
		
		Counter = 1;
		Enter   = 0;
		Exit    = 1;
		flag_draw = 1;
	
		while(Exit != 0)
		{
			if(flag_draw == 1)
			{
				flag_draw = 0;
				
				ST7735_FillScreen(ST7735_BLACK);

				ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
				ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

				ST7735_DrawString(10, 42,  "data is being",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 62,  "withdrawn",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
			}
			
		  //***Start
			if(flag_start)
			{
					HAL_TIM_Base_Start(&htim3);
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 3);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
					GetData();
					flag_start = 0;			
			}
		  //***Process
			GPS_Process();
			
			if(flag_uart)
			{
					Data_Package();
					flag_uart = 0;
			}
      //***exit
			if(flag_exit == 1)
			{
				HAL_Delay(10);
				HAL_TIM_Base_Stop(&htim3);
				HAL_ADC_Stop_DMA(&hadc1);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); 		
				
				ST7735_FillScreen(ST7735_BLACK);
		
				ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
				ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
				
				while(flag_exit == 1)
				{
					if(Counter >= 3)      {	 Counter = 1; }
					else if(Counter <=0)  {  Counter = 2; }
					ST7735_DrawString(10, 22,  "confirm logout",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
					ST7735_DrawString(10, 42,  "or click back",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
					ST7735_DrawString(10, 62,  "enter",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
					ST7735_DrawString(10, 82,  "back",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
				
					if(Counter == 1)
					{
						Menu_Blick(10, 62, 50, 62, "enter" , "" ,Font_7x10);
						
						if(Enter == 1)
						{
							Enter     = 0;
							flag_exit = 0;
							Exit      = 0;
							flag_draw	= 1;
						}
					}
					
					if(Counter == 2)
					{
						Menu_Blick(10, 82, 50, 82, "back" , "" ,Font_7x10);
					
						if(Enter == 1)
						{
							Enter     = 0;
							flag_exit = 0;
							Exit      = 1;
							flag_draw	= 1;									
						}
					}		
				}	
			}
		}
	}
}

void MainMenu_File()
{
	Counter = 1;
	Enter   = 0;
	Exit    = 1;
	flag_draw = 1;
	
	while(Exit != 0)
	{
		if(Counter >= 3)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 2; }
			
		if(flag_draw == 1)
		{
			flag_draw = 0;
			
			ST7735_FillScreen(ST7735_BLACK);

			ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
			ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

			ST7735_DrawString(10, 42,  "Write in",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 52,  "a new file",Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 62,  "Yes",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 72,  "No",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
		}

		if(Counter == 1)
		{
			Menu_Blick(10, 62,60, 62,"Yes","", Font_7x10);
			if(Enter == 1)
			{
				Enter = 0;
				File_Create();
				Exit = 0;
			}
		}
		if(Counter == 2)
		{
			Menu_Blick(10, 72,60, 72,"No","", Font_7x10);
			if(Enter == 1)
			{
				Enter = 0;
				Exit  = 0;
			}
		}
	}
}


void MainMenu_Setting()
{
	Menu_Blick(10, 62, 30, 62, "2" , "settings" ,Font_11x18);  
	if(Enter == 1)
	{
		
		Counter   = 1;
		Enter     = 0;
		Exit      = 1;
		flag_draw = 1;
		while(Exit != 0)
		{
			if(Counter >= 5)      {	 Counter = 1; }
		  else if(Counter <=0)  {  Counter = 4; }

			if(flag_draw == 1)
			{
				flag_draw = 0;
				
				ST7735_FillScreen(ST7735_BLACK);
				
				ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
				ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
				
				ST7735_DrawString(10, 42,  "1",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(30, 42,  "GPS",    Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 62,  "2",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(30, 62,  "antenna", Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 82,  "3",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(30, 82,  "display",     Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 102, "4",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(30, 102, "USB-MSD",     Font_11x18, ST7735_BLUE, ST7735_BLACK);
			}
			if(Counter == 1)
			{
				Menu_Blick(10, 42, 30, 42, "1" , "GPS" ,Font_11x18); 
				if(Enter == 1)
				{
					Counter   = 1;
					Enter     = 0;
					flag_draw = 1;
					
					while(Exit != 0)
					{
						if(Counter >= 3)      {	 Counter = 1; }
	          else if(Counter <=0)  {  Counter = 2; }
						
						if(flag_draw == 1)
						{
							flag_draw = 0;
							ST7735_FillScreen(ST7735_BLACK);

							ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
							ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

							ST7735_DrawString(10, 42,  "1",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(30, 42,  "GPS test", Font_11x18, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 62,  "2",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(30, 62,  "time",     Font_11x18, ST7735_BLUE, ST7735_BLACK);		
						}
						
						if(Counter == 1)
						{
							Menu_Blick(10, 42, 30, 42, "1" , "GPS test" ,Font_11x18); 
							
							if(Enter == 1)
							{
								Enter   = 0;
								ST7735_FillScreen(ST7735_BLACK);
								
								ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
								ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
							  char Txbuffs[5] = {0,};
					      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
		            GetData();
								while(Exit != 0)
								{
									GPS_Process();
									
									ST7735_DrawString(10, 22,  "time",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
									sprintf(Txbuffs, "%2d:%2d",GPS.GPGGA.UTC_Hour,GPS.GPGGA.UTC_Min);
									ST7735_DrawString(80, 22,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									
									ST7735_DrawString(10, 42,  "satellite",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									clear_buffer(Txbuffs);
									sprintf(Txbuffs, "%2d",GPS.GPGGA.SatellitesUsed);
									ST7735_DrawString(80, 42,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									
									ST7735_DrawString(10, 62,  "latitude",   Font_7x10, ST7735_BLUE, ST7735_BLACK);
									clear_buffer(Txbuffs);
									sprintf(Txbuffs, "%0.4f",GPS.GPGGA.LatitudeDecimal);
									ST7735_DrawString(80, 62,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									
									ST7735_DrawString(10, 82,  "longitude",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									clear_buffer(Txbuffs);
									sprintf(Txbuffs, "%0.4f",GPS.GPGGA.LongitudeDecimal);
									ST7735_DrawString(80, 82,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);

									ST7735_DrawString(10, 102, "height",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
									clear_buffer(Txbuffs);
									sprintf(Txbuffs, "%0.4f",GPS.GPGGA.MSL_Altitude);
									ST7735_DrawString(80, 102,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
									
									if(flag_exit == 1)
									{
										Exit      = 0;
										flag_exit = 0;
										flag_draw = 1;
										Enter     = 0;
										HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
									}
								}
							}  
						}
						if(Counter == 2)
						{
							Menu_Blick(10, 62, 30, 62, "2" , "time" ,Font_11x18); 
							if(Enter == 1)
							{
								Counter = 1;
								Enter   = 0;
								
								char Txbuffs[5] = {0,};
								clear_buffer(Txbuffs);
								
								ST7735_FillScreen(ST7735_BLACK);
								
								ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
								ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
								
								ST7735_DrawString(10, 22,  "setting the time",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
								ST7735_DrawString(10, 32,  "zone",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
								ST7735_DrawString(10, 142, "back", Font_7x10, ST7735_BLUE, ST7735_BLACK);
								sprintf(Txbuffs, "%2d", Get_UTC_Hour_Fix());
								ST7735_DrawString(10, 62,  Txbuffs,       Font_7x10, ST7735_BLUE, ST7735_BLACK);
								// настройка времени по часовом поясу
								while(Exit != 0)
								{
									if(Counter >= 3)      {	 Counter = 1; }
	                else if(Counter <=0)  {  Counter = 2; }
						
									if(Counter == 1)
									{
										Menu_Blick(10, 62,80, 62,Txbuffs,"",Font_7x10);
										if(Enter == 1)
										{
											Enter  = 0;
											Send_UTC_Hour_Fix(1);
											sprintf(Txbuffs, "%2d", Get_UTC_Hour_Fix());
											ST7735_DrawString(10, 62,  Txbuffs,       Font_7x10, ST7735_BLUE, ST7735_BLACK);
										}
										
										if(flag_exit == 1)
										{
											flag_exit = 0;
											Send_UTC_Hour_Fix(2);
											sprintf(Txbuffs, "%2d", Get_UTC_Hour_Fix());
											ST7735_DrawString(10, 62,  Txbuffs,       Font_7x10, ST7735_BLUE, ST7735_BLACK);
										}
									}
									
									if(Counter  == 2)
									{
										Menu_Blick(10, 142,80, 142,"back","",Font_7x10);
										
										if(Enter == 1)
										{
											Enter     = 0;
											Exit      = 0;
											flag_exit = 0;
											flag_draw = 1;
										}
									}
								}
							} 
						}
						Exit  = 1;
						if(flag_exit == 1)
						{
							Enter     = 0;
							Exit      = 0;
							flag_exit = 0;
							flag_draw = 1;
						}
					}	
				}  
			}
			
			if(Counter == 2)
			{
        Menu_Blick(10, 62, 30, 62, "2" , "antenna" ,Font_11x18); 
				if(Enter == 1)
				{
					Counter   = 1;
					Enter     = 0;
					flag_draw = 1;
					int Counter_Potentiometer_c = 0;
					int Counter_Potentiometer_s = 0;
					char Txbuffc[4] = {0,};
					char Txbuffs[4] = {0,};
					sprintf(Txbuffc, "%d", Counter_Potentiometer_c);
					sprintf(Txbuffs, "%d", Counter_Potentiometer_s);
					
					while(Exit != 0)
					{
						if(Counter >= 4)      {	 Counter = 1; }
	          else if(Counter <=0)  {  Counter = 3; }

						if(flag_draw == 1)
						{
							flag_draw = 0;
							ST7735_FillScreen(ST7735_BLACK);
				
							ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
							ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
							
							ST7735_DrawString(10, 12,  "calibration",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(20, 32,  "-128-127 ",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 42,  Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 72,  "sensitivity",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(20, 92,  "0-16",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 112, Txbuffs, Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 142, "back", Font_7x10, ST7735_BLUE, ST7735_BLACK);
						}
						
						ST7735_DrawString(10, 42,  "   ", Font_7x10, ST7735_BLUE, ST7735_BLACK);
						ST7735_DrawString(10, 112, "   ", Font_7x10, ST7735_BLUE, ST7735_BLACK);
						ST7735_DrawString(10, 42,  Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);
						ST7735_DrawString(10, 112, Txbuffs, Font_7x10, ST7735_BLUE, ST7735_BLACK);
						
						if(Counter == 1)
						{
							Menu_Blick(10, 42, 60, 42, Txbuffc , "" ,Font_7x10); 
							if(flag_exit == 1)
							{
								flag_exit = 0;
								Counter_Potentiometer_c-=8;
								sprintf(Txbuffc, "%d", Counter_Potentiometer_c);
							  Potentiometer_SendResistance(COMMAND_BIT_0, CHENNEL_SELECT_BIT_0, Counter_Potentiometer_c);
							}
							if(Enter == 1)
							{
								Enter = 0;
								Counter_Potentiometer_c+=8;
								sprintf(Txbuffc, "%d", Counter_Potentiometer_c);
								Potentiometer_SendResistance(COMMAND_BIT_0, CHENNEL_SELECT_BIT_0, Counter_Potentiometer_c);
							}
						}
						if(Counter == 2)
						{
							Menu_Blick(10, 112, 60, 112, Txbuffs, "" ,Font_7x10); 
							if(flag_exit == 1)
							{
								flag_exit = 0;
								Counter_Potentiometer_s--;
								sprintf(Txbuffs, "%d", Counter_Potentiometer_s);
								Potentiometer_SendResistance(COMMAND_BIT_0, CHENNEL_SELECT_BIT_1, Counter_Potentiometer_s);
							}
							if(Enter == 1)
							{
								Enter = 0;
								Counter_Potentiometer_s++;
								sprintf(Txbuffs, "%d", Counter_Potentiometer_s);
								Potentiometer_SendResistance(COMMAND_BIT_0, CHENNEL_SELECT_BIT_1, Counter_Potentiometer_s);
							}
						}
						if(Counter == 3)
						{
							Menu_Blick(10, 142, 60, 142, "back", "" ,Font_7x10);
							if(Enter == 1)
							{
								Exit      = 0;
								flag_exit = 0;
								flag_draw = 1;
								Enter     = 0;
							}
						}
					}
				}  
			}
			
			if(Counter == 3)
			{
				Menu_Blick(10, 82, 30, 82, "3" , "display" ,Font_11x18); 
				if(Enter == 1)
				{
					Enter     = 0;
					flag_draw = 1;
					Counter   = 1;
					while(Exit != 0)
					{
						if(Counter >= 3)      {	 Counter = 1; }
	          else if(Counter <=0)  {  Counter = 2; }
						
						if(flag_draw == 1)
						{
							flag_draw = 0;
							ST7735_FillScreen(ST7735_BLACK);

							ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
							ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
							
							ST7735_DrawString(90, 22,  "off",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 22,  "Inversion", Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(90, 32,  "off",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(10, 32,  "sharpness",     Font_7x10, ST7735_BLUE, ST7735_BLACK);		
						}
						
						if(Counter == 1)
			      {
							Menu_Blick(90, 22, 10, 22, "off" , "Inversion" ,Font_7x10);
							
						}
						if(Counter == 2)
			      {
							Menu_Blick(90, 32, 10, 32, "off" , "sharpness" ,Font_7x10);
						}
						
						if(flag_exit == 1)
						{
							Exit      = 0;
							flag_exit = 0;
							flag_draw = 1;
							Enter     = 0;
						}
						
					}
					
				}  
			}
			
			if(Counter == 4)
			{
				Menu_Blick(10, 102, 30, 102, "4", "USB-MSD" ,Font_11x18); 
				if(Enter == 1)
				{
					Enter     = 0;
					flag_draw = 1;
					char Buffer[10] = {0,};
					Card_Capacity();
					while(Exit != 0)
					{
						if(flag_draw == 1)
						{
							flag_draw = 0;
							
							ST7735_FillScreen(ST7735_BLACK);
							
							ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
							ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	

							sprintf(Buffer, "%d",capacity[0]);
							ST7735_DrawString(30, 32,  Buffer,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							
							clear_buffer(Buffer);
							sprintf(Buffer, "%d",capacity[1]);
							ST7735_DrawString(30, 52,  Buffer,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							
							ST7735_DrawString(30, 22,  "Free space",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
							ST7735_DrawString(30, 42,  "Total size",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
						}
						
						if(flag_exit == 1)
							// каталог файлов,  и его редоктирование
						{
							Enter     = 0;
							Exit      = 0;
							flag_exit = 0;
							flag_draw = 1;
						}	
					}
				}		
			}
		Exit = 1;
			if(flag_exit == 1)
			{
				Enter     = 0;
				flag_exit = 0;
				Exit      = 0;
				flag_draw	= 1;
			}
		} 		
	}  
}
void MainMenu_Help()
{
	if(Enter == 1)
	{
		Enter   = 0;
		
		ST7735_FillScreen(ST7735_BLACK);
		
		ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
		ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
		
		while(Exit != 0)
		{
			ST7735_DrawString(10, 22,  "In the settings,",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 42,  "check the GPS,",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 62,  "calibrate the",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 82,  "antenna, then",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
			ST7735_DrawString(10, 102, "select start",      Font_7x10, ST7735_BLUE, ST7735_BLACK);
			
			if(flag_exit == 1)
			{
				flag_exit = 0;
				Exit      = 0;
				flag_draw = 1;
			}
		}
	}
  Menu_Blick(10, 82, 30, 82, "3" , "help" ,Font_11x18);  
}

void MainMenu_Exit()
{
	if(Enter == 1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		uint8_t cmd = 0x10;
		HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	}	
	Menu_Blick(10, 102, 30, 102, "4" , "exit" ,Font_11x18);
}

void MainMenu_BattonClick_CallBack(uint16_t GPIO_PIN)
{
	switch (GPIO_PIN){
		//вверх
		case  GPIO_PIN_6:
			Counter++;
		  gpio_pin = GPIO_PIN_6;
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
			break;
		// вниз
		case  GPIO_PIN_7:
			Counter--;
		  gpio_pin = GPIO_PIN_7;
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
			break;
		// enter
		case  GPIO_PIN_2:
			Enter++;
		  if(Enter > 1) Enter = 1;
		  gpio_pin = GPIO_PIN_2;
			HAL_NVIC_DisableIRQ(EXTI2_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
		  break;
		//выход
		case  GPIO_PIN_3:
			flag_exit++;
		  if(flag_exit > 1) flag_exit = 1;
		  gpio_pin = GPIO_PIN_3;
			HAL_NVIC_DisableIRQ(EXTI3_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
      break;
		}
}

void MainMenu_ADC_CallBack()
{
	flag_adc = 1;
}
//********************************************************************//
//********************************************************************//

