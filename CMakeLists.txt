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
find_package(Doxygen REQUIRED)
set(VIRTUALENV ${PYTHON} "-mvenv")
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
set(SPHINX_EXECUTABLE "./venv/bin/sphinx-build")
add_custom_command(
    OUTPUT venv
    COMMAND ${VIRTUALENV} venv
)
add_custom_command(
    OUTPUT venv.stamp
    DEPENDS venv docs/requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/docs/requirements.txt docs-requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}docs
    COMMAND ./venv/bin/pip install -r docs-requirements.txt --upgrade
)
add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    COMMAND ${SPHINX_EXECUTABLE} -b html ${SPHINX_SOURCE} ${SPHINX_BUILD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py ${SPHINX_SOURCE}/index.rst 
    COMMENT "Generating documentation with Sphinx"
)
add_custom_target(
    docs
    DEPENDS venv.stamp ${SPHINX_INDEX_FILE}
)
