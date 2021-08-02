#include "unity/src/unity.h"
#include "common.h"

# define ENTRY 1

/*
 * Rules:
 *  ENTRY = "0"*
 * Input:
 *  ""
 * Output:
 *  NULL
 */
void test_match_zeroormore_at_least_zero(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
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
 *  ENTRY = "0"*
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */
void test_match_zeroormore_multiple_times(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("00000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("00000", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"+
 * Input:
 *  "0"
 * Output:
 *  ENTRY: "0"
 */
void test_match_onceormore_at_least_once(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddOnceOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("entry", node);
    ASSERT_EQUAL_NODE_STRING("0", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"+
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */
void test_match_onceormore_multiple_times(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddOnceOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("00000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("entry", node);
    ASSERT_EQUAL_NODE_STRING("00000", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"+
 * Input:
 *  "1"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_onceormore_zero_raise_match_error(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddOnceOrMore(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("1", "entry");
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
 *  ENTRY = "0"?
 * Input:
 *  ""
 * Output:
 *  NULL
 */
void test_match_zerooronce_match_empty(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrOnce(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
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
 *  ENTRY = "0"?
 * Input:
 *  "0"
 * Output:
 *  ENTRY: "0"
 */
void test_match_zerooronce_exact_once(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrOnce(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("0", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"?
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "0"
 */
void test_match_zerooronce_at_most_once(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddZeroOrOnce(grammar, "entry", P4_CreateLiteral("0", true))
    );

    P4_Source* source = P4_CreateSource("00000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("0", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"{5}
 * Input:
 *  "00000"
 * Output:
 *  ENTRY: "00000"
 */
void test_match_repeat_exact_successfully(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 5)
    );

    P4_Source* source = P4_CreateSource("00000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("00000", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = "0"{5}
 * Input:
 *  "0000"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_repeat_exact_no_less(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 5)
    );

    P4_Source* source = P4_CreateSource("0000", "entry");
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
 *  ENTRY = "0"{5}
 * Input:
 *  "00001"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_repeat_exact_insufficient_attaching_unmatch(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 5)
    );

    P4_Source* source = P4_CreateSource("00001", "entry");
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
 *  ENTRY = "0"{5}
 * Input:
 *  "000000"
 * Output:
 *  ENTRY: "00000"
 */
void test_match_repeat_exact_no_more(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRepeatExact(grammar, "entry", P4_CreateLiteral("0", true), 5)
    );

    P4_Source* source = P4_CreateSource("000000", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("00000", node);
    ASSERT_EQUAL_NODE_RULE("entry", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_zeroormore_at_least_zero);
    RUN_TEST(test_match_zeroormore_multiple_times);
    RUN_TEST(test_match_onceormore_at_least_once);
    RUN_TEST(test_match_onceormore_multiple_times);
    RUN_TEST(test_match_onceormore_zero_raise_match_error);
    RUN_TEST(test_match_zerooronce_match_empty);
    RUN_TEST(test_match_zerooronce_exact_once);
    RUN_TEST(test_match_zerooronce_at_most_once);
    RUN_TEST(test_match_repeat_exact_successfully);
    RUN_TEST(test_match_repeat_exact_no_less);
    RUN_TEST(test_match_repeat_exact_insufficient_attaching_unmatch);
    RUN_TEST(test_match_repeat_exact_no_more);

    return UNITY_END();
}
