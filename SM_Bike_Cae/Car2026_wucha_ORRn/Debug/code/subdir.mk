################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../code/Agorithm.c" \
"../code/Car_control.c" \
"../code/GPS.c" \
"../code/Imu.c" \
"../code/Imu_Enhanced.c" \
"../code/Ins.c" \
"../code/Kemu1.c" \
"../code/Kemu2.c" \
"../code/Kemu3.c" \
"../code/Key.c" \
"../code/LED.c" \
"../code/Menu.c" \
"../code/Menu_Integration.c" \
"../code/Menu_PID_Tuning.c" \
"../code/Motor.c" \
"../code/PID.c" \
"../code/PID_Flash_Storage.c" \
"../code/Servo.c" \
"../code/battery.c" \
"../code/encode.c" \
"../code/encoder_fusion.c" \
"../code/flash.c" \
"../code/imu_fusion_enhanced.c" \
"../code/navigation.c" \
"../code/ps2.c" \
"../code/ps2_control.c" \
"../code/ps2_test.c" \
"../code/small_driver_uart_control.c" \
"../code/vofa.c" 

COMPILED_SRCS += \
"code/Agorithm.src" \
"code/Car_control.src" \
"code/GPS.src" \
"code/Imu.src" \
"code/Imu_Enhanced.src" \
"code/Ins.src" \
"code/Kemu1.src" \
"code/Kemu2.src" \
"code/Kemu3.src" \
"code/Key.src" \
"code/LED.src" \
"code/Menu.src" \
"code/Menu_Integration.src" \
"code/Menu_PID_Tuning.src" \
"code/Motor.src" \
"code/PID.src" \
"code/PID_Flash_Storage.src" \
"code/Servo.src" \
"code/battery.src" \
"code/encode.src" \
"code/encoder_fusion.src" \
"code/flash.src" \
"code/imu_fusion_enhanced.src" \
"code/navigation.src" \
"code/ps2.src" \
"code/ps2_control.src" \
"code/ps2_test.src" \
"code/small_driver_uart_control.src" \
"code/vofa.src" 

C_DEPS += \
"./code/Agorithm.d" \
"./code/Car_control.d" \
"./code/GPS.d" \
"./code/Imu.d" \
"./code/Imu_Enhanced.d" \
"./code/Ins.d" \
"./code/Kemu1.d" \
"./code/Kemu2.d" \
"./code/Kemu3.d" \
"./code/Key.d" \
"./code/LED.d" \
"./code/Menu.d" \
"./code/Menu_Integration.d" \
"./code/Menu_PID_Tuning.d" \
"./code/Motor.d" \
"./code/PID.d" \
"./code/PID_Flash_Storage.d" \
"./code/Servo.d" \
"./code/battery.d" \
"./code/encode.d" \
"./code/encoder_fusion.d" \
"./code/flash.d" \
"./code/imu_fusion_enhanced.d" \
"./code/navigation.d" \
"./code/ps2.d" \
"./code/ps2_control.d" \
"./code/ps2_test.d" \
"./code/small_driver_uart_control.d" \
"./code/vofa.d" 

OBJS += \
"code/Agorithm.o" \
"code/Car_control.o" \
"code/GPS.o" \
"code/Imu.o" \
"code/Imu_Enhanced.o" \
"code/Ins.o" \
"code/Kemu1.o" \
"code/Kemu2.o" \
"code/Kemu3.o" \
"code/Key.o" \
"code/LED.o" \
"code/Menu.o" \
"code/Menu_Integration.o" \
"code/Menu_PID_Tuning.o" \
"code/Motor.o" \
"code/PID.o" \
"code/PID_Flash_Storage.o" \
"code/Servo.o" \
"code/battery.o" \
"code/encode.o" \
"code/encoder_fusion.o" \
"code/flash.o" \
"code/imu_fusion_enhanced.o" \
"code/navigation.o" \
"code/ps2.o" \
"code/ps2_control.o" \
"code/ps2_test.o" \
"code/small_driver_uart_control.o" \
"code/vofa.o" 


