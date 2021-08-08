#include "unity/src/unity.h"
#include "common.h"

void test_match_cut_raise_cut_error(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, "entry", 2,
            P4_CreateReference("r1"),
            P4_CreateReference("r2")
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "r1", 3,
            P4_CreateLiteral("[", true),
            P4_CreateCut(),
            P4_CreateLiteral("]", true)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r2", "null", true)
    );
    P4_Source* source = P4_CreateSource("[null]", "entry");
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_CutError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL_STRING("line 1:2, expect r1 (char ']')", P4_GetErrorMessage(source));
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_match_cut_skip_cut_error(void) {
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoiceWithMembers(grammar, "entry", 2,
            P4_CreateReference("r1"),
            P4_CreateReference("r2")
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddSequenceWithMembers(grammar, "r1", 3,
            P4_CreateLiteral("[", true),
            P4_CreateCut(),
            P4_CreateLiteral("]", true)
        )
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, "r2", "null", true)
    );

    P4_Source* source1 = P4_CreateSource("null", "entry");
    TEST_ASSERT_NOT_NULL(source1);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source1)
    );
    P4_DeleteSource(source1);

    P4_Source* source2 = P4_CreateSource("[]", "entry");
    TEST_ASSERT_NOT_NULL(source2);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source2)
    );
    P4_DeleteSource(source2);

    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_cut_raise_cut_error);
    RUN_TEST(test_match_cut_skip_cut_error);

    return UNITY_END();
}
