################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/DS3231.c \
../Lib/lcd_i2c.c 

OBJS += \
./Lib/DS3231.o \
./Lib/lcd_i2c.o 

C_DEPS += \
./Lib/DS3231.d \
./Lib/lcd_i2c.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/%.o Lib/%.su Lib/%.cyclo: ../Lib/%.c Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/ADMIN/Downloads/Compressed/STM32-master/STM32-master/DS3231/Lib" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Lib

clean-Lib:
	-$(RM) ./Lib/DS3231.cyclo ./Lib/DS3231.d ./Lib/DS3231.o ./Lib/DS3231.su ./Lib/lcd_i2c.cyclo ./Lib/lcd_i2c.d ./Lib/lcd_i2c.o ./Lib/lcd_i2c.su

.PHONY: clean-Lib

