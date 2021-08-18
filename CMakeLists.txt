cmake_minimum_required(VERSION 3.9)

project(peppa C)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 7)
set(PROJECT_VERSION_PATCH 14)
set(P4_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")


include(GNUInstallDirs)

set(P4_LIB peppa)
set(P4_HEADERS peppa.h)
set(P4_SOURCES peppa.c)
add_library("${P4_LIB}" SHARED "${P4_HEADERS}" "${P4_SOURCES}")

target_compile_options("${P4_LIB}" PRIVATE "-ansi")
target_compile_options("${P4_LIB}" PRIVATE "-Wall")

option(-DENABLE_UNISTR "Enables libunistring." OFF)
message("-- Optional Dependency: libunistring - ${ENABLE_UNISTR}.")
if (ENABLE_UNISTR)
    find_library(UNISTRING_LIB unistring REQUIRED)
    find_path(UNISTR_H NAMES unistr.h)
    target_compile_options("${P4_LIB}" PRIVATE "-I${UNISTR_H}")
    target_compile_options("${P4_LIB}" PRIVATE "-D ENABLE_UNISTR")
    target_link_libraries("${P4_LIB}" PUBLIC "${UNISTRING_LIB}")
endif()

option(ENABLE_DEBUG "Enables CC -g." OFF)
if (ENABLE_DEBUG)
    target_compile_options("${P4_LIB}" PRIVATE "-g")
endif()

option(-DENABLE_ADDRESS_SANITIZER "Enables AddressSanitizer." OFF)
message("-- Clang: AddressSanitizer - ${ENABLE_ADDRESS_SANITIZER}.")
if (ENABLE_SANITIZERS)
    list(APPEND custom_compiler_flags
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=float-cast-overflow
        -fsanitize-address-use-after-scope
    )
endif()

option(ENABLE_MEMORY_SANITIZER "Enable Clang memory sanitizer for nvim binary." OFF)
message("-- Clang: MemorySanitizer - ${ENABLE_MEMORY_SANITIZER}.")
if (ENABLE_MEMORY_SANITIZER)
    message("-- Clang: Enabled MemorySanitizer.")
    list(APPEND custom_compiler_flags
        -fsanitize=memory
        -fsanitize-memory-track-origins
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
    )
endif()

foreach(compiler_flag ${custom_compiler_flags})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flag}")
endforeach()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configs/libpeppa.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libpeppa.pc"
    @ONLY
)
install(
    FILES peppa.h
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/libpeppa.pc"
    DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig"
)
install(
    TARGETS "${P4_LIB}"
    EXPORT "${P4_LIB}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
)

add_subdirectory(tests)
add_subdirectory(docs)