# Each subdirectory must supply rules for building sources it contributes
"code/Agorithm.src":"../code/Agorithm.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Agorithm.o":"code/Agorithm.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Car_control.src":"../code/Car_control.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Car_control.o":"code/Car_control.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/GPS.src":"../code/GPS.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/GPS.o":"code/GPS.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Imu.src":"../code/Imu.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Imu.o":"code/Imu.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Imu_Enhanced.src":"../code/Imu_Enhanced.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Imu_Enhanced.o":"code/Imu_Enhanced.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Ins.src":"../code/Ins.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Ins.o":"code/Ins.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Kemu1.src":"../code/Kemu1.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Kemu1.o":"code/Kemu1.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Kemu2.src":"../code/Kemu2.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Kemu2.o":"code/Kemu2.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Kemu3.src":"../code/Kemu3.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Kemu3.o":"code/Kemu3.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Key.src":"../code/Key.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Key.o":"code/Key.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/LED.src":"../code/LED.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/LED.o":"code/LED.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Menu.src":"../code/Menu.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Menu.o":"code/Menu.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Menu_Integration.src":"../code/Menu_Integration.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Menu_Integration.o":"code/Menu_Integration.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Menu_PID_Tuning.src":"../code/Menu_PID_Tuning.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Menu_PID_Tuning.o":"code/Menu_PID_Tuning.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Motor.src":"../code/Motor.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Motor.o":"code/Motor.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PID.src":"../code/PID.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/PID.o":"code/PID.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PID_Flash_Storage.src":"../code/PID_Flash_Storage.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/PID_Flash_Storage.o":"code/PID_Flash_Storage.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Servo.src":"../code/Servo.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/Servo.o":"code/Servo.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/battery.src":"../code/battery.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/battery.o":"code/battery.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/encode.src":"../code/encode.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/encode.o":"code/encode.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/encoder_fusion.src":"../code/encoder_fusion.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/encoder_fusion.o":"code/encoder_fusion.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/flash.src":"../code/flash.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/flash.o":"code/flash.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/imu_fusion_enhanced.src":"../code/imu_fusion_enhanced.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/imu_fusion_enhanced.o":"code/imu_fusion_enhanced.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/navigation.src":"../code/navigation.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/navigation.o":"code/navigation.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/ps2.src":"../code/ps2.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/ps2.o":"code/ps2.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/ps2_control.src":"../code/ps2_control.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/ps2_control.o":"code/ps2_control.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/ps2_test.src":"../code/ps2_test.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/ps2_test.o":"code/ps2_test.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/small_driver_uart_control.src":"../code/small_driver_uart_control.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/small_driver_uart_control.o":"code/small_driver_uart_control.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/vofa.src":"../code/vofa.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026_wucha_ORRn/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/vofa.o":"code/vofa.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/Agorithm.d ./code/Agorithm.o ./code/Agorithm.src ./code/Car_control.d ./code/Car_control.o ./code/Car_control.src ./code/GPS.d ./code/GPS.o ./code/GPS.src ./code/Imu.d ./code/Imu.o ./code/Imu.src ./code/Imu_Enhanced.d ./code/Imu_Enhanced.o ./code/Imu_Enhanced.src ./code/Ins.d ./code/Ins.o ./code/Ins.src ./code/Kemu1.d ./code/Kemu1.o ./code/Kemu1.src ./code/Kemu2.d ./code/Kemu2.o ./code/Kemu2.src ./code/Kemu3.d ./code/Kemu3.o ./code/Kemu3.src ./code/Key.d ./code/Key.o ./code/Key.src ./code/LED.d ./code/LED.o ./code/LED.src ./code/Menu.d ./code/Menu.o ./code/Menu.src ./code/Menu_Integration.d ./code/Menu_Integration.o ./code/Menu_Integration.src ./code/Menu_PID_Tuning.d ./code/Menu_PID_Tuning.o ./code/Menu_PID_Tuning.src ./code/Motor.d ./code/Motor.o ./code/Motor.src ./code/PID.d ./code/PID.o ./code/PID.src ./code/PID_Flash_Storage.d ./code/PID_Flash_Storage.o ./code/PID_Flash_Storage.src ./code/Servo.d ./code/Servo.o ./code/Servo.src ./code/battery.d ./code/battery.o ./code/battery.src ./code/encode.d ./code/encode.o ./code/encode.src ./code/encoder_fusion.d ./code/encoder_fusion.o ./code/encoder_fusion.src ./code/flash.d ./code/flash.o ./code/flash.src ./code/imu_fusion_enhanced.d ./code/imu_fusion_enhanced.o ./code/imu_fusion_enhanced.src ./code/navigation.d ./code/navigation.o ./code/navigation.src ./code/ps2.d ./code/ps2.o ./code/ps2.src ./code/ps2_control.d ./code/ps2_control.o ./code/ps2_control.src ./code/ps2_test.d ./code/ps2_test.o ./code/ps2_test.src ./code/small_driver_uart_control.d ./code/small_driver_uart_control.o ./code/small_driver_uart_control.src ./code/vofa.d ./code/vofa.o ./code/vofa.src

.PHONY: clean-code

