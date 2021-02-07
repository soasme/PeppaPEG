#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

P4_PRIVATE(void) test_match_simple_literal(void) {
# define SIMPLE_LITERAL 1

    P4_Grammar* grammar = P4_CreateGrammar();

    TEST_ASSERT_NOT_NULL(grammar);

    P4_AddLiteral(grammar, SIMPLE_LITERAL, "Hello World", false);

    TEST_ASSERT_EQUAL_UINT(1, grammar->count);
    TEST_ASSERT_NOT_NULL(grammar->rules);

    // case 1: same literal "Hello World".
    P4_Source*  source = P4_CreateSource("Hello World", SIMPLE_LITERAL);

    TEST_ASSERT_EQUAL_UINT(P4_Ok, P4_Parse(grammar, source));

    P4_Token*   token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);

    P4_Slice*   slice = P4_GetTokenSlice(token);

    TEST_ASSERT_EQUAL_SLICE(slice, 0, 11);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_simple_literal);

    return UNITY_END();
}

