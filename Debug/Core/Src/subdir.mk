################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/GPS.c \
../Core/Src/Potentiometer.c \
../Core/Src/diskio.c \
../Core/Src/displays.c \
../Core/Src/ff.c \
../Core/Src/ff_gen_drv.c \
../Core/Src/fonts.c \
../Core/Src/magnetic.c \
../Core/Src/main.c \
../Core/Src/st7735.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/usbd_conf.c \
../Core/Src/usbd_core.c \
../Core/Src/usbd_ctlreq.c \
../Core/Src/usbd_ioreq.c 

C_DEPS += \
./Core/Src/GPS.d \
./Core/Src/Potentiometer.d \
./Core/Src/diskio.d \
./Core/Src/displays.d \
./Core/Src/ff.d \
./Core/Src/ff_gen_drv.d \
./Core/Src/fonts.d \
./Core/Src/magnetic.d \
./Core/Src/main.d \
./Core/Src/st7735.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/usbd_conf.d \
./Core/Src/usbd_core.d \
./Core/Src/usbd_ctlreq.d \
./Core/Src/usbd_ioreq.d 

OBJS += \
./Core/Src/GPS.o \
./Core/Src/Potentiometer.o \
./Core/Src/diskio.o \
./Core/Src/displays.o \
./Core/Src/ff.o \
./Core/Src/ff_gen_drv.o \
./Core/Src/fonts.o \
./Core/Src/magnetic.o \
./Core/Src/main.o \
./Core/Src/st7735.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/usbd_conf.o \
./Core/Src/usbd_core.o \
./Core/Src/usbd_ctlreq.o \
./Core/Src/usbd_ioreq.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../FATFS/App -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../FATFS/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/GPS.d ./Core/Src/GPS.o ./Core/Src/GPS.su ./Core/Src/Potentiometer.d ./Core/Src/Potentiometer.o ./Core/Src/Potentiometer.su ./Core/Src/diskio.d ./Core/Src/diskio.o ./Core/Src/diskio.su ./Core/Src/displays.d ./Core/Src/displays.o ./Core/Src/displays.su ./Core/Src/ff.d ./Core/Src/ff.o ./Core/Src/ff.su ./Core/Src/ff_gen_drv.d ./Core/Src/ff_gen_drv.o ./Core/Src/ff_gen_drv.su ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/fonts.su ./Core/Src/magnetic.d ./Core/Src/magnetic.o ./Core/Src/magnetic.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/st7735.d ./Core/Src/st7735.o ./Core/Src/st7735.su ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/usbd_conf.d ./Core/Src/usbd_conf.o ./Core/Src/usbd_conf.su ./Core/Src/usbd_core.d ./Core/Src/usbd_core.o ./Core/Src/usbd_core.su ./Core/Src/usbd_ctlreq.d ./Core/Src/usbd_ctlreq.o ./Core/Src/usbd_ctlreq.su ./Core/Src/usbd_ioreq.d ./Core/Src/usbd_ioreq.o ./Core/Src/usbd_ioreq.su

.PHONY: clean-Core-2f-Src

