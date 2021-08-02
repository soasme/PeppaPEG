#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

# define ENTRY 1
# define REFERENCE 2

/*
 * Set grammar recursion limit to 10.
 *
 * Rules:
 *  R1 = "x" / "-" R1
 * Input:
 *  "---------x"
 * Error:
 *  P4_StackError
 * Output:
 *   NULL
 */
void test_maximum_recursion_limit(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(P4_Ok,
        P4_AddChoiceWithMembers(grammar, "entry", 2,
            P4_CreateLiteral("x", true),
            P4_CreateSequenceWithMembers(2,
                P4_CreateLiteral("-", true),
                P4_CreateReference("entry")
            )
        )
    );

    P4_Source* source = P4_CreateSource("----------x", "entry");
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL(
        P4_DEFAULT_RECURSION_LIMIT,
        P4_GetRecursionLimit(grammar)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetRecursionLimit(grammar, 10)
    );
    TEST_ASSERT_EQUAL(10, P4_GetRecursionLimit(grammar));

    TEST_ASSERT_EQUAL(
        P4_StackError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_replace_grammar_rule(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, "entry", "{{", true));
    TEST_ASSERT_EQUAL(P4_Ok, P4_ReplaceGrammarRule(grammar, "entry", P4_CreateLiteral("<%", true)));
    P4_Source* source = P4_CreateSource("<%", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));
    P4_Node* root = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(root);
    ASSERT_EQUAL_NODE_STRING("<%", root);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_replace_grammar_rule_refreshing_references(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, "reference", "{{", true));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddReference(grammar, "entry", "reference"));
    TEST_ASSERT_EQUAL(P4_Ok, P4_ReplaceGrammarRule(grammar, "reference", P4_CreateLiteral("<%", true)));
    P4_Source* source = P4_CreateSource("<%", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* root = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(root);
    ASSERT_EQUAL_NODE_STRING("<%", root);

    /*
    P4_Expression* ref = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(ref->ref_expr, P4_GetGrammarRule(grammar, REFERENCE));
    */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_join(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    const int ROW = 1;
    const int NUM = 2;
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddJoin(grammar, "row", ",", "num"));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddRange(grammar, "num", '0', '9', 1));

    P4_Source* source = P4_CreateSource("1,2,3", "row");
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_STRING("1,2,3", node);
    ASSERT_EQUAL_NODE_RULE("row", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_STRING("1", node->head);
    ASSERT_EQUAL_NODE_RULE("num", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_STRING("2", node->head->next);
    ASSERT_EQUAL_NODE_RULE("num", node->head->next);

    TEST_ASSERT_NOT_NULL(node->head->next->next);
    TEST_ASSERT_EQUAL(node->tail, node->head->next->next);
    ASSERT_EQUAL_NODE_STRING("3", node->head->next->next);
    ASSERT_EQUAL_NODE_RULE("num", node->head->next->next);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_error_string(void) {
    TEST_ASSERT_EQUAL_STRING("InternalError", P4_GetErrorString(P4_InternalError));
    TEST_ASSERT_EQUAL_STRING("MatchError", P4_GetErrorString(P4_MatchError));
    TEST_ASSERT_EQUAL_STRING("NameError", P4_GetErrorString(P4_NameError));
    TEST_ASSERT_EQUAL_STRING("AdvanceError", P4_GetErrorString(P4_AdvanceError));
    TEST_ASSERT_EQUAL_STRING("MemoryError", P4_GetErrorString(P4_MemoryError));
    TEST_ASSERT_EQUAL_STRING("ValueError", P4_GetErrorString(P4_ValueError));
    TEST_ASSERT_EQUAL_STRING("KeyError", P4_GetErrorString(P4_KeyError));
    TEST_ASSERT_EQUAL_STRING("NullError", P4_GetErrorString(P4_NullError));
    TEST_ASSERT_EQUAL_STRING("StackError", P4_GetErrorString(P4_StackError));
    TEST_ASSERT_EQUAL_STRING("UnknownError", P4_GetErrorString(999999));
}

void test_source_slice(void) {
    const int R1 = 1;

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, "r1", "XXX", false));

    P4_String input = "YXXXY";
    P4_Source* source = P4_CreateSource(input, "r1");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetSourceSlice(source, 1, 4));
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("r1", node);
    ASSERT_EQUAL_NODE_STRING("XXX", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_lineno_offset(void) {
    const int R1 = 1;

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, "r1", "A\nBC\n", false));

    P4_Source* source = P4_CreateSource("A\nBC\n", "r1");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("r1", node);
    ASSERT_EQUAL_NODE_STRING("A\nBC\n", node);
    ASSERT_EQUAL_NODE_LINE_OFFSET(1, 1, 3, 1, node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);

}

void test_name(void) {
    const int R1 = 1;

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, "r1", "A", false));
    P4_Expression* expr = P4_GetGrammarRuleByName(grammar, "r1");
    TEST_ASSERT_EQUAL( expr, P4_GetGrammarRuleByName(grammar, "r1"));
    TEST_ASSERT_EQUAL( NULL, P4_GetGrammarRuleByName(grammar, "r0"));

    P4_DeleteGrammar(grammar);
}

static int my_inspect_refcnt = 0;
P4_Error my_inspect(P4_Node* node, void* userdata) {
    my_inspect_refcnt++;
    return P4_Ok;
}

void test_inspect(void) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = any any; any = .;");
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source = P4_CreateSource("XX", "entry");
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL( P4_Ok, P4_Parse(grammar, source));

    TEST_ASSERT_EQUAL( P4_Ok, P4_InspectSourceAst(P4_GetSourceAst(source), NULL, my_inspect));
    TEST_ASSERT_EQUAL( 3, my_inspect_refcnt); /* one for parent, two for children. */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

static int my_inspect_refcnt2 = 0;
P4_Error my_inspect2(P4_Node* node, void* userdata) {
    if (strcmp(node->rule_name, "entry") != 0)
        my_inspect_refcnt2++;
    return P4_Ok;
}

void test_inspect2(void) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = any any; any = .;");
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source = P4_CreateSource("XX", "entry");
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL( P4_Ok, P4_Parse(grammar, source));

    TEST_ASSERT_EQUAL( P4_Ok, P4_InspectSourceAst(P4_GetSourceAst(source), (void*)1, my_inspect2));
    TEST_ASSERT_EQUAL( 2, my_inspect_refcnt2); /* ignore parent. */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_acquire_source_ast(void) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = .;");
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source = P4_CreateSource("X", "entry");
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL( P4_Ok, P4_Parse(grammar, source));

    P4_Node* root = P4_AcquireSourceAst(source);
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_NULL(P4_GetSourceAst(source));

    P4_DeleteNode(root);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_maximum_recursion_limit);
    RUN_TEST(test_replace_grammar_rule);
    RUN_TEST(test_replace_grammar_rule_refreshing_references);
    RUN_TEST(test_join);
    RUN_TEST(test_error_string);
    RUN_TEST(test_source_slice);
    RUN_TEST(test_lineno_offset);
    RUN_TEST(test_name);
    RUN_TEST(test_inspect);
    RUN_TEST(test_inspect2);
    RUN_TEST(test_acquire_source_ast);

    return UNITY_END();
}
