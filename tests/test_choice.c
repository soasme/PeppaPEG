#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "你好, 世界"
 * Output:
 *   R1: "你好, 世界"
 */
void test_match_literal_choices_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, "r1", 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("你好, 世界", "r1");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(14, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(R1, node);
    ASSERT_EQUAL_NODE_STRING("你好, 世界", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "HELLO WORLD"
 * Output:
 *   R1: "HELLO WORLD"
 */
void test_match_literal_choices_successfully2(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, "r1", 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", "r1");
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
    ASSERT_EQUAL_NODE_RULE(R1, node);
    ASSERT_EQUAL_NODE_STRING("HELLO WORLD", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "HELLO WORLD" | "你好, 世界"
 * Input:
 *  "HELLO WORL"
 * Error:
 *  P4_MatchError
 * Output:
 *   NULL
 */
void test_match_literal_choices_no_option_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, R1, "r1", 2,
            P4_CreateLiteral("HELLO WORLD", true),
            P4_CreateLiteral("你好, 世界", true)
        )
    );

    P4_Source* source = P4_CreateSource("HELLO WORL", "r1");
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

    RUN_TEST(test_match_literal_choices_successfully);
    RUN_TEST(test_match_literal_choices_successfully2);
    RUN_TEST(test_match_literal_choices_no_option_raise_match_error);

    return UNITY_END();
}

