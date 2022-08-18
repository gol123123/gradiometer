#include "stm32f4xx_hal.h"
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "Potentiometer.h"
#include "magnetic.h"

#ifndef __DISPLAYS_H
#define __DISPLAYS_H


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim11;
//volatile uint8_t flag_toggle = 1;

extern uint32_t gpio_pin;


void Menu_Blick(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const char* str1, const char* str2, FontDef font);
void MainMenu_Draw(void);
void MainMenu_Parent(void);
void MainMenu_Start(void);
void MainMenu_File(void);
void MainMenu_Setting(void);
void MainMenu_Help(void);
void MainMenu_Exit(void);
void MainMenu_BattonClick_CallBack(uint16_t GPIO_PIN);
void MainMenu_ADC_CallBack(void);
#ifdef __cplusplus
}
#endif

#endif