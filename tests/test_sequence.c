#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  ENTRY = "你好" & " " & "WORLD"
 * Input:
 *  "你好 WORLD"
 * Output:
 *   ENTRY: "你好 WORLD"
 */
P4_PRIVATE(void) test_match_literal_sequence_successfully(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 3,
            P4_CreateLiteral("你好", true),
            P4_CreateLiteral(" ", true),
            P4_CreateLiteral("WORLD", true)
        )
    );

    P4_Source* source = P4_CreateSource("你好 WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(12, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, ENTRY));

    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("你好 WORLD", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "你好" & " " & "WORLD"
 * Input:
 *  "你好 "
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
P4_PRIVATE(void) test_match_literal_sequence_partially_raise_match_error(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 3,
            P4_CreateLiteral("你好", true),
            P4_CreateLiteral(" ", true),
            P4_CreateLiteral("WORLD", true)
        )
    );

    P4_Source* source = P4_CreateSource("你好 ", ENTRY);
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

/*
 * Rules:
 *  ENTRY = "你好" & " " & "WORLD"
 * Input:
 *  "你好 WORL"
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
P4_PRIVATE(void) test_match_literal_sequence_having_member_nomatch_raise_match_error(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 3,
            P4_CreateLiteral("你好", true),
            P4_CreateLiteral(" ", true),
            P4_CreateLiteral("WORLD", true)
        )
    );

    P4_Source* source = P4_CreateSource("你好 WORL", ENTRY);
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

/*
 * Rules:
 *  ENTRY = R1 & R1
 *  R1 = i"OLA"
 * Input:
 *  "OLAola"
 * Output:
 *   ENTRY:
 *     R1: "OLA"
 *     R1: "ola"
 */
P4_PRIVATE(void) test_match_reference_in_sequence_successfully(void) {
# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateReference(R1),
            P4_CreateReference(R1)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "OLA", false)
    );

    P4_Source* source = P4_CreateSource("OLAola", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, ENTRY));
    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("OLAola", tokenstr);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("OLA", token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->head);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_STRING("ola", token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_literal_sequence_successfully);
    RUN_TEST(test_match_literal_sequence_partially_raise_match_error);
    RUN_TEST(test_match_literal_sequence_having_member_nomatch_raise_match_error);
    RUN_TEST(test_match_reference_in_sequence_successfully);

    return UNITY_END();
}
