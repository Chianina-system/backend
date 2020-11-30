################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/comp/array/arraystomerge.cpp 

OBJS += \
./src/comp/array/arraystomerge.o 

CPP_DEPS += \
./src/comp/array/arraystomerge.d 


# Each subdirectory must supply rules for building sources it contributes
src/comp/array/%.o: ../src/comp/array/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


