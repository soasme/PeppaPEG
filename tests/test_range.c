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
void test_match_ascii_digits_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, '0', '9', 1)
    );

    P4_Source* source = P4_CreateSource("0", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(R1, node);
    ASSERT_EQUAL_NODE_STRING("0", node);

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
void test_match_utf8_code_point_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, 0x4E00, 0X9FCC, 1)
    );

    P4_Source* source = P4_CreateSource("好", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(R1, node);
    ASSERT_EQUAL_NODE_STRING("好", node);

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
void test_match_code_points_outside_range_cause_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, R1, 0x4E00, 0X9FCC, 1)
    );

    P4_Source* source = P4_CreateSource("Good", R1);
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

    RUN_TEST(test_match_ascii_digits_successfully);
    RUN_TEST(test_match_utf8_code_point_successfully);
    RUN_TEST(test_match_code_points_outside_range_cause_match_error);

    return UNITY_END();
}
