################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/src/drvApa102.c \
../Src/src/drvDisplay.c \
../Src/src/drvMMA8653.c \
../Src/src/drvNvMemory.c \
../Src/src/drvPower.c \
../Src/src/drvProgram.c \
../Src/src/drvSwitch.c \
../Src/src/eeprom.c \
../Src/src/utilFifo.c 

OBJS += \
./Src/src/drvApa102.o \
./Src/src/drvDisplay.o \
./Src/src/drvMMA8653.o \
./Src/src/drvNvMemory.o \
./Src/src/drvPower.o \
./Src/src/drvProgram.o \
./Src/src/drvSwitch.o \
./Src/src/eeprom.o \
./Src/src/utilFifo.o 

C_DEPS += \
./Src/src/drvApa102.d \
./Src/src/drvDisplay.d \
./Src/src/drvMMA8653.d \
./Src/src/drvNvMemory.d \
./Src/src/drvPower.d \
./Src/src/drvProgram.d \
./Src/src/drvSwitch.d \
./Src/src/eeprom.d \
./Src/src/utilFifo.d 


# Each subdirectory must supply rules for building sources it contributes
Src/src/drvApa102.o: ../Src/src/drvApa102.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvApa102.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvDisplay.o: ../Src/src/drvDisplay.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvDisplay.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvMMA8653.o: ../Src/src/drvMMA8653.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvMMA8653.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvNvMemory.o: ../Src/src/drvNvMemory.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvNvMemory.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvPower.o: ../Src/src/drvPower.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvPower.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvProgram.o: ../Src/src/drvProgram.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvProgram.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/drvSwitch.o: ../Src/src/drvSwitch.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/drvSwitch.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/eeprom.o: ../Src/src/eeprom.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/eeprom.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/src/utilFifo.o: ../Src/src/utilFifo.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/src/utilFifo.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

