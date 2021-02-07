#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

P4_PRIVATE(void) test_match_case_insensitive_literal(void) {
# define SIMPLE_LITERAL 1

    P4_Grammar* grammar;
    P4_Source*  source;
    P4_Token*   token;
    P4_Slice*   slice;

    // Setup
    grammar = P4_CreateGrammar();

    TEST_ASSERT_NOT_NULL(grammar);

    P4_AddLiteral(grammar, SIMPLE_LITERAL, "Hello World", false);

    TEST_ASSERT_EQUAL_UINT(1, grammar->count);
    TEST_ASSERT_NOT_NULL(grammar->rules);


    // case 1: same literal "Hello World".
    {
        source = P4_CreateSource("Hello World", SIMPLE_LITERAL);

        TEST_ASSERT_EQUAL_UINT(P4_Ok, P4_Parse(grammar, source));

        token = P4_GetSourceAst(source);

        TEST_ASSERT_NOT_NULL(token);
        TEST_ASSERT_NULL(token->next);
        TEST_ASSERT_NULL(token->head);
        TEST_ASSERT_NULL(token->tail);

        slice = P4_GetTokenSlice(token);

        TEST_ASSERT_EQUAL_SLICE(slice, 0, 11);

        P4_DeleteSource(source);
    }

    // case 2: case insensitive literal, but still match.
    {
        source = P4_CreateSource("HeLlO WoRlD", SIMPLE_LITERAL);

        TEST_ASSERT_EQUAL_UINT(P4_Ok, P4_Parse(grammar, source));

        token = P4_GetSourceAst(source);

        TEST_ASSERT_NOT_NULL(token);
        TEST_ASSERT_NULL(token->next);
        TEST_ASSERT_NULL(token->head);
        TEST_ASSERT_NULL(token->tail);

        slice = P4_GetTokenSlice(token);

        TEST_ASSERT_EQUAL_SLICE(slice, 0, 11);

        P4_DeleteSource(source);
    }

    // case 3: completely different literal, no match.
    {
        source = P4_CreateSource("HEll0 W0R1D", SIMPLE_LITERAL);

        TEST_ASSERT_EQUAL_UINT(P4_MatchError, P4_Parse(grammar, source));

        token = P4_GetSourceAst(source);

        TEST_ASSERT_NULL(token);

        P4_DeleteSource(source);
    }

    // tear down.
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_case_insensitive_literal);

    return UNITY_END();
}
