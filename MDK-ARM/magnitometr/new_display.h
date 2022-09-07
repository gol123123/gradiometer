#include "stm32f4xx_hal.h"
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "magnetic.h"

#ifndef __NEW_DISPLAY_H
#define __NEW_DISPLAY_H


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim10;
//extern volatile uint8_t flag_toggle;;

extern uint32_t gpio_pin;
extern volatile uint8_t flag_toggle;

void Menu_Blick(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const char* str1, const char* str2, FontDef font);
void MainMenu_Help_(void);
void MainMenu_Exit_(void);
void MainMenu_BattonClick_CallBack(uint16_t GPIO_PIN);
void MainMenu_ADC_CallBack(void);
void MainMenu_Timer10_CallBack(void);

void StateMachin(void);

#ifdef __cplusplus
}
#endif

#endif