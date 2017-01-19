################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/L1UDEV001A-Bootloader.c \
../src/cr_startup_lpc11uxx.c \
../src/l11uxx_internal_flash_lib.c 

OBJS += \
./src/L1UDEV001A-Bootloader.o \
./src/cr_startup_lpc11uxx.o \
./src/l11uxx_internal_flash_lib.o 

C_DEPS += \
./src/L1UDEV001A-Bootloader.d \
./src/cr_startup_lpc11uxx.d \
./src/l11uxx_internal_flash_lib.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -I"C:\Users\denry.LIEWENTHAL\Documents\LPCXpresso_8.1.2_603\workspace_11u35\CMSISv2p00_LPC11Uxx\inc" -I"C:\Users\denry.LIEWENTHAL\Documents\LPCXpresso_8.1.2_603\workspace_11u35\L1UDEV001A-Bootloader\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


