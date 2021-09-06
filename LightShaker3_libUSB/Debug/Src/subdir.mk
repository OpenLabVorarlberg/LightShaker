################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/configConsole.c \
../Src/main.c \
../Src/stm32_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32f0xx.c \
../Src/tiny_printf.c 

OBJS += \
./Src/configConsole.o \
./Src/main.o \
./Src/stm32_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32f0xx.o \
./Src/tiny_printf.o 

C_DEPS += \
./Src/configConsole.d \
./Src/main.d \
./Src/stm32_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32f0xx.d \
./Src/tiny_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Src/configConsole.o: ../Src/configConsole.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/configConsole.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/main.o: ../Src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/stm32_it.o: ../Src/stm32_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32_it.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/syscalls.o: ../Src/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/syscalls.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/sysmem.o: ../Src/sysmem.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/sysmem.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/system_stm32f0xx.o: ../Src/system_stm32f0xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/system_stm32f0xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/tiny_printf.o: ../Src/tiny_printf.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/tiny_printf.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

