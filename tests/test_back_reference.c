#include "unity/src/unity.h"
#include "common.h"

# define ENTRY  1
# define MARKER 2

/*
 * Rules:
 *  ENTRY = MARKER "..." \1
 *  MARKER = "'" / "\""
 * Input:
 *  '...'
 * Output:
 *  ENTRY:
 *    START: "'"
 *    END: "'"
 */
void test_match_back_reference_successfully(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
            P4_CreateReference("marker"),
            P4_CreateLiteral("...", true),
            P4_CreateBackReference(0, true)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, MARKER, "marker", 2,
            P4_CreateLiteral("'", true),
            P4_CreateLiteral("\"", true)
        )
    );

    P4_Source* source = P4_CreateSource("'...'", ENTRY);
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL_MESSAGE(
        P4_Ok,
        P4_Parse(grammar, source),
        "Match Failed"
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("'...'", node);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);

    P4_Node* head = node->head;
    TEST_ASSERT_NOT_NULL(head);
    ASSERT_EQUAL_NODE_STRING("'", head);
    ASSERT_EQUAL_NODE_RULE(MARKER, head);

    P4_Node* tail = node->tail;
    TEST_ASSERT_NOT_NULL(tail);
    TEST_ASSERT_NOT_EQUAL(head, tail);
    ASSERT_EQUAL_NODE_STRING("'", tail);
    ASSERT_EQUAL_NODE_RULE(MARKER, tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = MARKER "..." \1
 *  MARKER = "'" / "\""
 * Input:
 *  "...'
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_back_reference_latter_not_match(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
            P4_CreateReference("marker"),
            P4_CreateLiteral("...", true),
            P4_CreateBackReference(0, true)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, MARKER, "marker", 2,
            P4_CreateLiteral("'", true),
            P4_CreateLiteral("\"", true)
        )
    );

    P4_Source* source = P4_CreateSource("\"...'", ENTRY);
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

/*
 * Rules:
 *  ENTRY = MARKER "..." i\1
 *  MARKER = i"TAG"
 * Input:
 *  TAG...tag"
 * Output:
 *  ENTRY:
 *    MARKER: "TAG"
 *    MARKER: "tag"
 */
void test_match_back_reference_insensitive_match(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, ENTRY, "entry", 3,
            P4_CreateReference("marker"),
            P4_CreateLiteral("...", true),
            P4_CreateBackReference(0, false)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, MARKER, "marker", "TAG", true)
    );

    P4_Source* source = P4_CreateSource("TAG...tag", ENTRY);
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL_MESSAGE(
        P4_Ok,
        P4_Parse(grammar, source),
        "Match Failed"
    );

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("TAG...tag", node);
    ASSERT_EQUAL_NODE_RULE(ENTRY, node);

    P4_Node* head = node->head;
    TEST_ASSERT_NOT_NULL(head);
    ASSERT_EQUAL_NODE_STRING("TAG", head);
    ASSERT_EQUAL_NODE_RULE(MARKER, head);

    P4_Node* tail = node->tail;
    TEST_ASSERT_NOT_NULL(tail);
    TEST_ASSERT_NOT_EQUAL(head, tail);
    ASSERT_EQUAL_NODE_STRING("tag", tail);
    ASSERT_EQUAL_NODE_RULE(MARKER, tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_back_reference_successfully);
    RUN_TEST(test_match_back_reference_latter_not_match);
    RUN_TEST(test_match_back_reference_insensitive_match);

    return UNITY_END();
}
