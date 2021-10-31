#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *   expr = atom < add < mul;
 *   atom = [0-9];
 *   add = "+";
 *   mul = "*";
 *
 * Input:
 *   "1+2*3*4"
 * Output:
 *   expr(1+2*3*4):
 *     atom(1)
 *     add
 *     expr(2*3*4):
 *       expr(2*3):
 *         atom(2)
 *         mul
 *         atom(3)
 *       mul
 *       atom(4)
 */
void test_match_precedence_1(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddPrecedenceWithMembers(grammar, "expr", 3,
            P4_CreateReference("atom"),
            P4_CreateReference("add"),
            P4_CreateReference("mul")
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddRange(grammar, "atom", '0', '9', 1)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "add", "+", true)
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "mul", "*", true)
    );

    P4_Source* source = P4_CreateSource("1+2*3*4", "expr");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_precedence_1);

    return UNITY_END();
}
