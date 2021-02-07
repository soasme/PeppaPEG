#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  R1 = "你好" & " " & "WORLD"
 * Input:
 *  "你好 WORLD"
 * Output:
 *   R1: "你好 WORLD"
 */
P4_PRIVATE(void) test_match_literal_sequence_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, R1, 3)
    );

    P4_Expression* e1 = P4_GetGrammarRule(grammar, R1);
    TEST_ASSERT_NOT_NULL(e1);

    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 0, P4_CreateLiteral("你好", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 1, P4_CreateLiteral(" ", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 2, P4_CreateLiteral("WORLD", true))
    );

    P4_Source* source = P4_CreateSource("你好 WORLD", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, R1));

    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("你好 WORLD", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "你好" & " " & "WORLD"
 * Input:
 *  "你好 "
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
P4_PRIVATE(void) test_match_literal_sequence_partially_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, R1, 3)
    );

    P4_Expression* e1 = P4_GetGrammarRule(grammar, R1);
    TEST_ASSERT_NOT_NULL(e1);

    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 0, P4_CreateLiteral("你好", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 1, P4_CreateLiteral(" ", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 2, P4_CreateLiteral("WORLD", true))
    );

    P4_Source* source = P4_CreateSource("你好 ", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "你好" & " " & "WORLD"
 * Input:
 *  "你好 WORL"
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
P4_PRIVATE(void) test_match_literal_sequence_having_member_nomatch_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, R1, 3)
    );

    P4_Expression* e1 = P4_GetGrammarRule(grammar, R1);
    TEST_ASSERT_NOT_NULL(e1);

    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 0, P4_CreateLiteral("你好", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 1, P4_CreateLiteral(" ", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddMember(e1, 2, P4_CreateLiteral("WORLD", true))
    );

    P4_Source* source = P4_CreateSource("你好 WORL", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_literal_sequence_successfully);
    RUN_TEST(test_match_literal_sequence_partially_raise_match_error);
    RUN_TEST(test_match_literal_sequence_having_member_nomatch_raise_match_error);

    return UNITY_END();
}
