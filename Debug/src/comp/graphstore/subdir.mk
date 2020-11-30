################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/comp/graphstore/fptree.cpp 

OBJS += \
./src/comp/graphstore/fptree.o 

CPP_DEPS += \
./src/comp/graphstore/fptree.d 


# Each subdirectory must supply rules for building sources it contributes
src/comp/graphstore/%.o: ../src/comp/graphstore/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


