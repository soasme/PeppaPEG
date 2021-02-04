#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

P4_PRIVATE(void) test_match_simple_literal(void) {
# define RULE_SIMPLE_LITERAL 1

    P4_Grammar* grammar = P4_CreateGrammar();
    P4_Expression* simplelit = P4_CreateLiteral("hello world", false);
    P4_AddGrammarRule(grammar, RULE_SIMPLE_LITERAL, simplelit);

    P4_Token* token = P4_Parse(grammar, RULE_SIMPLE_LITERAL, "hello world");

    TEST_ASSERT_NOT_NULL(token);

    P4_DeleteToken(token);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_simple_literal);

    return UNITY_END();
}

