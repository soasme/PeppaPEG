#include "unity/src/unity.h"
#include "common.h"


/*
 * Rules:
 *  ENTRY = START END
 *  START = DEFREF("EOF", "EOF" / "EOT")
 *  END   = BACKREF("EOF")
 * Input:
 *  "EOTEOT"
 * Output:
 *  ENTRY:
 *    START: "EOT"
 *    END: "EOT"
 */
P4_PRIVATE(void) test_match_back_reference_successfully(void) {
    TEST_IGNORE_MESSAGE("backref not implementated");

# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    /* TODO: ADD BACKREF grammar rules. */

    P4_Source* source = P4_CreateSource("EOTEOT", ENTRY);
    TEST_ASSERT_NOT_NULL(source);

    /* TODO: TEST BACKREF */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = START END
 *  START = DEFREF("EOF", "EOF" / "EOT")
 *  END   = BACKREF("EOF")
 * Input:
 *  "EOTEOF"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
P4_PRIVATE(void) test_match_back_reference_latter_not_match(void) {
    TEST_IGNORE_MESSAGE("backref not implementated");

    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    /* TODO: ADD BACKREF grammar rules. */

    P4_Source* source = P4_CreateSource("EOTEOT", ENTRY);
    TEST_ASSERT_NOT_NULL(source);

    /* TODO: TEST BACKREF */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  ENTRY = START END
 *  START = DEFREF("EOF", "EOF" / "EOT")
 *  END   = BACKREF("EOF")
 * Input:
 *  "EOFEOT"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
P4_PRIVATE(void) test_match_back_reference_former_not_match(void) {
    TEST_IGNORE_MESSAGE("backref not implementated");

# define ENTRY  1
# define R1     2
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    /* TODO: ADD BACKREF grammar rules. */

    P4_Source* source = P4_CreateSource("EOTEOT", ENTRY);
    TEST_ASSERT_NOT_NULL(source);

    /* TODO: TEST BACKREF */

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_back_reference_successfully);
    RUN_TEST(test_match_back_reference_former_not_match);
    RUN_TEST(test_match_back_reference_latter_not_match);

    return UNITY_END();
}
