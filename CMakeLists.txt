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

add_subdirectory(tests)


# Docs
if (ENABLE_DOCS)
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
    set(DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/docs/.doxygen.conf)
    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX_FILE}
        MAIN_DEPENDENCY ${P4_HEADERS} ${P4_SOURCES} ${DOXYFILE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generate doxygen docs"
        VERBATIM
    )
    add_custom_target(
        doxygen ALL
        DEPENDS ${DOXYGEN_INDEX_FILE}
    )

    find_program(PYTHON python3)
    find_program(PYTHONVENV python3-venv)

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
        OUTPUT ${SPHINX_EXECUTABLE}
        DEPENDS venv docs/requirements.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/docs/requirements.txt docs-requirements.txt
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}/docs
        COMMAND ./venv/bin/python3 -mensurepip
        COMMAND ./venv/bin/python3 -mpip install -r docs-requirements.txt --upgrade
        MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python
    )
    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        COMMAND ${SPHINX_EXECUTABLE} -b html ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
            ${SPHINX_SOURCE}/index.rst
            ${SPHINX_SOURCE}/references.rst
            ${SPHINX_SOURCE}/getting_started.rst
            ${DOXYGEN_INDEX_FILE}
        COMMENT "Generating documentation with Sphinx"
    )
    add_custom_target(
        docs ALL
        DEPENDS ${SPHINX_EXECUTABLE} ${SPHINX_INDEX_FILE}
    )
endif()
