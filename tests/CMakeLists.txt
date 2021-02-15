enable_testing()

add_library(unity STATIC unity/src/unity.c)

set(
    unity_testcases
    test_literal
    test_range
    test_sequence
    test_choice
    test_reference
    test_positive
    test_negative
    test_repeat
    test_flags
    test_back_reference
    test_example_mustache
    test_example_json
)

option(ENABLE_VALGRIND "Enable vagrind memory leak report." OFF)

if (ENABLE_VALGRIND)
    find_program(VALGRIND valgrind)
    message("-- Valgrind found: ${VALGRIND}")

    if (VALGRIND)
        set(VALGRIND_OPTIONS --trace-children=yes --leak-check=full --error-exitcode=1 -s)
    else()
        message(WARNING "Valgrind not installed.")
        unset(VALGRIND)
    endif()
endif()

foreach(unity_testcase ${unity_testcases})
    add_executable("${unity_testcase}" "${unity_testcase}.c" "unity_setup.c")
    target_link_libraries("${unity_testcase}" "${P4_LIB}" unity)
    target_compile_options("${unity_testcase}" PRIVATE "-g")
    target_compile_options("${unity_testcase}" PRIVATE "-ansi")

    if (VALGRIND)
        add_test(
            NAME "${unity_testcase}"
            COMMAND "${VALGRIND}" ${VALGRIND_OPTIONS} "${CMAKE_CURRENT_BINARY_DIR}/${unity_testcase}"
        )
    else()
        add_test(
            NAME "${unity_testcase}"
            COMMAND "./${unity_testcase}"
        )
    endif()

endforeach()

add_custom_target(
    check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${unity_testcases}
)
