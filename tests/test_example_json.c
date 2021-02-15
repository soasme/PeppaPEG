#include "unity/src/unity.h"
#include "common.h"
#include "../examples/json.h"

P4_PRIVATE(void) test_null(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("null", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONNull, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("null", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_true(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("true", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONTrue, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("true", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_false(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("false", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONFalse, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("false", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_number_integer_0(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("0", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONNumber, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("0", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONIntegral, token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("0", token->head);

    TEST_ASSERT_EQUAL(token->head, token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_number_integer_1234567890(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("1234567890", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONNumber, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1234567890", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONIntegral, token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1234567890", token->head);

    TEST_ASSERT_EQUAL(token->head, token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_number_float(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("0.1", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONNumber, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("0.1", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONIntegral, token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("0", token->head);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONFractional, token->tail);
    TEST_ASSERT_EQUAL_TOKEN_STRING(".1", token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_number_exp(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("1e+1", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONNumber, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1e+1", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONIntegral, token->head);
    TEST_ASSERT_EQUAL_TOKEN_STRING("1", token->head);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONExponent, token->tail);
    TEST_ASSERT_EQUAL_TOKEN_STRING("e+1", token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_string_xyz(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"xyz\"", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONString, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("\"xyz\"", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_string_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"\"", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONString, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("\"\"", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_array_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[ ]", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONArray, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("[ ]", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_array_one_item(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[ 1 ]", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONArray, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("[ 1 ]", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_array_multiple_items(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[1,2]", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONArray, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("[1,2]", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_object_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{ }", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONObject, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("{ }", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_object_one_item(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{\"k\":1}", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONObject, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("{\"k\":1}", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_object_multiple_items(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{\"k\":1,\"v\":2}", P4_JSONEntry);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(13, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONObject, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("{\"k\":1,\"v\":2}", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_objectitem(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"k\":1", P4_JSONObjectItem);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_JSONObjectItem, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("\"k\":1", token);

    TEST_ASSERT_NULL(token->next);

    TEST_ASSERT_NOT_NULL(token->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_null);
    RUN_TEST(test_true);
    RUN_TEST(test_false);
    RUN_TEST(test_number_integer_0);
    RUN_TEST(test_number_integer_1234567890);
    RUN_TEST(test_number_float);
    RUN_TEST(test_number_exp);
    RUN_TEST(test_string_xyz);
    RUN_TEST(test_string_empty);
    RUN_TEST(test_array_empty);
    RUN_TEST(test_array_one_item);
    RUN_TEST(test_array_multiple_items);
    RUN_TEST(test_objectitem);
    RUN_TEST(test_object_empty);
    RUN_TEST(test_object_one_item);
    RUN_TEST(test_object_multiple_items);

    return UNITY_END();
}
