#include "stm32f4xx_hal.h"
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "Potentiometer.h"
#include "magnetic.h"

#ifndef __NEW_DISPLAY_H
#define __NEW_DISPLAY_H


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim10;
//extern volatile uint8_t flag_toggle;
volatile uint8_t Counter2 = 0;

extern uint32_t gpio_pin;


void Menu_Blick_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const char* str1, const char* str2, FontDef font);
void MainMenu_Help_(void);
void MainMenu_Exit_(void);
void MainMenu_BattonClick_CallBack_(uint16_t GPIO_PIN);
void MainMenu_ADC_CallBack_(void);
void MainMenu_Timer10_CallBack_(void);

void StateMachin(void);

#ifdef __cplusplus
}
#endif

#endif