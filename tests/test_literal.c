#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

P4_PRIVATE(void) test_match_simple_literal(void) {
# define SIMPLE_LITERAL 1

    P4_Grammar* grammar = P4_CreateGrammar();

    P4_AddGrammarRule(grammar, SIMPLE_LITERAL, P4_CreateLiteral("HelloWorld", false));

    TEST_ASSERT_EQUAL_UINT(1, grammar->count);
    TEST_ASSERT_NOT_NULL(grammar->rules);

    P4_Source*  source = P4_CreateSource("HelloWorld", SIMPLE_LITERAL);

    TEST_ASSERT_EQUAL_UINT(P4_Ok, P4_Parse(grammar, source));

    P4_Token*   token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);

    TEST_ASSERT_EQUAL_UINT(0,       token->slice.i);
    TEST_ASSERT_EQUAL_UINT(10,      token->slice.j);
    TEST_ASSERT_EQUAL_UINT(0,       source->err);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_simple_literal);

    return UNITY_END();
}

