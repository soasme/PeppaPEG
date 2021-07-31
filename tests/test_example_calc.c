#include <glob.h>
#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/calc.h"

#define ASSERT_CALC(source_code, parse_code, r) do { \
    P4_Grammar*     grammar = P4_CreateCalcGrammar(); \
    TEST_ASSERT_NOT_NULL_MESSAGE(grammar, "peg rule should be valid peg code."); \
    P4_Expression*  entry = P4_GetGrammarRuleByName(grammar, "statement"); \
    TEST_ASSERT_NOT_NULL_MESSAGE(entry, "peg entry rule should created."); \
    P4_Source*      source  = P4_CreateSource((source_code), P4_CalcStatement); \
    TEST_ASSERT_EQUAL_MESSAGE( \
            (parse_code), P4_Parse(grammar, source), \
            "source code should be correctly parsed"); \
    if ( (parse_code) == P4_Ok ) { \
        P4_Node*   root = P4_GetSourceAst(source); \
        long result = 0; \
        TEST_ASSERT_EQUAL_MESSAGE( P4_Ok, P4_CalcEval(root, &result), "invalid eval"); \
        TEST_ASSERT_EQUAL_MESSAGE( (r), result, "invalid eval result" ); \
    } \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_eval(void) {
    ASSERT_CALC("1;", P4_Ok, 1);
    ASSERT_CALC("0;", P4_Ok, 0);
    ASSERT_CALC("-1;", P4_Ok, -1);
    ASSERT_CALC("1+1;", P4_Ok, 2);
    ASSERT_CALC("3-1-1;", P4_Ok, 1);
    ASSERT_CALC("2*3;", P4_Ok, 6);
    ASSERT_CALC("2*3+1;", P4_Ok, 7);
    ASSERT_CALC("2*3+1*2;", P4_Ok, 8);
    ASSERT_CALC("2*3+1*2;", P4_Ok, 8);
    ASSERT_CALC("2*(3+1)*2;", P4_Ok, 16);
    ASSERT_CALC("2*(3+1)/4*2;", P4_Ok, 4);
    ASSERT_CALC("2* -3  \t+1;", P4_Ok, -5);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_eval);
    return UNITY_END();
}
