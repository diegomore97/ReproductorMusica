################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_debug_console.c 

OBJS += \
./utilities/fsl_debug_console.o 

C_DEPS += \
./utilities/fsl_debug_console.d 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\utilities" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\drivers" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\CMSIS" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\utilities" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\drivers" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\CMSIS" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\board" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\source" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5" -I"C:\Users\Diego Moreno\Documents\MCUXpressoIDE_11.1.0_3209\workspace\Practica_5\startup" -Os -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


