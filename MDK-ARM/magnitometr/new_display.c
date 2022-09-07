#include "new_display.h"
#include <stdio.h>

#define States 17
#define States_Meas 4
#define MAX 10


extern TIM_HandleTypeDef htim10;

volatile uint8_t flag_start  = 1;
volatile uint8_t flag_adc    = 1;
volatile uint8_t Counter     = 1;
volatile uint8_t Enter       = 0;
volatile uint8_t flag_exit   = 0;
volatile uint8_t flag_enter  = 0;
extern   uint32_t capacity[2];

extern volatile uint16_t adc[3];
extern volatile unsigned long  LastTime;

volatile uint8_t Counter2 = 0;


typedef int State;  // состояние
typedef State (*Action)(State state); // указатель на функцию

// Перечисление состояний конечного автомата
typedef enum Enumeration_States{
	                       /////////////////////////////////////// 
                         //         /*States*/              /**/        
                         ///////////////////////////////////////  //////////////////////////////////////////////////////////////////////////////////////////// 	
	                       /* 0  */State_MainMenuParent = 0,  /**/  /*Отображаем главное меню                                                                 */
	                       /* 1  */State_MainMenuStart,       /**/  /*Указываем на пункт старт, необходимо нажать Enter, чтобы войти в следующее состояние    */
	                       /* 2  */State_MainMenuSetting,     /**/  /*Указываем на пункт настойки, необходимо нажать Enter, чтобы войти в следующее состояние */
                         /* 3  */State_MainMenuHelp,        /**/  /*Указываем на пункт помощь, необходимо нажать Enter, чтобы войти в следующее состояние   */
                         /* 4  */State_MainMenuExit,        /**/  /*Указываем на пункт выход, необходимо нажать Enter, чтобы войти в следующее состояние    */
                         /* 5  */State_StartCreateFile,     /**/  /*Переходим в меню создания файла                                                         */
                         /* 6  */State_StartMeasurement,    /**/  /*Переходим в меню измерения                                                              */
	                       /* 7  */State_Setting,             /**/  /*Отображаем меню настойки                                                                */
                         /* 8  */State_SettingGPS,          /**/  /*Указываем на пункт GPS, необходимо нажать Enter, чтобы войти в следующее состояние      */
	                       /* 9  */State_GPS,                 /**/  /*Отображаем меню GPS                                                                     */
                         /* 10 */State_GPSTest,             /**/  /*Указываем на пункт тест GPS, необходимо нажать Enter, чтобы войти в следующее состояние */
	                       /* 11 */State_GPSTime,             /**/  /*Указываем на пункт UTC GPS, необходимо нажать Enter, чтобы войти в следующее состояние  */
                         /* 12 */State_SettingAntenna,      /**/  /*Указываем на пункт Antenna, необходимо нажать Enter, чтобы войти в следующее состояние  */
                         /* 13 */State_SettingDispley,      /**/  /*Указываем на пункт Displey, необходимо нажать Enter, чтобы войти в следующее состояние  */
                         /* 14 */State_SettingUSB_MSD,      /**/  /*Указываем на пункт USB-MSD, необходимо нажать Enter, чтобы войти в следующее состояние  */
                         /* 15 */State_GoBack,              /**/  /*Указываем на пункт Go Back, необходимо нажать Enter, чтобы войти в следующее состояние  */
                         /* 16 */State_Idle                 /**/  /*Находимся в Idle                                                                        */
                         ///////////////////////////////////////  //////////////////////////////////////////////////////////////////////////////////////////// 
                         }State_Type; 

typedef enum Measurement_States{
		                     /////////////////////////////////////// 
                         //         /*States*/              /**/        
                         ////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////////////
                         /* 0  */State_MeasurementStart = 0,     /**/  /*Переходим в меню измерения                                                         */
                         /* 1  */State_MeasurementGPSProcess,    /**/  /*Переходим в меню измерения                                                         */
                         /* 2  */State_MeasurementDataPackage,   /**/  /*Переходим в меню измерения                                                         */
	                       /* 3  */State_MeasurementStop           /**/  /*Переходим в меню измерения                                                         */
	                       ////////////////////////////////////////////  /////////////////////////////////////////////////////////////////////////////////////// 
                         }State_Measurement;

State_Type curr_state;
State_Measurement curr_MeasState;
// структура перехода
typedef struct{
    State next;
    Action action;
}Transition,*pTransition;

