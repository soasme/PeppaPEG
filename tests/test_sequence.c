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
void test_match_literal_sequence_successfully(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
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

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);
    ASSERT_EQUAL_NODE_STRING("你好 WORLD", node);

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
void test_match_literal_sequence_partially_raise_match_error(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
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

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
void test_match_literal_sequence_having_member_nomatch_raise_match_error(void) {
# define ENTRY 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
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

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(node);

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
void test_match_reference_in_sequence_successfully(void) {
# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 2,
            P4_CreateReference(R1),
            P4_CreateReference(R1)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "r1", "OLA", false)
    );

    P4_Source* source = P4_CreateSource("OLAola", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(6, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);
    ASSERT_EQUAL_NODE_STRING("OLAola", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE(R1, node->head);
    ASSERT_EQUAL_NODE_STRING("OLA", node->head);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE(R1, node->tail);
    ASSERT_EQUAL_NODE_STRING("ola", node->tail);

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
