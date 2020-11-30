################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/utility/uintseqhash.cc 

CPP_SRCS += \
../src/utility/Logger.cpp \
../src/utility/ResourceManager.cpp \
../src/utility/Timer.cpp 

CC_DEPS += \
./src/utility/uintseqhash.d 

OBJS += \
./src/utility/Logger.o \
./src/utility/ResourceManager.o \
./src/utility/Timer.o \
./src/utility/uintseqhash.o 

CPP_DEPS += \
./src/utility/Logger.d \
./src/utility/ResourceManager.d \
./src/utility/Timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/utility/%.o: ../src/utility/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/utility/%.o: ../src/utility/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