//значание сопротивлений на MCP41010
typedef struct{
  int xtemp;
	int stemp;
	char Txbuffc[4];
	char Txbuffs[4];
}MCP41010;

MCP41010 mcp41010 = {.stemp = 0,
                     .xtemp = 0,
                     .Txbuffc = {0,},
                     .Txbuffs = {0,}};

// Круговая структура очереди
typedef struct{
    int queue[MAX];
    int head;
    int tail;
    int overflow;
}Queue,*pQueue;

// Структура конечного автомата
typedef struct{
    State current;
    int inTransition;
    Queue squeue;
}Statemachine, *pStatemachine;

// объявление функции действия
State MainMenuParent(State state);
State MainMenuStart(State state);
State MainMenuSetting(State state);
State MainMenuHelp(State state);
State MainMenuExit(State state);
State StartCreateFile(State state);
State StartMeasurement(State state);
State Setting(State state);
State SettingGPS(State state);
State GPSFun(State state);
State GPSTest(State state);
State GPSTime(State state);
State SettingAntenna(State state);
State SettingDispley(State state);
State SettingUSB_MSD(State state);
State GoBack(State state);
State Idle(State state);

State MeasurementStart(State state);
State MeasurementGPSProcess(State state);
State MeasurementDataPackage(State state);
State MeasurementStop(State state);


// Определяем переменную мьютекса
//[s0,c0,a1,s15]
Transition take_MainMenu          = {State_MainMenuStart, MainMenuParent};
//[s2,c2,a2,s15]                     
Transition take_MainMenuStart     = {State_Idle, MainMenuStart};
//[s3,c3,a3,s15]                     
Transition take_MainMenuSetting   = {State_Idle, MainMenuSetting};
//[s1,c2,a4,s15]                     
Transition take_MainMenuHelp      = {State_Idle, MainMenuHelp};
//[s1,c2,a5,s15]                     
Transition take_MainMenuExit      = {State_Idle, MainMenuExit};
//[s1,c2,a6,s15]                     
Transition take_StartCreateFile   = {State_Idle, StartCreateFile};
//[s1,c2,a7,s15]                     
Transition take_StartMeasurement  = {State_Idle, StartMeasurement};
//[s1,c2,a8,s15]                     
Transition take_Setting           = {State_Idle, Setting};
//[s1,c2,a8,s15]                     
Transition take_SettingGPS        = {State_Idle, SettingGPS};
//[s1,c2,a8,s15]  
Transition take_GPS               = {State_Idle, GPSFun};
//[s1,c2,a9,s15]                     
Transition take_GPSTest           = {State_Idle, GPSTest};
//[s1,c2,a10,s15]                    
Transition take_GPSTime           = {State_Idle, GPSTime};
//[s1,c2,a11,s15]                    
Transition take_SettingAntenna    = {State_Idle, SettingAntenna};
//[s1,c2,a12,s15]                    
Transition take_SettingDispley    = {State_Idle, SettingDispley};
//[s1,c2,a13,s15]                    
Transition take_SettingUSB_MSD    = {State_Idle, SettingUSB_MSD};
//[s1,c2,a14,s15]                    
Transition take_GoBack 						= {State_Idle, GoBack};
//[s1,c2,a15,s15]                    
Transition take_Idle							= {State_Idle, Idle};


//[s1,c2,a15,s15]                    
Transition take_MeasurementStart			  = {State_Idle, MeasurementStart};
//[s1,c2,a15,s15]                    
Transition take_MeasurementGPSProcess		= {State_Idle, MeasurementGPSProcess};
//[s1,c2,a15,s15]                    
Transition take_MeasurementDataPackage	= {State_Idle, MeasurementDataPackage};
//[s1,c2,a15,s15]                    
Transition take_MeasurementStop	        = {State_Idle, MeasurementStop};

