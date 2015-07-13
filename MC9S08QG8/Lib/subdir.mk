################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Lib/mc9s08qg8.c" 

C_SRCS += \
../Lib/mc9s08qg8.c 

OBJS += \
./Lib/mc9s08qg8.obj 

OBJS_QUOTED += \
"./Lib/mc9s08qg8.obj" 

C_DEPS += \
./Lib/mc9s08qg8.d 

OBJS_OS_FORMAT += \
./Lib/mc9s08qg8.obj 


# Each subdirectory must supply rules for building sources it contributes
Lib/mc9s08qg8.obj: ../Lib/mc9s08qg8.c
	@echo 'Building file: $<'
	@echo 'Invoking: HCS08 Compiler'
	"C:\Program Files (x86)\Freescale\CW MCU v10.0\eclipse\../MCU/prog/chc08" -ArgFile"Lib/mc9s08qg8.args" -ObjN="Lib/mc9s08qg8.obj" "$<"
	"C:\Program Files (x86)\Freescale\CW MCU v10.0\eclipse\../MCU/prog/chc08" -ArgFile"Lib/mc9s08qg8.args" -ObjN="Lib/mc9s08qg8.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Lib/%.d: ../Lib/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


