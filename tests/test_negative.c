#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  ENTRY = NEGATIVE("Hello") & i"HELLO WORLD"
 * Input:
 *  "HELLO WORLD"
 * Output:
 *  ENTRY: HELLO WORLD
 */
P4_PRIVATE(void) test_match_negative_successfully(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateNegative(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", ENTRY);
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
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, ENTRY));

    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = R1 & R2
 *  R1 = NEGATIVE("Hello")
 *  R2 = i"HELLO WORLD"
 * Input:
 *  "HELLO WORLD"
 * Output:
 *  ENTRY:
 *    R2: HELLO WORLD
 */
P4_PRIVATE(void) test_match_negative_consumes_no_input(void) {
# define ENTRY  1
# define R1     2
# define R2     3
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetReferenceMember(entry, 0, R1)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetReferenceMember(entry, 1, R2)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddNegative(grammar, R1, P4_CreateLiteral("Hello", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R2, "HELLO WORLD", false)
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, ENTRY));
    TEST_ASSERT_NULL(token->next);
    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", tokenstr);

    /* R1(Positive) has no token produced. */

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL(token->head, token->tail);
    TEST_ASSERT_EQUAL(token->head->expr, P4_GetGrammarRule(grammar, R2));
    autofree P4_String theadstr = P4_CopyTokenString(token->head);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", theadstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = NEGATIVE("Hello") & i"Hello World"
 * Input:
 *  "HELLO WORLD"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
P4_PRIVATE(void) test_match_negative_failed(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateNegative(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("Hello WORLD", ENTRY);
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

    RUN_TEST(test_match_negative_successfully);
    RUN_TEST(test_match_negative_consumes_no_input);
    RUN_TEST(test_match_negative_failed);

    return UNITY_END();
}

