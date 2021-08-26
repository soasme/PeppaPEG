#include "unity/src/unity.h"
#include "common.h"

/**
 * Grammar
 *  entry = "a" | entry "b";
 * Input
 *  abb
 */
void test_match_left_recursion_basic(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLeftRecursion(grammar, "entry",
            P4_CreateLiteral("a", true),
            P4_CreateLiteral("b", true)
        )
    );
    P4_Source* source = P4_CreateSource("abb", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("abb", node);

    /**
     *      (abb)
     *   (ab)
     */

    TEST_ASSERT_NOT_NULL(node->head);
    TEST_ASSERT_EQUAL(node->head, node->tail);
    ASSERT_EQUAL_NODE_STRING("ab", node->head);

    TEST_ASSERT_NULL(node->head->head);
    TEST_ASSERT_NULL(node->head->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/**
 * Grammar:
 *  entry = A | entry B;
 *  A = "a";
 *  B = "b";
 * Input:
 *  abb
 */
void test_match_left_recursion_references(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLeftRecursion(grammar, "entry",
            P4_CreateReference("A"),
            P4_CreateReference("B")
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "A", "a", true)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "B", "b", true)
    );
    P4_Source* source = P4_CreateSource("abb", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("abb", node);

    /**
     *      (abb)
     *   (ab)    (b)
     * (a) (b)
     */

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("ab", node->head);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_STRING("b", node->tail);

    TEST_ASSERT_NOT_NULL(node->head->head);
    ASSERT_EQUAL_NODE_STRING("a", node->head->head);

    TEST_ASSERT_NOT_NULL(node->head->tail);
    ASSERT_EQUAL_NODE_STRING("b", node->head->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/**
 * Grammar:
 *  S = E | S op E;
 *  op = "+" / "-";
 *  E = [0-9];
 * Input:
 *  abb
 */
void test_match_left_recursion_sequence_on_rhs(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLeftRecursion(grammar, "S",
            P4_CreateReference("E"),
            P4_CreateSequenceWithMembers(2,
                P4_CreateReference("op"),
                P4_CreateReference("E")
            )
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, "E", '0', '9', 1)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, "op", 2,
            P4_CreateLiteral("+", true),
            P4_CreateLiteral("-", true)
        )
    );
    P4_Source* source = P4_CreateSource("1+2+3", "S");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("1+2+3", node);

    /**
     *            (1+2+3)
     *    (1+2)     (+)     (3)
     * (1) (+) (2)
     */

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("1+2", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_STRING("+", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_STRING("3", node->tail);

    TEST_ASSERT_NOT_NULL(node->head->head);
    ASSERT_EQUAL_NODE_STRING("1", node->head->head);

    TEST_ASSERT_NOT_NULL(node->head->head->next);
    ASSERT_EQUAL_NODE_STRING("+", node->head->head->next);

    TEST_ASSERT_NOT_NULL(node->head->tail);
    ASSERT_EQUAL_NODE_STRING("2", node->head->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/**
 * Grammar:
 *  @spaced ws = " " / "\t" / "\n";
 *  S = E | S op E;
 *  op = "+" / "-";
 *  E = [0-9];
 * Input:
 *  1 + 2  +  3
 */
void test_match_left_recursion_spaced(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ADD_WHITESPACE(grammar, "ws");
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLeftRecursion(grammar, "S",
            P4_CreateReference("E"),
            P4_CreateSequenceWithMembers(2,
                P4_CreateReference("op"),
                P4_CreateReference("E")
            )
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, "E", '0', '9', 1)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, "op", 2,
            P4_CreateLiteral("+", true),
            P4_CreateLiteral("-", true)
        )
    );
    P4_Source* source = P4_CreateSource("1 + 2  +  3", "S");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("1 + 2  +  3", node);

    /**
     *            (1 + 2  +  3)
     *    (1 + 2)     (+)     (3)
     *  (1) (+) (2)
     */

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("1 + 2", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_STRING("+", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_STRING("3", node->tail);

    TEST_ASSERT_NOT_NULL(node->head->head);
    ASSERT_EQUAL_NODE_STRING("1", node->head->head);

    TEST_ASSERT_NOT_NULL(node->head->head->next);
    ASSERT_EQUAL_NODE_STRING("+", node->head->head->next);

    TEST_ASSERT_NOT_NULL(node->head->tail);
    ASSERT_EQUAL_NODE_STRING("2", node->head->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/** Because lifted eliminates the hierarchy of left recursion.
 *  which in essence makes the expression become (lhs rhs*).
 * */
void test_left_recursion_cannot_use_with_lifted(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLeftRecursion(grammar, "entry",
            P4_CreateLiteral("a", true),
            P4_CreateLiteral("b", true)
        )
    );
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED));

    P4_Source* source = P4_CreateSource("abbb", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_PegError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL_STRING("line 1:2, left recursion rule entry cannot be lifted", P4_GetErrorMessage(source));
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_left_recursion_basic);
    RUN_TEST(test_match_left_recursion_references);
    RUN_TEST(test_match_left_recursion_sequence_on_rhs);
    RUN_TEST(test_match_left_recursion_spaced);
    RUN_TEST(test_left_recursion_cannot_use_with_lifted);

    return UNITY_END();
}
