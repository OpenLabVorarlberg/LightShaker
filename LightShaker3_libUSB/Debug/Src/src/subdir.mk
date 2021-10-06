################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/src/drvAccelerometer.c \
../Src/src/drvDisplay.c \
../Src/src/drvNeopixels.c \
../Src/src/drvNvMemory.c \
../Src/src/drvPower.c \
../Src/src/eeprom.c \
../Src/src/utilFifo.c 

OBJS += \
./Src/src/drvAccelerometer.o \
./Src/src/drvDisplay.o \
./Src/src/drvNeopixels.o \
./Src/src/drvNvMemory.o \
./Src/src/drvPower.o \
./Src/src/eeprom.o \
./Src/src/utilFifo.o 

C_DEPS += \
./Src/src/drvAccelerometer.d \
./Src/src/drvDisplay.d \
./Src/src/drvNeopixels.d \
./Src/src/drvNvMemory.d \
./Src/src/drvPower.d \
./Src/src/eeprom.d \
./Src/src/utilFifo.d 


# Each subdirectory must supply rules for building sources it contributes
Src/src/%.o: ../Src/src/%.c Src/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/git/LightShaker/LightShaker3_libUSB/Src/src" -I"C:/git/LightShaker/LightShaker3_libUSB/Src" -I"C:/git/LightShaker/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

