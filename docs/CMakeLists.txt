# Docs
if (ENABLE_DOCS)
    set(SITE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

    find_package(Doxygen REQUIRED)

    # Generate Doxyfile

    set(DOXYGEN_PROJECT_NAME "Peppa\ PEG")
    set(DOXYGEN_PROJECT_BRIEF "\"An\ Ultra\ Lightweight\ PEG\ Parser\ in\ ANSI\ C\"")
    set(DOXYGEN_INPUT ${PROJECT_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/)
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_HAVE_DOT NO)
    set(DOXYFILE_IN ${CMAKE_BINARY_DIR}/CMakeDoxyfile.in)
    set(DOXYFILE ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

    # Generate XML files using Doxygen

    set(DOXYGEN_INDEX_FILE ${SITE_OUTPUT_DIR}/xml/index.xml)

    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX_FILE}
        MAIN_DEPENDENCY ${P4_HEADERS} ${P4_SOURCES} ${DOXYFILE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generate doxygen docs"
        VERBATIM
    )
    add_custom_target(
        doxygen ALL
        DEPENDS ${DOXYFILE} ${DOXYGEN_INDEX_FILE}
    )

    # Generate docs using Sphinx

    find_program(PYTHON python3)

    set(VIRTUALENV ${PYTHON} "-mvenv")
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/)
    set(SPHINX_BUILD ${SITE_OUTPUT_DIR}/site/docs)
    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
    set(SPHINX_EXECUTABLE sphinx-build)

    add_custom_command(
        OUTPUT venv
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt docs-requirements.txt
        COMMAND ${VIRTUALENV} venv
        COMMAND ./venv/bin/python3 -mensurepip
        COMMAND ./venv/bin/python3 -mpip install -r docs-requirements.txt --upgrade
    )
    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        COMMAND ./venv/bin/${SPHINX_EXECUTABLE} -b html ${SPHINX_SOURCE} ${SPHINX_BUILD}
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
        DEPENDS ${DOXYGEN_INDEX_FILE} venv ${SPHINX_INDEX_FILE}
    )
endif()

