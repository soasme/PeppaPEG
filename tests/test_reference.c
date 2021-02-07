#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  ENTRY = R1
 *  R1 = "HELLO"
 * Input:
 *  "HELLO"
 * Output:
 *   ENTRY: "HELLO"
 */
P4_PRIVATE(void) test_match_reference_successfully(void) {
# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddReference(grammar, ENTRY, R1)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "HELLO", true)
    );

    P4_Source* source = P4_CreateSource("HELLO", ENTRY);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL(token->expr, P4_GetGrammarRule(grammar, ENTRY));
    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING("HELLO", tokenstr);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_reference_successfully);

    return UNITY_END();
}