// Таблица преобразования
pTransition transition_table[States]=
{ 
   /////////////////////////////////////// 
   //         /*States*/              /**/        
   /////////////////////////////////////// 	
   /* 0  */&take_MainMenu, 	          /**/    
   /* 1  */&take_MainMenuStart,       /**/    
   /* 2  */&take_MainMenuSetting,     /**/    
   /* 3  */&take_MainMenuHelp,        /**/    
   /* 4  */&take_MainMenuExit,        /**/    
   /* 5  */&take_StartCreateFile,     /**/    
   /* 6  */&take_StartMeasurement,    /**/ 
   /* 7  */&take_Setting,             /**/ 	
   /* 8  */&take_SettingGPS,          /**/ 
   /* 9  */&take_GPS,                 /**/	
   /* 10 */&take_GPSTest,             /**/    
   /* 11 */&take_GPSTime,             /**/    
   /* 12 */&take_SettingAntenna,      /**/    
   /* 13 */&take_SettingDispley,      /**/    
   /* 14 */&take_SettingUSB_MSD,      /**/    
   /* 15 */&take_GoBack,              /**/    
   /* 16 */&take_Idle,                /**/     
   /////////////////////////////////////// 
};

// Таблица преобразования для измерений
pTransition Measurement_transition_table[States_Meas]=
{ 
   //////////////////////////////////////////// 
   //         /*States*/                   /**/        
   //////////////////////////////////////////// 	
   /* 0  */&take_MeasurementStart, 	       /**/    
   /* 1  */&take_MeasurementGPSProcess,    /**/    
   /* 2  */&take_MeasurementDataPackage,   /**/  
   /* 2  */&take_MeasurementStop,          /**/  	
   //////////////////////////////////////////// 
};
/***************************************************************************************
** Function name:           MCP41010_SPI_SendOneByte
** Description:             Sending data by SPI
***************************************************************************************/
void static MCP41010_SPI_SendOneByte(int xtemp)
{
	uint8_t xnum = 0;
	
	 /* Dp_sck отправляет 8 импульсов, отправка данных при растущих ребрах */
	for(xnum=0; xnum<8; xnum++)
	{
		 // dp_si запись данных
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (xtemp & 0x80) >>7);
		//(xtemp & 0x80) >>7;
		xtemp = xtemp << 1;
		
		 // После задержания данных стабильна, расшифровка пограничного импульса генерируется в DP_SCK, и данные отправляются на MCP41010.
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
		 // dp_sck создает падающий импульс
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
	}
}
/***************************************************************************************
** Function name:           initialize
** Description:             initialization of a finite state machine
***************************************************************************************/
void initialize(pStatemachine machine, State_Type state_types)
{	
    machine->current = state_types;
    machine->inTransition = false;
    machine->squeue.head =0 ;
    machine->squeue.overflow = false;
}
/***************************************************************************************
** Function name:           InitializeMeasurement
** Description:             initialization of a finite state machine Measurement
***************************************************************************************/
void InitializeMeasurement(pStatemachine machine, State_Measurement state_types)
{
    machine->current = state_types;
    machine->inTransition = false;
    machine->squeue.head =0 ;
    machine->squeue.overflow = false;
}
/***************************************************************************************
** Function name:           takeAction
** Description:             performs the current state
***************************************************************************************/
static State takeAction(pStatemachine machine)
{
    State current = machine->current;
    pTransition p = transition_table[current];
    current = (*(p->action))(current);
    //current = p->next;
    machine->current = current;
    return current;
}
/***************************************************************************************
** Function name:           takeActionMeasurement
** Description:             performs the current state Measurement
***************************************************************************************/
static State takeActionMeasurement(pStatemachine machine)
{
    State current = machine->current;
    pTransition p = Measurement_transition_table[current];
    current = (*(p->action))(current);
    //current = p->next;
    machine->current = current;
    return current;
}
/***************************************************************************************
** Function name:           StateMachin
** Description:             starts the state machine
***************************************************************************************/
void StateMachin()
	{
	Statemachine machine;
	curr_state = State_MainMenuParent;	
	initialize(&machine, curr_state);	
	
	while(1)
	{
		takeAction(&machine);
	}
}



/***************************************************************************************
** Function name:           Menu_Blick
** Description:             displays the selected
***************************************************************************************/
void Menu_Blick(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const char* str1, const char* str2, FontDef font)
{
 ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, 0x1414);
	if(Counter2) {
		ST7735_DrawString(x1, y1,  str1, font, ST7735_BLUE, ST7735_BLACK);
		ST7735_DrawString(x2, y2,  str2, font, ST7735_BLUE, ST7735_BLACK);
		--Counter2;
	}
}

