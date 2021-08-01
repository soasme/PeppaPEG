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
void test_match_negative_successfully(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, "entry", 2)
    );
    P4_Expression* entry = P4_GetGrammarRuleByName(grammar, "entry");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateNegative(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("entry", node);
    ASSERT_EQUAL_NODE_STRING("HELLO WORLD", node);

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
void test_match_negative_consumes_no_input(void) {
# define ENTRY  1
# define R1     2
# define R2     3
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, "entry", 2)
    );
    P4_Expression* entry = P4_GetGrammarRuleByName(grammar, "entry");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetReferenceMember(entry, 0, "r1")
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetReferenceMember(entry, 1, "r2")
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddNegative(grammar, "r1", P4_CreateLiteral("Hello", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r2", "HELLO WORLD", false)
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("entry", node);
    ASSERT_EQUAL_NODE_STRING("HELLO WORLD", node);

    TEST_ASSERT_NULL(node->next);

    /* R1(Positive) has no node produced. */

    TEST_ASSERT_NOT_NULL(node->head);
    TEST_ASSERT_EQUAL(node->head, node->tail);
    ASSERT_EQUAL_NODE_RULE("r2", node->head);
    ASSERT_EQUAL_NODE_STRING("HELLO WORLD", node->head);

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
void test_match_negative_failed(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, "entry", 2)
    );
    P4_Expression* entry = P4_GetGrammarRuleByName(grammar, "entry");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreateNegative(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("Hello WORLD", "entry");
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

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_negative_successfully);
    RUN_TEST(test_match_negative_consumes_no_input);
    RUN_TEST(test_match_negative_failed);

    return UNITY_END();
}

