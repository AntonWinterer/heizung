################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/main.c" 

C_SRCS += \
../Sources/main.c 

OBJS += \
./Sources/main.obj 

OBJS_QUOTED += \
"./Sources/main.obj" 

C_DEPS += \
./Sources/main.d 

OBJS_OS_FORMAT += \
./Sources/main.obj 


# Each subdirectory must supply rules for building sources it contributes
Sources/main.obj: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: HCS08 Compiler'
	"C:\Program Files (x86)\Freescale\CW MCU v10.0\eclipse\../MCU/prog/chc08" -ArgFile"Sources/main.args" -ObjN="Sources/main.obj" "$<"
	"C:\Program Files (x86)\Freescale\CW MCU v10.0\eclipse\../MCU/prog/chc08" -ArgFile"Sources/main.args" -ObjN="Sources/main.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/%.d: ../Sources/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


