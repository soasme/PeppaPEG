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
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(15, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("HELLO \t\n\t WORLD", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
void test_spaced_rule_should_be_ignored_in_tight_sequence(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_TIGHT));

    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("HELLOWORLD", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("HELLOWORLD", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = R1 ":" R1
 *  R1 = "1" ~ "2" # TIGHT | SQUASHED
 *  WHITESPACE = " " | "\t" | "\n" # SPACED, LIFTED
 * Input:
 *  12  :  12
 * Output:
 *  ENTRY:
 *    R1: 12
 *    R1: 12
 */
void test_spaced_rule_should_loosen_sequence_despite_member_is_tight(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "entry", 3,
            P4_CreateReference("r1"),
            P4_CreateLiteral(":", true),
            P4_CreateReference("r1")
        )
    );
    TEST_ADD_WHITESPACE(grammar, "ws");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "r1", 2,
            P4_CreateLiteral("1", true),
            P4_CreateLiteral("2", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "r1", P4_FLAG_TIGHT | P4_FLAG_SQUASHED));

    P4_Source* source = P4_CreateSource("12  :  12", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("12  :  12", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

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
void test_spaced_rule_should_be_ignored_in_tight_sequence2(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateLiteral("HELLO", true),
            P4_CreateLiteral("WORLD", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("HELLO \t\n\t WORLD", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 6)
    );
    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("0 0\t0\n0\t0 0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("0 0\t0\n0\t0 0", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 6)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("0 0\t0\n0\t0 0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 6)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_TIGHT));
    TEST_ADD_WHITESPACE(grammar, "ws");

    P4_Source* source = P4_CreateSource("000000 0\t0\n0\t0 0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("000000", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateReference("z"),
            P4_CreateReference("z")
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "z", "0", true)
    );

    P4_Source* source = P4_CreateSource("00", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("00", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
        P4_AddZeroOrMore(grammar, "entry", P4_CreateReference("z"))
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "z", "0", true)
    );

    P4_Source* source = P4_CreateSource("000000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("000000", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
void test_lifted_literal_should_generate_no_node(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "entry", "0", false)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
void test_lifted_range_should_generate_no_node(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, "entry", '0', '9', 1)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
void test_lifted_choice_should_generate_no_node(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, "entry", 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("KIA ORA", true)
        )
    );

    P4_Expression* entry = P4_GetGrammarRule(grammar, "entry");
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("KIA ORA", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}
/* Reference # LIFT */

/* Repeat # LIFT */
void test_lifted_repeat_should_generate_no_node(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("00000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateReference("r1"),
            P4_CreateReference("r1")
        )
    );
    P4_Expression* entry = P4_GetGrammarRule(grammar, "entry");
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r1", "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "r1", P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("11", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("1", node->head);
    ASSERT_EQUAL_NODE_RULE("r1", node->head);

    TEST_ASSERT_EQUAL(node->head->next, node->tail);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_STRING("1", node->tail);
    ASSERT_EQUAL_NODE_RULE("r1", node->tail);

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
        P4_AddSequenceWithMembers(grammar, "entry", 2,
            P4_CreateReference("r1"),
            P4_CreateReference("r1")
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED | P4_FLAG_LIFTED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r1", "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "r1", P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("1", node);
    ASSERT_EQUAL_NODE_RULE("r1", node);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_STRING("1", node->next);
    ASSERT_EQUAL_NODE_RULE("r1", node->next);

    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

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
        P4_AddRepeatExact(grammar, "entry", P4_CreateReference("r1"), 2)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r1", "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "r1", P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("11", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("1", node->head);
    ASSERT_EQUAL_NODE_RULE("r1", node->head);

    TEST_ASSERT_EQUAL(node->head->next, node->tail);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_STRING("1", node->tail);
    ASSERT_EQUAL_NODE_RULE("r1", node->tail);

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
        P4_AddRepeatExact(grammar, "entry", P4_CreateReference("r1"), 2)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED | P4_FLAG_SQUASHED));
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r1", "1", true)
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "r1", P4_FLAG_SCOPED));

    P4_Source* source = P4_CreateSource("11", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("1", node);
    ASSERT_EQUAL_NODE_RULE("r1", node);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_STRING("1", node->next);
    ASSERT_EQUAL_NODE_RULE("r1", node->next);

    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_lift_repeat_for_single_child(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddOnceOrMore(grammar, "entry", P4_CreateReference("r1")));
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_NON_TERMINAL));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, "r1", "a", true));
    P4_Source* source = P4_CreateSource("a", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));
    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("a", node);
    ASSERT_EQUAL_NODE_RULE("r1", node);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_lift_sequence_for_single_child(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddSequenceWithMembers(grammar, "entry", 3,
            P4_CreateLiteral("(", true), P4_CreateReference("r1"), P4_CreateLiteral(")", true)));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, "r1", "a", true));
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_NON_TERMINAL));
    P4_Source* source = P4_CreateSource("(a)", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));
    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("a", node);
    ASSERT_EQUAL_NODE_RULE("r1", node);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_spaced_rule_should_loosen_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_sequence);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_sequence2);
    RUN_TEST(test_spaced_rule_should_loosen_sequence_despite_member_is_tight);
    RUN_TEST(test_spaced_rule_should_be_applied_in_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat);
    RUN_TEST(test_spaced_rule_should_be_ignored_in_tight_repeat2);
    RUN_TEST(test_squashed_rule_should_generate_no_children);
    RUN_TEST(test_squashed_repeat_should_generate_no_children);
    RUN_TEST(test_lifted_literal_should_generate_no_node);
    RUN_TEST(test_lifted_range_should_generate_no_node);
    RUN_TEST(test_lifted_choice_should_generate_no_node);
    RUN_TEST(test_lifted_repeat_should_generate_no_node);
    RUN_TEST(test_lift_repeat_for_single_child);
    RUN_TEST(test_lift_sequence_for_single_child);

    RUN_TEST(test_squashed_lifted_sequence_should_not_hide_scoped_literal);
    RUN_TEST(test_squashed_repeat_should_not_hide_scoped_literal);
    RUN_TEST(test_squashed_lifted_repeat_should_not_hide_scoped_literal);

    return UNITY_END();
}
