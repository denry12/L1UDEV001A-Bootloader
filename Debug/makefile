################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: L1UDEV001A-Bootloader.axf

# Tool invocations
L1UDEV001A-Bootloader.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\denry.LIEWENTHAL\Documents\LPCXpresso_8.1.2_603\workspace_11u35\CMSISv2p00_LPC11Uxx\Debug" -Xlinker -Map="L1UDEV001A-Bootloader.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "L1UDEV001A-Bootloader_Debug.ld" -o "L1UDEV001A-Bootloader.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) L1UDEV001A-Bootloader.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "L1UDEV001A-Bootloader.axf"; # arm-none-eabi-objcopy -v -O binary "L1UDEV001A-Bootloader.axf" "L1UDEV001A-Bootloader.bin" ; # checksum -p LPC11U35_401 -d "L1UDEV001A-Bootloader.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
