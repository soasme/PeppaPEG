#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

# define ENTRY 1

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
P4_PRIVATE(void) test_maximum_recursion_limit(void) {
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

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_maximum_recursion_limit);

    return UNITY_END();
}
