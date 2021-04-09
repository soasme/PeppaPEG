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
        P4_AddChoiceWithMembers(grammar, ENTRY, 2,
            P4_CreateLiteral("x", true),
            P4_CreateSequenceWithMembers(2,
                P4_CreateLiteral("-", true),
                P4_CreateReference(ENTRY)
            )
        )
    );

    P4_Source* source = P4_CreateSource("----------x", ENTRY);
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
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, ENTRY, "{{", true));
    TEST_ASSERT_EQUAL(P4_Ok, P4_ReplaceGrammarRule(grammar, ENTRY, P4_CreateLiteral("<%", true)));
    P4_Source* source = P4_CreateSource("<%", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));
    P4_Token* root = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(root);
    ASSERT_EQUAL_TOKEN_STRING("<%", root);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_replace_grammar_rule_refreshing_references(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddLiteral(grammar, REFERENCE, "{{", true));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddReference(grammar, ENTRY, REFERENCE));
    TEST_ASSERT_EQUAL(P4_Ok, P4_ReplaceGrammarRule(grammar, REFERENCE, P4_CreateLiteral("<%", true)));
    P4_Source* source = P4_CreateSource("<%", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* root = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(root);
    ASSERT_EQUAL_TOKEN_STRING("<%", root);

    P4_Expression* ref = P4_GetGrammarRule(grammar, ENTRY);
    TEST_ASSERT_EQUAL(ref->ref_expr, P4_GetGrammarRule(grammar, REFERENCE));

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_join(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    const int ROW = 1;
    const int NUM = 2;
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddJoin(grammar, ROW, ",", NUM));
    TEST_ASSERT_EQUAL(P4_Ok, P4_AddRange(grammar, NUM, '0', '9', 1));

    P4_Source* source = P4_CreateSource("1,2,3", ROW);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    ASSERT_EQUAL_TOKEN_STRING("1,2,3", token);
    ASSERT_EQUAL_TOKEN_RULE(ROW, token);

    TEST_ASSERT_NOT_NULL(token->head);
    ASSERT_EQUAL_TOKEN_STRING("1", token->head);
    ASSERT_EQUAL_TOKEN_RULE(NUM, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    ASSERT_EQUAL_TOKEN_STRING("2", token->head->next);
    ASSERT_EQUAL_TOKEN_RULE(NUM, token->head->next);

    TEST_ASSERT_NOT_NULL(token->head->next->next);
    TEST_ASSERT_EQUAL(token->tail, token->head->next->next);
    ASSERT_EQUAL_TOKEN_STRING("3", token->head->next->next);
    ASSERT_EQUAL_TOKEN_RULE(NUM, token->head->next->next);

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
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, R1, "XXX", false));

    P4_String input = "YXXXY";
    P4_Source* source = P4_CreateSource(input, R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_SetSourceSlice(source, 1, 4));
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    ASSERT_EQUAL_TOKEN_RULE(R1, token);
    ASSERT_EQUAL_TOKEN_STRING("XXX", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_lineno_offset(void) {
    const int R1 = 1;

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, R1, "A\nBC\n", false));

    P4_Source* source = P4_CreateSource("A\nBC\n", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    ASSERT_EQUAL_TOKEN_RULE(R1, token);
    ASSERT_EQUAL_TOKEN_STRING("A\nBC\n", token);
    ASSERT_EQUAL_TOKEN_LINE_OFFSET(1, 0, 2, 3, token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);

}

void test_name(void) {
    const int R1 = 1;

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL( P4_Ok, P4_AddLiteral(grammar, R1, "A", false));
    TEST_ASSERT_EQUAL( P4_Ok, P4_SetGrammarRuleName(grammar, R1, "R1"));
    TEST_ASSERT_EQUAL_STRING( "R1", P4_GetGrammarRuleName(grammar, R1));
    P4_Expression* expr = P4_GetGrammarRule(grammar, R1);
    TEST_ASSERT_EQUAL( expr, P4_GetGrammarRuleByName(grammar, "R1"));
    TEST_ASSERT_EQUAL( NULL, P4_GetGrammarRuleByName(grammar, "R0"));

    P4_DeleteGrammar(grammar);
}

static int my_inspect_refcnt = 0;
P4_Error my_inspect(P4_Token* token, void* userdata) {
    my_inspect_refcnt++;
    return P4_Ok;
}

void test_inspect(void) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = any any; any = .;");
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source = P4_CreateSource("XX", 1);
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL( P4_Ok, P4_Parse(grammar, source));

    TEST_ASSERT_EQUAL( P4_Ok, P4_InspectSourceAst(P4_GetSourceAst(source), NULL, my_inspect));
    TEST_ASSERT_EQUAL( 3, my_inspect_refcnt); /* one for parent, two for children. */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

static int my_inspect_refcnt2 = 0;
P4_Error my_inspect2(P4_Token* token, void* userdata) {
    if (token->rule_id != (P4_RuleID)userdata)
        my_inspect_refcnt2++;
    return P4_Ok;
}

void test_inspect2(void) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = any any; any = .;");
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source = P4_CreateSource("XX", 1);
    TEST_ASSERT_NOT_NULL(source);

    TEST_ASSERT_EQUAL( P4_Ok, P4_Parse(grammar, source));

    TEST_ASSERT_EQUAL( P4_Ok, P4_InspectSourceAst(P4_GetSourceAst(source), (void*)1, my_inspect2));
    TEST_ASSERT_EQUAL( 2, my_inspect_refcnt2); /* ignore parent. */

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

    return UNITY_END();
}
