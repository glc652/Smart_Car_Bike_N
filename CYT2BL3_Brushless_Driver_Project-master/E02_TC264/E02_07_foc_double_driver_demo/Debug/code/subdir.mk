################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../code/imu.c" \
"../code/imu660rb.c" \
"../code/init.c" \
"../code/small_driver_uart_control.c" 

COMPILED_SRCS += \
"code/imu.src" \
"code/imu660rb.src" \
"code/init.src" \
"code/small_driver_uart_control.src" 

C_DEPS += \
"./code/imu.d" \
"./code/imu660rb.d" \
"./code/init.d" \
"./code/small_driver_uart_control.d" 

OBJS += \
"code/imu.o" \
"code/imu660rb.o" \
"code/init.o" \
"code/small_driver_uart_control.o" 


# Each subdirectory must supply rules for building sources it contributes
"code/imu.src":"../code/imu.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fE:/Smart_Car_2025/CYT2BL3_Brushless_Driver_Project-master/E02_TC264/E02_07_foc_double_driver_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/imu.o":"code/imu.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/imu660rb.src":"../code/imu660rb.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fE:/Smart_Car_2025/CYT2BL3_Brushless_Driver_Project-master/E02_TC264/E02_07_foc_double_driver_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/imu660rb.o":"code/imu660rb.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/init.src":"../code/init.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fE:/Smart_Car_2025/CYT2BL3_Brushless_Driver_Project-master/E02_TC264/E02_07_foc_double_driver_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/init.o":"code/init.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/small_driver_uart_control.src":"../code/small_driver_uart_control.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fE:/Smart_Car_2025/CYT2BL3_Brushless_Driver_Project-master/E02_TC264/E02_07_foc_double_driver_demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/small_driver_uart_control.o":"code/small_driver_uart_control.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/imu.d ./code/imu.o ./code/imu.src ./code/imu660rb.d ./code/imu660rb.o ./code/imu660rb.src ./code/init.d ./code/init.o ./code/init.src ./code/small_driver_uart_control.d ./code/small_driver_uart_control.o ./code/small_driver_uart_control.src

.PHONY: clean-code

