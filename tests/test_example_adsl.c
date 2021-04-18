#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/adsl.h"

# define ASSERT_ASDL(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreateAsdlGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), P4_GetErrorMessage(source)); \
    P4_Token* root = P4_GetSourceAst(source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(grammar, f, root); \
    fclose(f); \
    P4_String s = read_file("check.json"); \
    TEST_ASSERT_EQUAL_STRING((output), s); \
    free(s); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_id (void) {
    ASSERT_ASDL(P4_AsdlRuleId, "mod", P4_Ok, "[{\"slice\":[0,3],\"type\":\"type_id\"}]");
    ASSERT_ASDL(P4_AsdlRuleId, "Statement", P4_Ok, "[{\"slice\":[0,9],\"type\":\"constructor_id\"}]");
}

void test_field(void) {
    ASSERT_ASDL(P4_AsdlRuleField, "int? end_lineno", P4_Ok, "["
        "{\"slice\":[0,15],\"type\":\"field\",\"children\":["
            "{\"slice\":[0,3],\"type\":\"type_id\"},"
            "{\"slice\":[3,4],\"type\":\"question\"},"
            "{\"slice\":[5,15],\"type\":\"type_id\"}"
        "]}"
    "]");
    ASSERT_ASDL(P4_AsdlRuleField, "int end_lineno", P4_Ok, "["
        "{\"slice\":[0,14],\"type\":\"field\",\"children\":["
            "{\"slice\":[0,3],\"type\":\"type_id\"},"
            "{\"slice\":[4,14],\"type\":\"type_id\"}"
        "]}"
    "]");
}

void test_fields(void) {
    ASSERT_ASDL(P4_AsdlRuleFields, "(int lineno, int? end_lineno)", P4_Ok, "["
        "{\"slice\":[0,29],\"type\":\"fields\",\"children\":["
            "{\"slice\":[1,11],\"type\":\"field\",\"children\":["
                "{\"slice\":[1,4],\"type\":\"type_id\"},"
                "{\"slice\":[5,11],\"type\":\"type_id\"}"
            "]},"
            "{\"slice\":[13,28],\"type\":\"field\",\"children\":["
                "{\"slice\":[13,16],\"type\":\"type_id\"},"
                "{\"slice\":[16,17],\"type\":\"question\"},"
                "{\"slice\":[18,28],\"type\":\"type_id\"}"
            "]}"
        "]}"
    "]");
}

void test_constructor(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_id);
    RUN_TEST(test_field);
    RUN_TEST(test_fields);
    RUN_TEST(test_constructor);

    return UNITY_END();
}
