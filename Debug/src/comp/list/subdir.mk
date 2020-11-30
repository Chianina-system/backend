################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/comp/list/edgelist.cpp \
../src/comp/list/liststomerge.cpp 

OBJS += \
./src/comp/list/edgelist.o \
./src/comp/list/liststomerge.o 

CPP_DEPS += \
./src/comp/list/edgelist.d \
./src/comp/list/liststomerge.d 


# Each subdirectory must supply rules for building sources it contributes
src/comp/list/%.o: ../src/comp/list/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


