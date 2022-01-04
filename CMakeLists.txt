cmake_minimum_required(VERSION 3.9)

project(peppa C)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 16)
set(PROJECT_VERSION_PATCH 0)
set(P4_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(GNUInstallDirs)

set(P4_LIB peppa)
set(P4_BIN cli)
set(P4_HEADERS peppa.h)
set(P4_SOURCES peppa.c)
add_library("${P4_LIB}" SHARED "${P4_HEADERS}" "${P4_SOURCES}")

option(ENABLE_CLI "Enable building command-line interface." On)
if (ENABLE_CLI)
    add_executable("${P4_BIN}" "shell.c")
    target_link_libraries("${P4_BIN}" "${P4_LIB}")
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${P4_BIN}"
        DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE
                    WORLD_READ WORLD_EXECUTE
        RENAME "peppa"
    )
endif()

target_compile_options("${P4_LIB}" PRIVATE "-ansi")
target_compile_options("${P4_LIB}" PRIVATE "-Wall")
target_compile_options("${P4_LIB}" PRIVATE "-Werror")
target_compile_options("${P4_LIB}" PRIVATE "-Wextra")

option(ENABLE_UNISTR "Enables libunistring." OFF)
message("-- Optional Dependency: libunistring - ${ENABLE_UNISTR}.")
if (ENABLE_UNISTR)
    find_library(UNISTRING_LIB unistring REQUIRED)
    find_path(UNISTR_H NAMES unistr.h)
    target_compile_options("${P4_LIB}" PRIVATE "-I${UNISTR_H}")
    target_compile_options("${P4_LIB}" PRIVATE "-D ENABLE_UNISTR")
    target_link_libraries("${P4_LIB}" PUBLIC "${UNISTRING_LIB}")
    target_compile_options("${P4_BIN}" PRIVATE "-I${UNISTR_H}")
    target_compile_options("${P4_BIN}" PRIVATE "-D ENABLE_UNISTR")
endif()

option(ENABLE_DEBUG "Enables CC -g." OFF)
message("-- CC: Debug - ${ENABLE_DEBUG}.")
if (ENABLE_DEBUG)
    target_compile_options("${P4_LIB}" PRIVATE "-g")
    target_compile_options("${P4_BIN}" PRIVATE "-g")
endif()

option(ENABLE_OPTIMIZATION "Enable Optimization." Off)
message("-- CC: Optimization - ${ENABLE_OPTIMIZATION}.")
if (ENABLE_OPTIMIZATION)
    target_compile_options("${P4_LIB}" PRIVATE "-O2")
    target_compile_options("${P4_BIN}" PRIVATE "-O2")
endif()

option(ENABLE_ADDRESS_SANITIZER "Enables AddressSanitizer." OFF)
message("-- Clang: AddressSanitizer - ${ENABLE_ADDRESS_SANITIZER}.")
if (ENABLE_ADDRESS_SANITIZER)
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

option(ENABLE_CHECK "Enable unit test." OFF)
if (ENABLE_CHECK)
    add_subdirectory(tests)
endif()

option(ENABLE_DOCS "Enable documentation build." OFF)
if (ENABLE_DOCS)
    add_subdirectory(docs)
endif()
