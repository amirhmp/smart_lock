# cmake_minimum_required(VERSION <specify CMake version here>)
project(Code C)

set(CMAKE_C_STANDARD 11)

include_directories(../INC)
include_directories(libs/at_command_interpreter)
include_directories(libs/at_command_interpreter/command_queue)
include_directories(libs/at_command_interpreter/string_buffer)
include_directories(libs/at_command_interpreter/utils)
include_directories(libs/init)
include_directories(libs/wait)
include_directories(libs/button)
include_directories(libs/menu_builder)

add_executable(Code
        libs/at_command_interpreter/command_queue/command_queue.c
        libs/at_command_interpreter/command_queue/command_queue.h
        libs/at_command_interpreter/string_buffer/string_buffer.c
        libs/at_command_interpreter/string_buffer/string_buffer.h
        libs/at_command_interpreter/utils/at_utils.c
        libs/at_command_interpreter/utils/at_utils.h
        libs/at_command_interpreter/at_command_interpreter.c
        libs/at_command_interpreter/at_command_interpreter.h
        libs/init/init.c
        libs/init/init.h
        libs/wait/wait.c
        libs/wait/wait.h
        libs/button/button.c
        libs/button/button.h
        libs/menu_builder/menus.h
        libs/menu_builder/menu_builder.c
        libs/menu_builder/menu_builder.h
        libs/menu_builder/static_stack.c
        libs/menu_builder/static_stack.h
        smart_lock.c libs/keypad/keypad.c libs/keypad/keypad.h libs/utils/utils.h libs/utils/utils.c smart_lock.h libs/mq_sensor/mq.c libs/mq_sensor/mq.h)

