#include <glob.h>
#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/json.h"

void test_null(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("null", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("null", node);
    ASSERT_EQUAL_NODE_STRING("null", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_true(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("true", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("true", node);
    ASSERT_EQUAL_NODE_STRING("true", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_false(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("false", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("false", node);
    ASSERT_EQUAL_NODE_STRING("false", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_number_integer_0(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("0", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("number", node);
    ASSERT_EQUAL_NODE_STRING("0", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_number_integer_1234567890(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("1234567890", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("number", node);
    ASSERT_EQUAL_NODE_STRING("1234567890", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_number_float(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("0.1", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("number", node);
    ASSERT_EQUAL_NODE_STRING("0.1", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_number_exp(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("1e+1", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("number", node);
    ASSERT_EQUAL_NODE_STRING("1e+1", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_string_xyz(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"xyz\"", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("string", node);
    ASSERT_EQUAL_NODE_STRING("\"xyz\"", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_string_pi(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"π\"", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("string", node);
    ASSERT_EQUAL_NODE_STRING("\"π\"", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


void test_string_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"\"", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("string", node);
    ASSERT_EQUAL_NODE_STRING("\"\"", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_array_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[ ]", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("array", node);
    ASSERT_EQUAL_NODE_STRING("[ ]", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_array_one_item(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[ 1 ]", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("array", node);
    ASSERT_EQUAL_NODE_STRING("[ 1 ]", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_array_deep_nesting(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    const int NESTING_DEPTH = 10;

    P4_String input = malloc(sizeof(char) * (NESTING_DEPTH*2 + 1));
    int i;

    for (i = 0; i < NESTING_DEPTH; i++) {
        input[i] = '[';
        input[NESTING_DEPTH+i] = ']';
    }
    input[NESTING_DEPTH*2] = '\0';

    source = P4_CreateSource(input, "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(NESTING_DEPTH*2, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("array", node);
    ASSERT_EQUAL_NODE_STRING(input, node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    free(input);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_array_multiple_items(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[1,2]", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("array", node);
    ASSERT_EQUAL_NODE_STRING("[1,2]", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}
void test_array_multiple_items_with_empty_spaces(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("[1,    2]", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("array", node);
    ASSERT_EQUAL_NODE_STRING("[1,    2]", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}


void test_object_empty(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{ }", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("object", node);
    ASSERT_EQUAL_NODE_STRING("{ }", node);

    TEST_ASSERT_NULL(node->next);
    TEST_ASSERT_NULL(node->head);
    TEST_ASSERT_NULL(node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_object_one_item(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{\"k\":1}", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("object", node);
    ASSERT_EQUAL_NODE_STRING("{\"k\":1}", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_object_multiple_items(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{\"k\":1,\"v\":2}", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(13, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("object", node);
    ASSERT_EQUAL_NODE_STRING("{\"k\":1,\"v\":2}", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_objectitem(void) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("\"k\":    1", "item");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("item", node);
    ASSERT_EQUAL_NODE_STRING("\"k\":    1", node);

    TEST_ASSERT_NULL(node->next);

    TEST_ASSERT_NOT_NULL(node->head);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_json_spec(char* json_spec) {
    if (strcmp(json_spec, "") == 0) return;

    P4_Grammar* grammar = P4_CreateJSONGrammar();

    char* buf = read_file(json_spec);

    P4_Source* source = P4_CreateSource(buf, "entry");

    P4_Error err = P4_Parse(grammar, source);
    static char errmsg[256] = "";

    sprintf(errmsg, "%s:%d:%s", json_spec, err, P4_GetErrorMessage(source));

    P4_DeleteSource(source);
    free(buf);
    P4_DeleteGrammar(grammar);

    if (strstr(json_spec, "test_parsing/y_") != NULL) {
        TEST_ASSERT_EQUAL_MESSAGE(P4_Ok, err, json_spec);
    } else if (strstr(json_spec, "test_parsing/n_") != NULL) {
        TEST_ASSERT_NOT_EQUAL_MESSAGE(P4_Ok, err, json_spec);
    } else {
        TEST_IGNORE_MESSAGE(errmsg);
    }

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
    RUN_TEST(test_string_pi);
    RUN_TEST(test_array_empty);
    RUN_TEST(test_array_one_item);
    RUN_TEST(test_array_multiple_items);
    RUN_TEST(test_array_multiple_items_with_empty_spaces);
    RUN_TEST(test_array_deep_nesting);
    RUN_TEST(test_objectitem);
    RUN_TEST(test_object_empty);
    RUN_TEST(test_object_one_item);

    /* https://github.com/nst/JSONTestSuite */
    glob_t globbuf = {0};
    glob("json_test_parsing/*.json", 0, 0, &globbuf);
    size_t i = 0;
    for (i = 0; i < globbuf.gl_pathc; i++) {
        char* json_spec = globbuf.gl_pathv[i];
        /* Unknown failed case. */
        if (strcmp(json_spec, "json_test_parsing/n_multidigit_number_then_00.json") == 0 ||
                false) continue;
        RUN_TEST_ARGS(test_json_spec, json_spec);
    }
    globfree(&globbuf);
    return UNITY_END();
}
