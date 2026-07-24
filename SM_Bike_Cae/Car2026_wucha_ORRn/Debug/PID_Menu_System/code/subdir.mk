################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.c" \
"../PID_Menu_System/code/PID_Menu.c" \
"../PID_Menu_System/code/main_with_pid_menu_example.c" 

COMPILED_SRCS += \
"PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.src" \
"PID_Menu_System/code/PID_Menu.src" \
"PID_Menu_System/code/main_with_pid_menu_example.src" 

C_DEPS += \
"./PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.d" \
"./PID_Menu_System/code/PID_Menu.d" \
"./PID_Menu_System/code/main_with_pid_menu_example.d" 

OBJS += \
"PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.o" \
"PID_Menu_System/code/PID_Menu.o" \
"PID_Menu_System/code/main_with_pid_menu_example.o" 


# Each subdirectory must supply rules for building sources it contributes
"PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.src":"../PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.c" "PID_Menu_System/code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.o":"PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.src" "PID_Menu_System/code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"PID_Menu_System/code/PID_Menu.src":"../PID_Menu_System/code/PID_Menu.c" "PID_Menu_System/code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"PID_Menu_System/code/PID_Menu.o":"PID_Menu_System/code/PID_Menu.src" "PID_Menu_System/code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"PID_Menu_System/code/main_with_pid_menu_example.src":"../PID_Menu_System/code/main_with_pid_menu_example.c" "PID_Menu_System/code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/ADS/workSpace/Car2026/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"PID_Menu_System/code/main_with_pid_menu_example.o":"PID_Menu_System/code/main_with_pid_menu_example.src" "PID_Menu_System/code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-PID_Menu_System-2f-code

clean-PID_Menu_System-2f-code:
	-$(RM) ./PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.d ./PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.o ./PID_Menu_System/code/Menu_PID_Tuning_With_Flash_Example.src ./PID_Menu_System/code/PID_Menu.d ./PID_Menu_System/code/PID_Menu.o ./PID_Menu_System/code/PID_Menu.src ./PID_Menu_System/code/main_with_pid_menu_example.d ./PID_Menu_System/code/main_with_pid_menu_example.o ./PID_Menu_System/code/main_with_pid_menu_example.src

.PHONY: clean-PID_Menu_System-2f-code

