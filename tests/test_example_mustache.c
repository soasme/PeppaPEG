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

    source = P4_CreateSource("{{xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(7, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    source = P4_CreateSource("{{ xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(9, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

P4_PRIVATE(void) test_partial(void) {
    P4_Grammar* grammar = P4_CreateMustacheGrammar();
    TEST_ASSERT_NOT_NULL(grammar);

    P4_Source* source;

    source = P4_CreateSource("{{>xyz}}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(8, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    source = P4_CreateSource("{{> xyz }}", P4_MustacheTag);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source));
    P4_DeleteSource(source);

    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_whitespace);
    RUN_TEST(test_variable);
    RUN_TEST(test_partial);

    return UNITY_END();
}
