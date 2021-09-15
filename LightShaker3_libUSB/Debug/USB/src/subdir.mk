################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB/src/usbd_core.c \
../USB/src/usbd_stm32f042f6p6_devfs.c \
../USB/src/winusb_loop.c 

OBJS += \
./USB/src/usbd_core.o \
./USB/src/usbd_stm32f042f6p6_devfs.o \
./USB/src/winusb_loop.o 

C_DEPS += \
./USB/src/usbd_core.d \
./USB/src/usbd_stm32f042f6p6_devfs.d \
./USB/src/winusb_loop.d 


# Each subdirectory must supply rules for building sources it contributes
USB/src/%.o: ../USB/src/%.c USB/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/git/LightShaker/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/git/LightShaker/LightShaker3_libUSB/Src/src" -I"C:/git/LightShaker/LightShaker3_libUSB/Src" -I"C:/git/LightShaker/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

