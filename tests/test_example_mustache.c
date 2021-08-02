#include "unity/src/unity.h"
#include "common.h"
#include "../examples/mustache.h"

void test_whitespace(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource(" ", "whitespace");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    source = P4_CreateSource("\t", "whitespace");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_variable(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("variable", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{ xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("variable", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_partial(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{>xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("partial", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{> xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("partial", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_section_open(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{#xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_open", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{# xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));
    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_open", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_section_open_inverted(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{^xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_open", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{^ xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_open", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_section_close(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{/xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_close", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{/ xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("section_close", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_unescaped(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{&xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("unescaped", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{& xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("unescaped", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_triple_unescaped(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{{xyz}}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("triple_unescaped", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{{ xyz }}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("triple_unescaped", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_comment(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Node* node;

    source = P4_CreateSource("{{!xyz}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("comment", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{! xyz }}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("comment", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

void test_set_delimiter_new_opener(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("<% %>", "new_opener");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_set_delimiter(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{=<% %>=}}", "tag");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);
    ASSERT_EQUAL_NODE_STRING("{{", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_RULE("set_delimiter", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    ASSERT_EQUAL_NODE_STRING("}}", node->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_set_delimiter_altered_grammar(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{=<% %>=}}<% x %>", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(18, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);
    ASSERT_EQUAL_NODE_STRING("{{=<% %>=}}", node);

    TEST_ASSERT_NOT_NULL(node->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->head);
    ASSERT_EQUAL_NODE_STRING("{{", node->head);

    TEST_ASSERT_NOT_NULL(node->head->next);
    ASSERT_EQUAL_NODE_STRING("=<% %>=", node->head->next);
    ASSERT_EQUAL_NODE_RULE("set_delimiter", node->head->next);

    TEST_ASSERT_NOT_NULL(node->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->tail);
    ASSERT_EQUAL_NODE_STRING("}}", node->tail);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_RULE("tag", node->next);
    ASSERT_EQUAL_NODE_STRING("<% x %>", node->next);

    TEST_ASSERT_NOT_NULL(node->next->head);
    ASSERT_EQUAL_NODE_RULE("opener", node->next->head);
    ASSERT_EQUAL_NODE_STRING("<%", node->next->head);

    TEST_ASSERT_NOT_NULL(node->next->head->next);
    ASSERT_EQUAL_NODE_RULE("variable", node->next->head->next);
    ASSERT_EQUAL_NODE_STRING("x", node->next->head->next);

    TEST_ASSERT_NOT_NULL(node->next->tail);
    ASSERT_EQUAL_NODE_RULE("closer", node->next->tail);
    ASSERT_EQUAL_NODE_STRING("%>", node->next->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_text_followed_by_newline(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("xyz\n", "text");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(4, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_text_followed_by_opener(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("xyz{{abc}}", "text");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);
    ASSERT_EQUAL_NODE_STRING("xyz", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_line_only_text(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("xyz", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(3, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);
    ASSERT_EQUAL_NODE_STRING("xyz", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_line_only_tag(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{xyz}}", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);
    ASSERT_EQUAL_NODE_STRING("{{xyz}}", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_line_mixing_text_and_tag(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("abc{{xyz}}", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);
    ASSERT_EQUAL_NODE_STRING("abc", node);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_RULE("tag", node->next);
    ASSERT_EQUAL_NODE_STRING("{{xyz}}", node->next);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_line_mixing_text_and_tag2(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{xyz}}abc", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("tag", node);
    ASSERT_EQUAL_NODE_STRING("{{xyz}}", node);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_RULE("text", node->next);
    ASSERT_EQUAL_NODE_STRING("abc", node->next);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_line_followed_by_newline(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("abc\n\n", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(5, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);
    ASSERT_EQUAL_NODE_STRING("abc\n", node);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_entry(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("abc\n{{xyz}}\n", "line");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(12, P4_GetSourcePosition(source));

    P4_Node* node = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(node);
    ASSERT_EQUAL_NODE_RULE("text", node);
    ASSERT_EQUAL_NODE_STRING("abc\n", node);

    TEST_ASSERT_NOT_NULL(node->next);
    ASSERT_EQUAL_NODE_RULE("tag", node->next);
    ASSERT_EQUAL_NODE_STRING("{{xyz}}", node->next);

    TEST_ASSERT_NOT_NULL(node->next->next);
    ASSERT_EQUAL_NODE_RULE("text", node->next->next);
    ASSERT_EQUAL_NODE_STRING("\n", node->next->next);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_whitespace);
    RUN_TEST(test_variable);
    RUN_TEST(test_partial);
    RUN_TEST(test_section_open);
    RUN_TEST(test_section_open_inverted);
    RUN_TEST(test_section_close);
    RUN_TEST(test_unescaped);
    RUN_TEST(test_triple_unescaped);
    RUN_TEST(test_comment);
    RUN_TEST(test_set_delimiter_new_opener);
    RUN_TEST(test_set_delimiter);
    RUN_TEST(test_set_delimiter_altered_grammar);
    RUN_TEST(test_text_followed_by_newline);
    RUN_TEST(test_text_followed_by_opener);
    RUN_TEST(test_line_only_text);
    RUN_TEST(test_line_only_tag);
    RUN_TEST(test_line_mixing_text_and_tag);
    RUN_TEST(test_line_mixing_text_and_tag2);
    RUN_TEST(test_line_followed_by_newline);
    RUN_TEST(test_entry);

    return UNITY_END();
}
