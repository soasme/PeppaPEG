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
    test_precedence
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
    test_literal_spec.json
    test_range_spec.json
    test_range_spec+unistr.json
    test_insensitive_spec.json
    test_sequence_spec.json
    test_choice_spec.json
    test_left_recursion_spec.json
    test_lookahead_spec.json
    test_cut_spec.json
    test_repeat_spec.json
    test_reference_spec.json
    test_back_reference_spec.yaml
    test_dot_spec.json
    test_comment_spec.json
    test_flags_spec.json
    test_peppa_spec.json
    test_golang_v1_17_spec.json
    hcl2/test_expression_heredoc+unistr.yaml
    hcl2/test_expression_operator+unistr.yaml
    hcl2/test_expression_primitive+unistr.yaml
    hcl2/test_structure_attribute+unistr.yaml
    hcl2/test_structure_block+unistr.yaml
    toml/test_array.yaml
    toml/test_bool.yaml
    toml/test_comment.yaml
    toml/test_control.yaml
    toml/test_datetime.yaml
    toml/test_float.yaml
    toml/test_inline_table.yaml
    toml/test_integer.yaml
    toml/test_key.yaml
    toml/test_string.yaml
    toml/test_table.yaml
    abnf/test_rfc5234.yaml
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
            "${PROJECT_SOURCE_DIR}/tests/${spec_testcase}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

set(
    realcode_testcases
    "${CMAKE_SOURCE_DIR}/configs/lua-v5.4.peg" Chunk "lua-5.4.3" lua
    "${CMAKE_SOURCE_DIR}/configs/abnf.peg" RuleList "abnf" abnf
)
while(realcode_testcases)
    list(POP_FRONT realcode_testcases GRAMMAR_FILE GRAMMAR_ENTRY TEST_DIR TEST_SUFFIX)
    add_test(
        NAME "${TEST_DIR}/*.${TEST_SUFFIX}"
        COMMAND "${P4_BIN}" "parse"
            "--grammar-file" "${GRAMMAR_FILE}"
            "--grammar-entry" "${GRAMMAR_ENTRY}"
            "--quiet"
            "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_DIR}/*.${TEST_SUFFIX}")
endwhile()

add_custom_target(
    check
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/json_test_parsing ${CMAKE_CURRENT_BINARY_DIR}/json_test_parsing
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/json_test_parsing ${CMAKE_CURRENT_BINARY_DIR}/../json_test_parsing
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${unity_testcases}
    DEPENDS ${P4_BIN}
)
