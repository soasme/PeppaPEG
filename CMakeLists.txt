cmake_minimum_required(VERSION 3.9)

project(peppapeg C)

set(P4_LIB peppapeg)
set(P4_HEADERS peppapeg.h)
set(P4_SOURCES peppapeg.c)
add_library("${P4_LIB}" SHARED "${P4_HEADERS}" "${P4_SOURCES}")
# If testing is enabled
target_compile_options("${P4_LIB}" PRIVATE "-g")
target_compile_options("${P4_LIB}" PRIVATE "-ansi")
target_compile_options("${P4_LIB}" PRIVATE "-Wall")
# target_compile_options("${P4_LIB}" PRIVATE "-fsanitize=address")

option(ENABLE_SANITIZERS "Enables AddressSanitizer and UndefinedBehaviorSanitizer." OFF)
if (ENABLE_SANITIZERS)
    list(APPEND custom_compiler_flags
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=float-cast-overflow
        -fsanitize-address-use-after-scope
        )
endif()

foreach(compiler_flag ${custom_compiler_flags})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flag}")
endforeach()

add_subdirectory(tests)
add_subdirectory(docs)
