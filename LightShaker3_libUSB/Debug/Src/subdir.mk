################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/AppMgmt.c \
../Src/App_ColorPicker.c \
../Src/App_Level.c \
../Src/App_POVDisplay.c \
../Src/App_Playground.c \
../Src/configConsole.c \
../Src/main.c \
../Src/stm32_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32f0xx.c \
../Src/tiny_printf.c 

OBJS += \
./Src/AppMgmt.o \
./Src/App_ColorPicker.o \
./Src/App_Level.o \
./Src/App_POVDisplay.o \
./Src/App_Playground.o \
./Src/configConsole.o \
./Src/main.o \
./Src/stm32_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32f0xx.o \
./Src/tiny_printf.o 

C_DEPS += \
./Src/AppMgmt.d \
./Src/App_ColorPicker.d \
./Src/App_Level.d \
./Src/App_POVDisplay.d \
./Src/App_Playground.d \
./Src/configConsole.d \
./Src/main.d \
./Src/stm32_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32f0xx.d \
./Src/tiny_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/git/LightShaker/LightShaker3_libUSB/Src/src" -I"C:/git/LightShaker/LightShaker3_libUSB/Src" -I"C:/git/LightShaker/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

