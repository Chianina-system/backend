################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/preproc/grammar.cpp \
../src/preproc/scc_tarjan.cpp 

OBJS += \
./src/preproc/grammar.o \
./src/preproc/scc_tarjan.o 

CPP_DEPS += \
./src/preproc/grammar.d \
./src/preproc/scc_tarjan.d 


# Each subdirectory must supply rules for building sources it contributes
src/preproc/%.o: ../src/preproc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


