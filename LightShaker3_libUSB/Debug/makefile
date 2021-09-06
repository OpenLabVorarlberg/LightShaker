################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB/src/subdir.mk
-include Startup/subdir.mk
-include Src/src/subdir.mk
-include Src/subdir.mk
-include Libraries/STM32_F0xx_StdPerip_Driver/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
LightShaker3_libUSB.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
LightShaker3_libUSB.list \

OBJCOPY_BIN += \
LightShaker3_libUSB.bin \


# All Target
all: LightShaker3_libUSB.elf secondary-outputs

# Tool invocations
LightShaker3_libUSB.elf: $(OBJS) $(USER_OBJS) C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\STM32F042F6PX_FLASH.ld
	arm-none-eabi-gcc -o "LightShaker3_libUSB.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\STM32F042F6PX_FLASH.ld" --specs=nosys.specs -Wl,-Map="LightShaker3_libUSB.map" -Wl,--gc-sections -static -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Libraries\CMSIS\Device\ST\STM32F0xx\Include" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Libraries\CMSIS\Include" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Libraries\CMSIS\RTOS" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Libraries\STM32_F0xx_StdPerip_Driver\inc" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Libraries\STM32_F0xx_StdPerip_Driver\src" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Src\src" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\Src" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\USB\inc" -L"C:\Users\SteDun00\Desktop\GitHub\LightShaker3_libUSB\USB\src" --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

LightShaker3_libUSB.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "LightShaker3_libUSB.list"
	@echo 'Finished building: $@'
	@echo ' '

LightShaker3_libUSB.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "LightShaker3_libUSB.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
