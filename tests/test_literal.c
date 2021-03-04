#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

/*
 * Rules:
 *  R1 = i"Helo World";
 * Input:
 *  "Hello World"
 * Output:
 *   R1: "Hello World"
 */
void test_match_same_insensitive_literal_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", false)
    );

    P4_Source* source = P4_CreateSource("Hello World", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("Hello World", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = i"Helo World";
 * Input:
 *  "HeLlO WoRlD"
 * Output:
 *   R1: "HeLlO WoRlD"
 */
void test_match_insensitive_literal_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", false)
    );

    P4_Source* source = P4_CreateSource("HeLlO WoRlD", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("HeLlO WoRlD", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = i"Helo World";
 * Input:
 *  "HEll0 W0R1D"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_different_insensitive_literal_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", false)
    );

    P4_Source* source = P4_CreateSource("HEll0 W0R1D", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "Helo World";
 * Input:
 *  "Hello World"
 * Output:
 *   R1: "Hello World"
 */
void test_match_same_sensitive_literal_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", true)
    );

    P4_Source* source = P4_CreateSource("Hello World", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(11, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("Hello World", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "Helo World";
 * Input:
 *  "HELLO WORLD"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_insensitive_input_to_sensitive_literal_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", true)
    );

    P4_Source* source = P4_CreateSource("HELLO WORLD", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = "Helo World";
 * Input:
 *  "HELL0 W0R1D"
 * Error:
 *  P4_MatchError
 * Output:
 *  NULL
 */
void test_match_different_sensitive_literal_raise_match_error(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Hello World", true)
    );

    P4_Source* source = P4_CreateSource("HELL0 W0R1D", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_MatchError,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(0, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NULL(token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = i"你好, world";
 * Input:
 *  "你好, WORLD"
 * Output:
 *   R1: "你好, WORLD"
 */
void test_match_unicode_literal_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "你好, world", false)
    );

    P4_Source* source = P4_CreateSource("你好, WORLD", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(13, P4_GetSourcePosition(source));

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("你好, WORLD", token);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

/*
 * Rules:
 *  R1 = INSENSITIVE("Peppa 🐷");
 * Input:
 *  "PEPPA 🐷"
 * Output:
 *   R1: "PEPPA 🐷"
 */
void test_match_emoji_literal_successfully(void) {
# define R1 1
    P4_Grammar* grammar = P4_CreateGrammar();
    TEST_ASSERT_NOT_NULL(grammar);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddLiteral(grammar, R1, "Peppa 🐷", false)
    );

    P4_Source* source = P4_CreateSource("PEPPA 🐷", R1);
    TEST_ASSERT_NOT_NULL(source);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_Parse(grammar, source)
    );
    TEST_ASSERT_EQUAL(10, P4_GetSourcePosition(source)); /* "Peppa ": 6 + 🐷: 4. */

    P4_Token* token = P4_GetSourceAst(source);
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_TOKEN_RULE(R1, token);
    TEST_ASSERT_EQUAL_TOKEN_STRING("PEPPA 🐷", token);

    TEST_ASSERT_NULL(token->next);
    TEST_ASSERT_NULL(token->head);
    TEST_ASSERT_NULL(token->tail);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_match_same_insensitive_literal_successfully);
    RUN_TEST(test_match_insensitive_literal_successfully);
    RUN_TEST(test_match_different_insensitive_literal_raise_match_error);

    RUN_TEST(test_match_same_sensitive_literal_successfully);
    RUN_TEST(test_match_insensitive_input_to_sensitive_literal_raise_match_error);
    RUN_TEST(test_match_different_sensitive_literal_raise_match_error);

    RUN_TEST(test_match_unicode_literal_successfully);
    RUN_TEST(test_match_emoji_literal_successfully);

    return UNITY_END();
}
