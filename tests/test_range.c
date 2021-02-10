#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  R1 = '0'..'9';
 * Input:
 *  "0"
 * Output:
 *   R1: "0"
 */
P4_PRIVATE(void) test_match_ascii_digits_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, '0', '9')
    );

    P4_Source* source = P4_CreateSource("0", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, R1));

    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("0", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = '0x0001'..'0x10ffff';
 * Input:
 *  "好"
 * Output:
 *   R1: "好"
 */
P4_PRIVATE(void) test_match_utf8_code_point_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, 0x4E00, 0X9FCC)
    );

    P4_Source* source = P4_CreateSource("好", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, R1));

    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("好", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = '0x4E00'..'0X9FCC';
 * Input:
 *  "good"
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
P4_PRIVATE(void) test_match_code_points_outside_range_cause_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, 0x4E00, 0X9FCC)
    );

    P4_Source* source = P4_CreateSource("Good", R1);
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

    RUN_TEST(test_match_ascii_digits_successfully);
    RUN_TEST(test_match_utf8_code_point_successfully);
    RUN_TEST(test_match_code_points_outside_range_cause_match_error);

    return UNITY_END();
}
