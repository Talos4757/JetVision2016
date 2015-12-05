################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/JetVision.cpp 

OBJS += \
./src/JetVision.o 

CPP_DEPS += \
./src/JetVision.d 


# Each subdirectory must supply rules for building sources it contributes
src/JetVision.o: ../src/JetVision.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/JetVision.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


