cmake_minimum_required(VERSION 3.10)

project(peppapeg C)

set(P4_LIB peppapeg)
set(P4_HEADERS peppapeg.h)
set(P4_SOURCES peppapeg.c)
add_library("${P4_LIB}" SHARED "${P4_HEADERS}" "${P4_SOURCES}")
# If testing is enabled
target_compile_options("${P4_LIB}" PRIVATE "-g")
# target_compile_options("${P4_LIB}" PRIVATE "-fsanitize=address")

add_subdirectory(tests)


# Docs

find_program(PYTHON python3)
set(VIRTUALENV ${PYTHON} "-mvenv")
add_custom_command(
    OUTPUT venv
    COMMAND ${VIRTUALENV} venv
)
add_custom_command(
    OUTPUT venv.stamp
    DEPENDS venv docs/requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/docs/requirements.txt docs-requirements.txt
    COMMAND ./venv/bin/pip install -r docs-requirements.txt --upgrade
)
add_custom_target(
    docs
    DEPENDS venv.stamp
)
