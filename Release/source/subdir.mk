################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Practica_3.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/Practica_3.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/Practica_3.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\utilities" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\drivers" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\CMSIS" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\utilities" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\drivers" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\CMSIS" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\board" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\source" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_3\startup" -Os -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


