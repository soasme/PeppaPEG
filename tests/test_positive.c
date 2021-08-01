#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  ENTRY = POSITIVE("Hello") & i"HELLO WORLD"
 * Input:
 *  "Hello WORLD"
 * Output:
 *  ENTRY: Hello WORLD
 */
void test_match_positive_successfully(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, "entry", 2)
    );
    P4_Expression* entry = P4_GetGrammarRuleByName(grammar, "entry");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreatePositive(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("Hello WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);
    ASSERT_EQUAL_NODE_STRING("Hello WORLD", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = R1 & R2
 *  R1 = POSITIVE("Hello")
 *  R2 = i"HELLO WORLD"
 * Input:
 *  "Hello WORLD"
 * Output:
 *  ENTRY:
 *    R2: Hello WORLD
 */
void test_match_positive_consumes_no_input(void) {
# define ENTRY  1
# define R1     2
# define R2     3
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, "entry", 2)
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
        P4_AddPositive(grammar, R1, "r1", P4_CreateLiteral("Hello", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R2, "r2", "HELLO WORLD", false)
    );

    P4_Source* source = P4_CreateSource("Hello WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);
    ASSERT_EQUAL_NODE_STRING("Hello WORLD", node);

    /* R1(Positive) has no node produced. */

    TEST_ASSERT_NOT_NULL(node->head);
    TEST_ASSERT_EQUAL(node->head, node->tail);
    ASSERT_EQUAL_NODE_RULE(R2, node->head);
    ASSERT_EQUAL_NODE_STRING("Hello WORLD", node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = POSITIVE("Hello") & i"Hello World"
 * Input:
 *  "HELLO WORLD"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_positive_failed(void) {
# define ENTRY  1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequence(grammar, ENTRY, "entry", 2)
    );
    P4_Expression* entry = P4_GetGrammarRuleByName(grammar, "entry");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 0, P4_CreatePositive(P4_CreateLiteral("Hello", true)))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(entry, 1, P4_CreateLiteral("HELLO WORLD", false))
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_positive_successfully);
    RUN_TEST(test_match_positive_consumes_no_input);
    RUN_TEST(test_match_positive_failed);

    return UNITY_END();
}