/***************************************************************************************
** Function name:           MainMenuParent
** Description:             Draw Main Menu Parent
***************************************************************************************/
State MainMenuParent(State state)
{
	Counter   = 1;
	// отрисовка главного меню
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
	// выбор функции	
  while(1)
	{		
	if(Counter >= 5)      {	 Counter = 1; }
	else if(Counter <=0)  {  Counter = 4; }

	if(Counter == 1)
		state = State_MainMenuStart; 
	
	else if(Counter == 2)
		state = State_MainMenuSetting;
	
	else if(Counter == 3)
		state = State_MainMenuHelp; 
	
	else if(Counter == 4)
		state = State_MainMenuExit; 
	
	return state;
  }
}
/***************************************************************************************
** Function name:           MainMenuStart
** Description:             go to the start menu
***************************************************************************************/
State MainMenuStart(State state)
{
	Menu_Blick(10, 42, 30, 42, "1" , "start" ,Font_11x18); 
	
	if(Counter == 1 && Enter == 1) {
		Enter = 0;
		state = State_StartCreateFile;
	}else
		state = State_MainMenuParent;
	
	return state;
}
/***************************************************************************************
** Function name:           MainMenuSetting
** Description:             go to the start menu setting
***************************************************************************************/
State MainMenuSetting(State state)
{
	Menu_Blick(10, 62, 30, 62, "2" , "settings" ,Font_11x18);  
	
	if(Counter == 2 && Enter == 1){
		Enter = 0;
		state = State_Setting;
	}else
		state = State_MainMenuParent;
	
	return state;
}
/***************************************************************************************
** Function name:           MainMenuHelp
** Description:             go to the start menu Help
***************************************************************************************/

State MainMenuHelp(State state)
{
	Menu_Blick(10, 82, 30, 82, "3" , "help" ,Font_11x18); 
	
	if(Counter == 3 && Enter == 1) {
		Enter = 0;
		MainMenu_Help_();	
	}
	
	state = State_MainMenuParent;	
	return state;
}
/***************************************************************************************
** Function name:           MainMenuExit
** Description:             go to the start menu Exit
***************************************************************************************/
State MainMenuExit(State state)
{
	Menu_Blick(10, 102, 30, 102, "4" , "exit" ,Font_11x18);
	
	if(Counter == 4 && Enter == 1){
		Enter = 0;
		MainMenu_Exit_();
  }else
		state = State_MainMenuParent;
	
	return state;
}

/***************************************************************************************
** Function name:           StartCreateFile
** Description:             Create File
***************************************************************************************/
State StartCreateFile(State state)
{
  Counter = 1;

	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

	ST7735_DrawString(10, 42,  "Write in",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 52,  "a new file",Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "Yes",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 72,  "No",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 102,  "Back",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	
  while(1)
	{
		if(Counter >= 4)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 3; }
		
		if(Counter == 1)
		{
			Menu_Blick(10, 62,60, 62,"Yes","", Font_7x10);
			if(Enter == 1)
			{
				Enter = 0;
				File_Create();                                             // not implemented
				state = State_StartMeasurement;
	      return state;
			}
		}
		if(Counter == 2)
		{
			Menu_Blick(10, 72,60, 72,"No","", Font_7x10);
			if(Enter == 1)
			{
				Enter = 0;
				state = State_StartMeasurement;
	      return state;
			}
		}
		if(Counter == 3)
		{
			Menu_Blick(10, 102,60, 102,"Back","", Font_7x10);
			if(Enter == 1)
			{
				Enter = 0;
				state = State_MainMenuParent;
	      return state;
			}
		}
	}
}
/***************************************************************************************
** Function name:           StartMeasurement
** Description:             Start Measurement
***************************************************************************************/
State StartMeasurement(State state)
{
	Counter = 1;
	char Txbuffc[4] = {0,};
		
	sprintf(Txbuffc, "%d", adc[0]);
	
	Statemachine Measurement;
		
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

	ST7735_DrawString(10, 42,  "data is being",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "withdrawn",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 82,  Txbuffc,            Font_7x10, ST7735_BLUE, ST7735_BLACK);
	
	InitializeMeasurement(&Measurement, State_MeasurementStart);
	
	while(1)
	{
		
		takeActionMeasurement(&Measurement);
		if(Measurement.current == 0)
		{
			state = State_MainMenuParent;
			return state;
		}
	}
}
/***************************************************************************************
** Function name:           MeasurementStart
** Description:             Measurement Start
***************************************************************************************/

