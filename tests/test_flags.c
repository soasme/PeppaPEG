#include "unity/src/unity.h"
#include "common.h"

# define ENTRY 1
# define WHITESPACE 2
# define R1 3


/* Sequence # TIGHT */

/*
 * Rules:
 *  ENTRY = "HELLO" & "WORLD"
 *  WHITESPACE = " " | "\t" | "\n" # SPACED
 * Input:
 *  "HELLO \t\n\t WORLD"
 * Output:
 *   ENTRY: "HELLO \t\n\t WORLD"
 */
void test_spaced_rule_should_loosen_sequence(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(15, P4_GetSourcePosition(source));

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
void test_spaced_rule_should_be_ignored_in_sequence(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_TIGHT));

    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLOWORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

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
void test_spaced_rule_should_be_ignored_in_sequence2(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", ENTRY);
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


/* Repeat # TIGHT */

/*
 * Rules:
 *  ENTRY = "0"{6}
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "0 0\t0\n0\t0 0"
 * Output:
 *   ENTRY: "0 0\t0\n0\t0 0"
 */
void test_spaced_rule_should_be_applied_in_repeat(void) {
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
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

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
void test_spaced_rule_should_be_ignored_in_tight_repeat(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateLiteral("0", true), 6)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("0 0\t0\n0\t0 0", ENTRY);
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
 *  ENTRY = "0"{6} # TIGHT
 *  WHITESPACE = " " | "\t" | "\n" # SPACED,LIFTED
 * Input:
 *  "000000 0\t0\n0\t0 0"
 * Output:
 *  ENTRY: "000000"
 */
void test_spaced_rule_should_be_ignored_in_tight_repeat2(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateLiteral("0", true), 6)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, WHITESPACE);

    P4_Source* source = P4_CreateSource("000000 0\t0\n0\t0 0", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

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

/* --- */

/* Sequence # SQUASH */

/*
 * Rules:
 *  ENTRY = Z & Z # SQUASH
 *  Z  = "0"
 * Input:
 *  "00"
 * Output:
 *  ENTRY: "00"
 */
void test_squashed_rule_should_generate_no_children(void) {
# define Z 3
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateReference(Z),
            P4_CreateReference(Z)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, Z, "0", true)
    );

    P4_Source* source = P4_CreateSource("00", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("00", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Repeat # SQUASH */
/*
 * Rules:
 *  ENTRY = ZERO* # SQUASH
 *  ZERO  = "0"
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */
void test_squashed_repeat_should_generate_no_children(void) {
# define Z 3
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrMore(grammar, ENTRY, P4_CreateReference(Z))
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, Z, "0", true)
    );

    P4_Source* source = P4_CreateSource("000000", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

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

/* Others: No effect # SQUASH */

/* --- */

/* Literal # LIFT */
/*
 * Rules:
 *  ENTRY = "0" # LIFT
 * Input:
 *  "0"
 * Output:
 *  NULL
 */
void test_lifted_literal_should_generate_no_token(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, ENTRY, "0", false)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("0", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Range # LIFT */
/*
 * Rules:
 *  ENTRY = '0'..'9' # LIFT
 * Input:
 *  "0"
 * Output:
 *  NULL
 */
void test_lifted_range_should_generate_no_token(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, ENTRY, '0', '9')
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("0", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Choice # LIFT */
/*
 * Rules:
 *  ENTRY = "HELLO" | "KIA ORA" # LIFT
 * Input:
 *  "KIA ORA"
 * Output:
 *  NULL
 */
void test_lifted_choice_should_generate_no_token(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, ENTRY, 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("KIA ORA", true)
        )
    );

    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("KIA ORA", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}
/* Reference # LIFT */

/* Repeat # LIFT */
void test_lifted_repeat_should_generate_no_token(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrMore(grammar, ENTRY, P4_CreateLiteral("0", true))
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("00000", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Positive/Negative: No effect # LIFT */

/* --- */

/* --- */

/* Sequence # SQUASH, */
/* Literal # SCOPED. */
/*
 * Rules:
 *  ENTRY = R1 & R1 # SQUASHED
 *  R1 = "1" # SCOPED
 * Input:
 *  "11"
 * Output:
 *  ENTRY:
 *   ONE: "1"
 *   ONE: "1"
 */
void test_squashed_sequence_should_not_hide_scoped_literal(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateReference(R1),
            P4_CreateReference(R1)
        )
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, R1, P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("11", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->head);

    TEST_ASSERT_EQUAL(token->head->next, token->tail);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Sequence # SQUASH,LIFT */
/* Literal # SCOPED. */
/*
 * Rules:
 *  ENTRY = R1 & R1 # SQUASHED,LIFTED
 *  R1 = "1" # SCOPED
 * Input:
 *  "11"
 * Output:
 *  ONE: "1"
 *  ONE: "1"
 */
void test_squashed_lifted_sequence_should_not_hide_scoped_literal(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, 2,
            P4_CreateReference(R1),
            P4_CreateReference(R1)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED | P4_FLAG_LIFTED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, R1, P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);

    TEST_ASSERT_NOT_NULL(token->next);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->next);

    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Repeat # SQUASHED, */
/* Literal # SCOPED. */
/*
 * Rules:
 *  ENTRY = R1{2} # SQUASHED
 *  R1 = "1" # SCOPED
 * Input:
 *  "11"
 * Output:
 *  ENTRY:
 *   ONE: "1"
 *   ONE: "1"
 */
void test_squashed_repeat_should_not_hide_scoped_literal(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateReference(R1), 2)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, R1, P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("11", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(ENTRY, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->head);

    TEST_ASSERT_EQUAL(token->head->next, token->tail);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/* Repeat # SQUASH,LIFT */
/* Literal # SCOPED. */
/*
 * Rules:
 *  ENTRY = R1{2} # SQUASHED,LIFTED
 *  R1 = "1" # SCOPED
 * Input:
 *  "11"
 * Output:
 *  R1: "1"
 *  R1: "1"
 */
void test_squashed_lifted_repeat_should_not_hide_scoped_literal(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, ENTRY, P4_CreateReference(R1), 2)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED | P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, R1, P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);

    TEST_ASSERT_NOT_NULL(token->next);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token->next);

    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_spaced_rule_should_loosen_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_sequence2);
    RUN_TEST(test_spaced_rule_should_be_applied_in_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat2);
    RUN_TEST(test_squashed_rule_should_generate_no_children);
    RUN_TEST(test_squashed_repeat_should_generate_no_children);
    RUN_TEST(test_lifted_literal_should_generate_no_token);
    RUN_TEST(test_lifted_range_should_generate_no_token);
    RUN_TEST(test_lifted_choice_should_generate_no_token);
    RUN_TEST(test_lifted_repeat_should_generate_no_token);

    RUN_TEST(test_squashed_lifted_sequence_should_not_hide_scoped_literal);
    RUN_TEST(test_squashed_repeat_should_not_hide_scoped_literal);
    RUN_TEST(test_squashed_lifted_repeat_should_not_hide_scoped_literal);

    return UNITY_END();
}
