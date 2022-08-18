#include "stm32f4xx_hal.h"

#ifndef __POTENTIOMETER_H
#define __POTENTIOMETER_H

#define CS_ACTIVATE()           (GPIOA->ODR &= ~GPIO_ODR_ODR1)
#define CS_DIACTIVATE()         (GPIOA->ODR |= GPIO_ODR_ODR1)

#define COMMAND_BIT_0           0X10 // Записать данные, содержащиеся в байте данных, в потенциометре
#define COMMAND_BIT_1           0X20 // (Выключение) Потенциометр (ы), определяемый битами выбора потенциометра, перейдет в режим отключения.

#define CHENNEL_SELECT_BIT_0    0X01 // Команда, выполняемая на Потенциометр 0
#define CHENNEL_SELECT_BIT_1    0X02 // Команда, выполняемая на Потенциометр 1
#define CHENNEL_SELECT_BIT_3    0X03 // Команда, выполняемая на обоих Потенциометрах

extern SPI_HandleTypeDef hspi2;


void Potentiometer_SendBytes(uint8_t reg, uint8_t data);
void Potentiometer_Inint(void);
void Potentiometer_SendResistance(uint8_t CommandBit, uint8_t Potentiometer, uint8_t data);

uint8_t Potentiometer_GetResistance(void);

#ifdef __cplusplus
}
#endif

#endif 