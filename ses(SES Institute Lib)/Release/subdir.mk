################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ses_adc.c \
../ses_button.c \
../ses_led.c \
../ses_scheduler.c \
../ses_timer.c 

OBJS += \
./ses_adc.o \
./ses_button.o \
./ses_led.o \
./ses_scheduler.o \
./ses_timer.o 

C_DEPS += \
./ses_adc.d \
./ses_button.d \
./ses_led.d \
./ses_scheduler.d \
./ses_timer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128rfa1 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


