enable_testing()

include(GNUInstallDirs)
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
    test_cut
    test_back_reference
    test_left_recursion
    test_misc
    test_peg
    test_example_mustache
    test_example_json
    test_example_calc
    test_example_dot
)

option(ENABLE_VALGRIND "Enable vagrind memory leak report." OFF)


if (ENABLE_VALGRIND)
    find_program(VALGRIND valgrind)
    message("-- Valgrind found: ${VALGRIND}")

    if (VALGRIND)
        set(VALGRIND_OPTIONS --trace-children=yes --leak-check=full --show-leak-kinds=all --error-exitcode=1)
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
    target_compile_options("${unity_testcase}" PRIVATE "-DDEBUG")

    if (ENABLE_UNISTR)
        target_compile_options("${unity_testcase}" PRIVATE "-I${UNISTR_H}")
        target_compile_options("${unity_testcase}" PRIVATE "-DENABLE_UNISTR")
    endif()

    if (VALGRIND)
        add_test(
            NAME "${unity_testcase}"
            COMMAND "${VALGRIND}" ${VALGRIND_OPTIONS} "${CMAKE_CURRENT_BINARY_DIR}/${unity_testcase}"
        )
    else()
        set(ENV{ASAN_OPTIONS} "detect_leaks=1")
        add_test(
            NAME "${unity_testcase}"
            COMMAND "./${unity_testcase}"
        )
    endif()

endforeach()

set(
    spec_testcases
    test_literal_spec
    test_range_spec
    test_range_spec+unistr
    test_insensitive_spec
    test_sequence_spec
    test_choice_spec
    test_left_recursion_spec
    test_lookahead_spec
    test_cut_spec
    test_repeat_spec
    test_reference_spec
    test_back_reference_spec
    test_dot_spec
    test_comment_spec
    test_flags_spec
    test_peppa_spec
    test_toml_v1_0_spec
    test_golang_v1_17_spec
)

find_program(PYTHON python3)
foreach(spec_testcase ${spec_testcases})
    if (VALGRIND)
        set(SPEC_BIN ${VALGRIND})
        string(APPEND SPEC_BIN ${VALGRIND_OPTIONS} )
        string(CONCAT SPEC_BIN "${CMAKE_BINARY_DIR}/${P4_BIN}")
    else()
        set(SPEC_BIN "${CMAKE_BINARY_DIR}/${P4_BIN}")
    endif()

    if (spec_testcase MATCHES "unistr" AND NOT ENABLE_UNISTR)
        continue()
    endif()

    add_test(
        NAME "${spec_testcase}"
        COMMAND "${PYTHON}"
            "${PROJECT_SOURCE_DIR}/scripts/check_spec.py"
            "${SPEC_BIN}"
            "${PROJECT_SOURCE_DIR}/tests/${spec_testcase}.json"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

add_custom_target(
    check
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/json_test_parsing ${CMAKE_CURRENT_BINARY_DIR}/json_test_parsing
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/json_test_parsing ${CMAKE_CURRENT_BINARY_DIR}/../json_test_parsing
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${unity_testcases}
    DEPENDS ${P4_BIN}
)
