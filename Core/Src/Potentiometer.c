#include "Potentiometer.h"

uint8_t DataResister = 0;

void Potentiometer_SendBytes(uint8_t reg, uint8_t data)
{
  while((SPI2->SR & SPI_SR_TXE) == RESET);  
  SPI2->DR = (uint16_t)((reg << 8) | data );
	while((SPI2->SR & SPI_SR_BSY) != RESET);
}
/*----------------------------------------------------------------------------*/


void Potentiometer_Inint()
{
	// сопротивление меняется в пределах от 0-255
	Potentiometer_SendResistance(COMMAND_BIT_0, CHENNEL_SELECT_BIT_3 , 128);
}

void Potentiometer_SendResistance(uint8_t CommandBit, uint8_t Potentiometer, uint8_t data)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	
	DataResister = data;
	// сопротивление меняется в пределах от 0-255
	Potentiometer_SendBytes((CommandBit | Potentiometer),data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}
uint8_t Potentiometer_GetResistance()
{
	return DataResister;
}
