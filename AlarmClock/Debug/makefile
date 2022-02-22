################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
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
LSS += \
task_6_3.lss \

FLASH_IMAGE += \
task_6_3.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: task_6_3.elf secondary-outputs

dependents:
	-cd C:/Users/junai/Documents/Workspaces/Eclipse_C_Cpp/ses/Debug && $(MAKE) all

# Tool invocations
task_6_3.elf: $(OBJS) $(USER_OBJS) C:/Users/junai/Documents/Workspaces/Eclipse_C_Cpp/ses/Debug/libses.a
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,task_6_3.map -L"C:\Users\junai\Documents\Workspaces\Eclipse_C_Cpp\ses" -L"C:\Users\junai\Documents\Workspaces\Eclipse_C_Cpp\ses\Debug" -mmcu=atmega128rfa1 -o "task_6_3.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

task_6_3.lss: task_6_3.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S task_6_3.elf  >"task_6_3.lss"
	@echo 'Finished building: $@'
	@echo ' '

task_6_3.hex: task_6_3.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex task_6_3.elf  "task_6_3.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: task_6_3.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128rfa1 task_6_3.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) task_6_3.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
C:/Users/junai/Documents/Workspaces/Eclipse_C_Cpp/ses/Debug/libses.a:

-include ../makefile.targets
