################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../challenge.c \
../ses_adc.c \
../ses_button.c \
../ses_led.c \
../ses_motorFrequency.c \
../ses_pwm.c \
../ses_rotary.c \
../ses_scheduler.c \
../ses_timer.c 

OBJS += \
./challenge.o \
./ses_adc.o \
./ses_button.o \
./ses_led.o \
./ses_motorFrequency.o \
./ses_pwm.o \
./ses_rotary.o \
./ses_scheduler.o \
./ses_timer.o 

C_DEPS += \
./challenge.d \
./ses_adc.d \
./ses_button.d \
./ses_led.d \
./ses_motorFrequency.d \
./ses_pwm.d \
./ses_rotary.d \
./ses_scheduler.d \
./ses_timer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O1 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128rfa1 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


