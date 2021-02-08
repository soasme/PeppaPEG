#include "unity/src/unity.h"
#include "common.h"

# define ENTRY 1
# define WHITESPACE 2


// Sequence # TIGHT

/*
 * Rules:
 *  ENTRY = "HELLO" & "WORLD"
 *  WHITESPACE = " " | "\t" | "\n" # SPACED
 * Input:
 *  "HELLO \t\n\t WORLD"
 * Output:
 *   ENTRY: "HELLO \t\n\t WORLD"
 */
P4_PRIVATE(void) test_spaced_rule_should_loosen_sequence(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateLiteral("HELLO", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("WORLD", true))
    );
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("HELLO \t\n\t WORLD", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


/*
 * Rules:
 *  ENTRY = "HELLO" & "WORLD" # TIGHT
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "HELLOWORLD"
 * Output:
 *  ENTRY: "HELLOWORLD"
 */
P4_PRIVATE(void) test_spaced_rule_should_be_ignored_in_sequence(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateLiteral("HELLO", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("WORLD", true))
    );
    P4_SetExpressionFlag(entry, P4_FLAG_TIGHT);
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLOWORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("HELLOWORLD", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "HELLO" & "WORLD" # TIGHT
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "HELLO \t\n\t WORLD"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
P4_PRIVATE(void) test_spaced_rule_should_be_ignored_in_sequence2(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, 2)
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateLiteral("HELLO", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("WORLD", true))
    );
    P4_SetExpressionFlag(entry, P4_FLAG_TIGHT);
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", ENTRY);
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


// Repeat # TIGHT

/*
 * Rules:
 *  ENTRY = "0"{6}
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "0 0\t0\n0\t0 0"
 * Output:
 *   ENTRY: "0 0\t0\n0\t0 0"
 */
P4_PRIVATE(void) test_spaced_rule_should_be_applied_in_repeat(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateLiteral("0", true), 6)
    );
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("0 0\t0\n0\t0 0", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("0 0\t0\n0\t0 0", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


/*
 * Rules:
 *  ENTRY = "0"{6} # TIGHT
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "0 0\t0\n0\t0 0"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
P4_PRIVATE(void) test_spaced_rule_should_be_ignored_in_tight_repeat(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateLiteral("0", true), 6)
    );
    P4_SetExpressionFlag(P4_GetGrammarRule(grammar, ENTRY), P4_FLAG_TIGHT);
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("0 0\t0\n0\t0 0", ENTRY);
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
 *  ENTRY = "0"{6} # TIGHT
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "000000 0\t0\n0\t0 0"
 * Output:
 *  ENTRY: "000000"
 */
P4_PRIVATE(void) test_spaced_rule_should_be_ignored_in_tight_repeat2(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateLiteral("0", true), 6)
    );
    P4_SetExpressionFlag(P4_GetGrammarRule(grammar, ENTRY), P4_FLAG_TIGHT);
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("000000 0\t0\n0\t0 0", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("000000", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

// ---

// Sequence # SQUASH
/*
 * Rules:
 *  ENTRY = ZERO & ZERO # SQUASH
 *  ZERO  = "0"
 * Input:
 *  "00"
 * Output:
 *  ENTRY: "00"
 */

// Repeat # SQUASH
/*
 * Rules:
 *  ENTRY = ZERO* # SQUASH
 *  ZERO  = "0"
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */

// Others: No effect # SQUASH

// ---

// Literal # LIFT
/*
 * Rules:
 *  ENTRY = ZERO*
 *  ZERO  = "0" # LIFT
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */

// Range # LIFT
// Range # LIFT
// Choice # LIFT
// Reference # LIFT
// Repeat # LIFT
// Positive/Negative: No effect # LIFT

// ---

// Sequence # SQUASH,LIFT

// Repeat # SQUASH,LIFT

// ---

// Sequence # SQUASH,
// Literal # SCOPED.

// Sequence # SQUASH,LIFT
// Literal # SCOPED.

// Repeat # SQUASH,
// Literal # SCOPED.

// Repeat # SQUASH,LIFT
// Literal # SCOPED.

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_spaced_rule_should_loosen_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_sequence2);
    RUN_TEST(test_spaced_rule_should_be_applied_in_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat2);

    return UNITY_END();
}