State MeasurementStart(State state)
{
	if(flag_start)
	{
		HAL_TIM_Base_Start(&htim3);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 3);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
		GetData();
		flag_start = 0;	
	}
	
	LastTime = HAL_GetTick();
	
	if(flag_exit)
	state = State_MeasurementStop;
	else
	state = State_MeasurementGPSProcess;

	return state;
}
/***************************************************************************************
** Function name:           MeasurementGPSProcess
** Description:             Measurement GPS Process
***************************************************************************************/
State MeasurementGPSProcess(State state)
{
	if((HAL_GetTick() - LastTime) > 100)
	{
		GPS_Process();
		state = State_MeasurementDataPackage;	
		LastTime = HAL_GetTick();
  }
	else if(flag_exit)  
		state = State_MeasurementStop;
	else 
    state = State_MeasurementGPSProcess;	
	  
	return state;
}
/***************************************************************************************
** Function name:           MeasurementDataPackage
** Description:             Data Package
***************************************************************************************/
State MeasurementDataPackage(State state)
{
	Data_Package();
	
	if(flag_exit)
	state = State_MeasurementStop;
	else
	state = State_MeasurementGPSProcess;

	return state;
}
/***************************************************************************************
** Function name:           MeasurementStop
** Description:             Measurement Stop
***************************************************************************************/
State MeasurementStop(State state)
{
//	останавилаем процесс 
  HAL_TIM_Base_Stop(&htim3);
	HAL_ADC_Stop_DMA(&hadc1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); 		
	
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(10, 22,  "confirm logout",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 42,  "or click back",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "enter",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 82,  "back",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	
	while(1)
	{
    if(Counter >= 3)      {	 Counter = 1; }
	  else if(Counter <=0)  {  Counter = 2; }

		if(Counter == 1)
		{
			Menu_Blick(10, 62, 50, 62, "enter" , "" ,Font_7x10);
			
			if(Enter == 1)
			{
				Enter     = 0;                                         //???????
				state = State_MainMenuParent;
        return state;			                                     //???????
			}                                                        //???????
		}                                                          //???????
																															 //???????   
		if(Counter == 2)                                           //???????  в данный момент вместо перехода в State_MainMenuParent мы перейдём в
			{                                                        //???????  State_MeasurementStart, надо подумать над логикой выхода из цикла работ
			Menu_Blick(10, 82, 50, 82, "back" , "" ,Font_7x10);      //???????  это автомата во внешний автомат
																															 //???????
			if(Enter == 1)                                           //???????
			{                                                        //???????
				flag_exit = 0;                                         //???????	                                  
				state = State_MainMenuParent;                          //???????
				return state;
			}
		}		
	}	
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State Setting(State state)
{
	Counter   = 1;

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

	while(1)
	{
		if(Counter >= 5)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 4; }
		
		if(Counter == 1)
		{
			Menu_Blick(10, 42, 30, 42, "1" , "GPS" ,Font_11x18); 
			if(Enter == 1)
			{
				Enter = 0;
				state = State_SettingGPS;
				return state;
			}  
		}
		
		if(Counter == 2)
		{
			Menu_Blick(10, 62, 30, 62, "2" , "antenna" ,Font_11x18); 
			if(Enter == 1)
			{
				Enter     = 0;
				state = State_SettingAntenna;
				return state;
			}  
		}
		
		if(Counter == 3)
		{
			Menu_Blick(10, 82, 30, 82, "3" , "display" ,Font_11x18); 
			if(Enter == 1)
			{
				Enter     = 0;
				state = State_SettingDispley;
				return state;
			}  
		}
		
		if(Counter == 4)
		{
			Menu_Blick(10, 102, 30, 102, "4", "USB-MSD" ,Font_11x18); 
			if(Enter == 1)
			{
				Enter = 0;
				state = State_SettingUSB_MSD;
				return state;
			}		
		}
		if(flag_exit)
		{
			flag_exit = 0;
			state = State_MainMenuParent;
			return state;
		}
	} 	
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State SettingGPS(State state)
{
	Counter   = 1;
	
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

	ST7735_DrawString(10, 42,  "1",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(30, 42,  "GPS test", Font_11x18, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "2",        Font_11x18, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(30, 62,  "time",     Font_11x18, ST7735_BLUE, ST7735_BLACK);	
	
	while(1)
	{
		if(Counter >= 3)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 2; }
		
		if(Counter == 1)
		{
			Menu_Blick(10, 42, 30, 42, "1" , "GPS test" ,Font_11x18); 
			if(Enter == 1)
			{
				Enter   = 0;
			  state = State_GPSTest;
				return state;
			}  
		}
		
		if(Counter == 2)
		{
			Menu_Blick(10, 62, 30, 62, "2" , "time" ,Font_11x18); 
			if(Enter == 1)
			{
				Counter = 1;
				Enter   = 0;
			  state = State_GPSTime;
				return state;
			} 
		}
		
		if(flag_exit)
		{
			flag_exit = 0;
			state = State_Setting;
			return state;
		} 
	}	
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State GPSTest(State state)
{
	ST7735_FillScreen(ST7735_BLACK);
	
	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(10, 22,  "time",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 42,  "satellite",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "latitude",   Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 82,  "longitude",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
  ST7735_DrawString(10, 102, "height",     Font_7x10, ST7735_BLUE, ST7735_BLACK);	
	char Txbuffs[5][7] = {0,};
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	GetData();
	
	while(1)
	{
		GPS_Process();
		
		sprintf(Txbuffs[0], "%2d:%2d",GPS.GPGGA.UTC_Hour,GPS.GPGGA.UTC_Min);
		sprintf(Txbuffs[1], "%2d",    GPS.GPGGA.SatellitesUsed);
		sprintf(Txbuffs[2], "%0.4f",  GPS.GPGGA.LatitudeDecimal);
		sprintf(Txbuffs[3], "%0.4f",  GPS.GPGGA.LongitudeDecimal);
		sprintf(Txbuffs[4], "%0.4f",  GPS.GPGGA.MSL_Altitude);
		
		// 1 вариант отображения значений
		for(uint8_t i = 0; i < 5; i++)
		{
			ST7735_DrawString(80, i*20+22,   Txbuffs[i],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
		}
		
		for(uint8_t i = 0; i < 5; i++)
		{
			clear_buffer(Txbuffs[i]);
		}
		
		// 2 вариант отображения значений
/*
		ST7735_DrawString(80, 22,   Txbuffs[0],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
		ST7735_DrawString(80, 42,   Txbuffs[1],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	  ST7735_DrawString(80, 62,   Txbuffs[2],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
    ST7735_DrawString(80, 82,   Txbuffs[3],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
		ST7735_DrawString(80, 102,  Txbuffs[4],  Font_7x10, ST7735_BLUE, ST7735_BLACK);
*/		
		
		
		
		// 3 вариант отображения значений
/*		sprintf(Txbuffs, "%2d:%2d",GPS.GPGGA.UTC_Hour,GPS.GPGGA.UTC_Min);
//		ST7735_DrawString(80, 22,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
//		
//		clear_buffer(Txbuffs);
//		sprintf(Txbuffs, "%2d",GPS.GPGGA.SatellitesUsed);
//		ST7735_DrawString(80, 42,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
//		

//		clear_buffer(Txbuffs);
//		sprintf(Txbuffs, "%0.4f",GPS.GPGGA.LatitudeDecimal);
//		ST7735_DrawString(80, 62,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);
//		

//		clear_buffer(Txbuffs);
//		sprintf(Txbuffs, "%0.4f",GPS.GPGGA.LongitudeDecimal);
//		ST7735_DrawString(80, 82,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK);

//		clear_buffer(Txbuffs);
//		sprintf(Txbuffs, "%0.4f",GPS.GPGGA.MSL_Altitude);
//		ST7735_DrawString(80, 102,  Txbuffs,  Font_7x10, ST7735_BLUE, ST7735_BLACK); */
		
		if(flag_exit)
		{
			flag_exit = 0;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
			state = State_SettingGPS;
			return state;
		}
	}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State GPSTime(State state)
{
	char Txbuffs[5] = {0,};
  sprintf(Txbuffs, "%2d", Get_UTC_Hour_Fix());
	
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);

	ST7735_DrawString(10, 22,  "setting the time",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 32,  "zone",       Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  Txbuffs,       Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 142, "back", Font_7x10, ST7735_BLUE, ST7735_BLACK);
	

	// настройка времени по часовом поясу
	while(1)
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
				state = State_SettingGPS;
				return state;
			}
		}
	}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State SettingAntenna(State state)
{
	Counter   = 1;
	
	sprintf(mcp41010.Txbuffc, "%d", mcp41010.xtemp);
	sprintf(mcp41010.Txbuffs, "%d", mcp41010.stemp);
	
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(10, 12,  "calibration",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(20, 32,  "-128-127 ",      Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 42,  mcp41010.Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 72,  "sensitivity",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(20, 92,  "0-16",           Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 112, mcp41010.Txbuffs, Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 142, "back",           Font_7x10, ST7735_BLUE, ST7735_BLACK);

	while(1)
	{
		if(Counter >= 4)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 3; }

		if(Counter == 1)
		{
			Menu_Blick(10, 42, 60, 42, mcp41010.Txbuffc , "" ,Font_7x10); 
			if(flag_exit)
			{
				flag_exit = 0;
				mcp41010.xtemp-=8;
				
				if(mcp41010.xtemp<-128) mcp41010.xtemp=-128;
				
				sprintf(mcp41010.Txbuffc, "%d", mcp41010.xtemp);
				ST7735_DrawString(10, 42,  "       ",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 42,  mcp41010.Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);
				
//				while(flag_toggle == 5){                                                              
//					xtemp-=8;
//					ST7735_DrawString(15, 42,  "      ", Font_7x10, ST7735_BLUE, ST7735_BLACK);
//					sprintf(Txbuffc, "%d", xtemp);
//					//ST7735_DrawString(10, 42,  Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);
//					if(flag_toggle == 5)	HAL_Delay(100);
//				}
				
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				MCP41010_SPI_SendOneByte(0x13);
				MCP41010_SPI_SendOneByte(mcp41010.xtemp*10);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			}
			
			if(Enter)
			{
				Enter = 0;
				mcp41010.xtemp+=8;
				
				if(mcp41010.xtemp>127) mcp41010.xtemp=127;
				
				sprintf(mcp41010.Txbuffc, "%d", mcp41010.xtemp); 
				ST7735_DrawString(10, 42,  "     ",          Font_7x10, ST7735_BLUE, ST7735_BLACK);                                                                                     
				ST7735_DrawString(10, 42,  mcp41010.Txbuffc, Font_7x10, ST7735_BLUE, ST7735_BLACK);                     
																																																			 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				MCP41010_SPI_SendOneByte(0x13);
				MCP41010_SPI_SendOneByte(mcp41010.xtemp*10);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			}
		}
		if(Counter == 2)
		{
			Menu_Blick(10, 112, 60, 112, mcp41010.Txbuffs, "" ,Font_7x10); 
			if(flag_exit)
			{
				flag_exit = 0;
				mcp41010.stemp--;
				
				if(mcp41010.stemp<=0) mcp41010.stemp=0;
				
				sprintf(mcp41010.Txbuffs, "%d", mcp41010.stemp);
				ST7735_DrawString(10, 112, "   ",            Font_7x10, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 112, mcp41010.Txbuffs, Font_7x10, ST7735_BLUE, ST7735_BLACK);
				
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				MCP41010_SPI_SendOneByte(0x13);
				MCP41010_SPI_SendOneByte(mcp41010.stemp*10);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			}
			
			if(Enter)
			{
				Enter = 0;
				mcp41010.stemp++;
				
				if(mcp41010.stemp>=16) mcp41010.stemp=16;
				
				sprintf(mcp41010.Txbuffs, "%d", mcp41010.stemp);
				ST7735_DrawString(10, 112,  "   ",           Font_7x10, ST7735_BLUE, ST7735_BLACK);
				ST7735_DrawString(10, 112, mcp41010.Txbuffs, Font_7x10, ST7735_BLUE, ST7735_BLACK);
				
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				MCP41010_SPI_SendOneByte(0x13);
				MCP41010_SPI_SendOneByte(mcp41010.stemp*10);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			}
		}
		
		if(Counter == 3)
		{
			Menu_Blick(10, 142, 60, 142, "back", "" ,Font_7x10);
			if(Enter)
			{
				Enter     = 0;
				state = State_Setting;
				return state;
			}
		}
	}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State SettingDispley(State state)
{
	Counter   = 1;
	uint8_t invert[2] = {1, 1};
	char *Toggle[2] = {"off", "no"};
	char *ToggleReply[2] = {Toggle[0], Toggle[0]};
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(90, 22,  "off",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 22,  "Inversion",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(90, 32,  "off",        Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 32,  "sharpness",  Font_7x10, ST7735_BLUE, ST7735_BLACK);	
	
	while(1)
	{  
		if(Counter >= 3)      {	 Counter = 1; }
		else if(Counter <=0)  {  Counter = 2; }
		
		if(Counter == 1)
		{
			Menu_Blick(90, 22, 10, 22, ToggleReply[0] , "Inversion" ,Font_7x10);
			if(Enter)
			{
				Enter = 0;
			  ST7735_InvertColors(invert[0]);
				
				if(invert[0])  invert[0] = 0;
				else           invert[0] = 1;
				
        ToggleReply[0] = Toggle[invert[0]];				
			}
		}
		if(Counter == 2)
		{
			Menu_Blick(90, 32, 10, 32, ToggleReply[1] , "sharpness" ,Font_7x10);
			if(Enter)
			{
				Enter = 0;
				ST7735_IdleMode(invert[1]);
				
				if(invert[1])  invert[1] = 0;
				else           invert[1] = 1;
				
        ToggleReply[1] = Toggle[invert[1]];				
			}
		}
		
		if(flag_exit)
		{
			flag_exit = 0;
			state = State_Setting;
			return state;
		}
	}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State SettingUSB_MSD(State state)
{
	char Buffer[2][10] = {0,};
	Card_Capacity();
	
	sprintf(Buffer[0], "%d",capacity[0]);
	sprintf(Buffer[1], "%d",capacity[1]);
	
	ST7735_FillScreen(ST7735_BLACK);
	
	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(30, 22,  "Free space",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(30, 32,  Buffer[0],     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(30, 42,  "Total size",  Font_7x10, ST7735_BLUE, ST7735_BLACK);		
	ST7735_DrawString(30, 52,  Buffer[1],     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	
	clear_buffer(Buffer[0]);
	clear_buffer(Buffer[1]);
	
	while(1)
	{
		// каталог файлов,  и его редоктирование
		// void fun()
		// {
		//   ......
		// }
		
		if(flag_exit)
		{
			flag_exit = 0;
			state = State_Setting;
    	return state;
		}	
	}

}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State GoBack(State state)
{
	return state;
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
State Idle(State state)
{
	return state;
}

State GPSFun(State state)
{
  return state;
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
void MainMenu_Help_()
{
	ST7735_FillScreen(ST7735_BLACK);
	
	ST7735_DrawRect(0, 0, 128, 160, ST7735_GREEN);
	ST7735_DrawRect(3, 3, 122, 154, ST7735_GREEN);
	
	ST7735_DrawString(10, 22,  "In the settings,",  Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 42,  "check the GPS,",    Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 62,  "calibrate the",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 82,  "antenna, then",     Font_7x10, ST7735_BLUE, ST7735_BLACK);
	ST7735_DrawString(10, 102, "select start",      Font_7x10, ST7735_BLUE, ST7735_BLACK);
	
	while(1)
	{
		if(flag_exit)
		{
			flag_exit = 0;
			return;
		}
	}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
void MainMenu_Exit_()
{
	ST7735_Backlight_Off();
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
void MainMenu_BattonClick_CallBack(uint16_t GPIO_PIN)
{
	++Counter2;
	uint8_t counter_old = Counter;
	LastTime = HAL_GetTick();
	switch (GPIO_PIN){
		//вверх
		case  GPIO_PIN_6:
			Counter--;
		  gpio_pin = GPIO_PIN_6;
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
			break;
		// вниз
		case  GPIO_PIN_7:
			Counter++;
		  gpio_pin = GPIO_PIN_7;
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
			break;
		// enter
		case  GPIO_PIN_2:
			Enter++;
	  	HAL_TIM_Base_Start_IT(&htim10);
		
		  if(Enter > 1) Enter = 1;
		
		  gpio_pin = GPIO_PIN_2;
			HAL_NVIC_DisableIRQ(EXTI2_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11); // запускаем таймер
		  break;
		//выход
		case  GPIO_PIN_3:
			flag_exit++;
		  HAL_TIM_Base_Start_IT(&htim10);
		
		  if(flag_exit > 1) flag_exit = 1;
		
		  gpio_pin = GPIO_PIN_3;
			HAL_NVIC_DisableIRQ(EXTI3_IRQn); // сразу же отключаем прерывания на этом пине   
      HAL_TIM_Base_Start_IT(&htim11);  // запускаем таймер
      break;
		}
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
void MainMenu_Timer10_CallBack()
{
	 flag_exit = 0;
	 Enter     = 0;
}
/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
void MainMenu_ADC_CallBack()
{
	flag_adc = 1;
}
//********************************************************************//
//********************************************************************//