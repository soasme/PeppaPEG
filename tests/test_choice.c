#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "你好, 世界"
 * Output:
 *   R1: "你好, 世界"
 */
void test_match_literal_choices_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("你好, 世界", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(14, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    ASSERT_EQUAL_TOKEN_RULE(R1, token);
    ASSERT_EQUAL_TOKEN_STRING("你好, 世界", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "HELLO WORLD"
 * Output:
 *   R1: "HELLO WORLD"
 */
void test_match_literal_choices_successfully2(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    ASSERT_EQUAL_TOKEN_RULE(R1, token);
    ASSERT_EQUAL_TOKEN_STRING("HELLO WORLD", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "HELLO WORL"
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
void test_match_literal_choices_no_option_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("HELLO WORL", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_literal_choices_successfully);
    RUN_TEST(test_match_literal_choices_successfully2);
    RUN_TEST(test_match_literal_choices_no_option_raise_match_error);

    return UNITY_END();
}

