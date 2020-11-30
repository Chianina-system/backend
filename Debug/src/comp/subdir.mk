################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/comp/cfg_compute_syn.cpp \
../src/comp/computationset.cpp \
../src/comp/context.cpp \
../src/comp/edgearray.cpp \
../src/comp/minheap.cpp \
../src/comp/myalgorithm.cpp \
../src/comp/peg_compute.cpp \
../src/comp/peg_compute_parallel.cpp \
../src/comp/pegraph.cpp 

OBJS += \
./src/comp/cfg_compute_syn.o \
./src/comp/computationset.o \
./src/comp/context.o \
./src/comp/edgearray.o \
./src/comp/minheap.o \
./src/comp/myalgorithm.o \
./src/comp/peg_compute.o \
./src/comp/peg_compute_parallel.o \
./src/comp/pegraph.o 

CPP_DEPS += \
./src/comp/cfg_compute_syn.d \
./src/comp/computationset.d \
./src/comp/context.d \
./src/comp/edgearray.d \
./src/comp/minheap.d \
./src/comp/myalgorithm.d \
./src/comp/peg_compute.d \
./src/comp/peg_compute_parallel.d \
./src/comp/pegraph.d 


# Each subdirectory must supply rules for building sources it contributes
src/comp/%.o: ../src/comp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


