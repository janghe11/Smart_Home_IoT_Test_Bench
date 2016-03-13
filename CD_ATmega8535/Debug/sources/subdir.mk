################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sources/get_rs232.c \
../sources/main.c \
../sources/set_doorlock.c 

OBJS += \
./sources/get_rs232.o \
./sources/main.o \
./sources/set_doorlock.o 

C_DEPS += \
./sources/get_rs232.d \
./sources/main.d \
./sources/set_doorlock.d 


# Each subdirectory must supply rules for building sources it contributes
sources/%.o: ../sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega8535 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


