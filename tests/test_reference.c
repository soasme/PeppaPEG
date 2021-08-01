#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  ENTRY = R1
 *  R1 = "HELLO"
 * Input:
 *  "HELLO"
 * Output:
 *   ENTRY:
 *     R1: "HELLO"
 */
void test_match_reference_successfully(void) {
# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddReference(grammar, ENTRY, "entry", "r1")
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "r1", "HELLO", true)
    );

    P4_Source* source = P4_CreateSource("HELLO", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("HELLO", node);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);

    TEST_ASSERT_NOT_NULL(node->head);
    TEST_ASSERT_EQUAL(node->head, node->tail);
    ASSERT_EQUAL_NODE_STRING("HELLO", node);
    ASSERT_EQUAL_NODE_RULE(R1, node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = R1
 * Input:
 *  "HELLO"
 * Error:
 *  P4_NameError
 * Output:
 *  NULL
 */
void test_match_reference_not_defined_raise_name_error(void) {
# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddReference(grammar, ENTRY, "entry", "r1")
    );

    P4_Source* source = P4_CreateSource("HELLO", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_NameError,
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

    RUN_TEST(test_match_reference_successfully);
    RUN_TEST(test_match_reference_not_defined_raise_name_error);

    return UNITY_END();
}
