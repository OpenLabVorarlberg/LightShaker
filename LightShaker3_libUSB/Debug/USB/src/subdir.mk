################################################################################
# Automatically-generated file. Do not edit!
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
USB/src/usbd_core.o: ../USB/src/usbd_core.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB/src/usbd_core.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
USB/src/usbd_stm32f042f6p6_devfs.o: ../USB/src/usbd_stm32f042f6p6_devfs.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB/src/usbd_stm32f042f6p6_devfs.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
USB/src/winusb_loop.o: ../USB/src/winusb_loop.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -g3 -DUSE_HAL_DRIVER -DSTM32F042x6 -DDEBUG -c -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/Include" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/CMSIS/RTOS" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Libraries/STM32_F0xx_StdPerip_Driver/inc" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src/src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/Src" -I"C:/Users/SteDun00/Desktop/GitHub/LightShaker3_libUSB/USB/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB/src/winusb_loop.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

