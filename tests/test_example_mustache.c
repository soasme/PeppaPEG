#include "unity/src/unity.h"
#include "common.h"
#include "../examples/mustache.h"

P4_PRIVATE(void) test_whitespace(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource(" ", P4_MustacheWhitespace);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    source = P4_CreateSource("\t", P4_MustacheWhitespace);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(1, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_variable(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheVariable, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{ xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheVariable, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_partial(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{>xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustachePartial, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{> xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustachePartial, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_section_open(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{#xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionOpen, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{# xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));
    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionOpen, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_section_open_inverted(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{^xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionOpen, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{^ xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionOpen, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_section_close(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{/xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionClose, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{/ xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSectionClose, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_unescaped(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{&xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheUnescaped, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{& xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheUnescaped, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_triple_unescaped(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{{xyz}}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTripleUnescaped, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);
    P4_DeleteSource(source);

    source = P4_CreateSource("{{{ xyz }}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTripleUnescaped, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_comment(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;
    P4_Token* token;

    source = P4_CreateSource("{{!xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheComment, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    source = P4_CreateSource("{{! xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));

    token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheComment, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_set_delimiter_new_opener(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("<% %>", P4_MustacheNewOpener);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(2, P4_GetSourcePosition(source));

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_set_delimiter(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{=<% %>=}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    P4_Token* token = P4_GetSourceAst(source);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheTag, token);

    TEST_ASSERT_NOT_NULL(token->head);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheOpener, token->head);

    TEST_ASSERT_NOT_NULL(token->head->next);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheSetDelimiter, token->head->next);

    TEST_ASSERT_NOT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(P4_MustacheCloser, token->tail);

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

    return UNITY_END();
}
