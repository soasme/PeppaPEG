# ifndef P4_TEST_COMMON
# define P4_TEST_COMMON

#include "unity/src/unity.h"

#include "../peppapeg.h"

#define autofree __attribute__ ((cleanup (cleanup_freep)))

static void
cleanup_freep (void *p)
{
  void **pp = (void **) p;
  if (*pp)
    free (*pp);
}

void TEST_ASSERT_EQUAL_SLICE(P4_Slice* s, size_t i, size_t j) {
    TEST_ASSERT_EQUAL_UINT(i,   s->i);
    TEST_ASSERT_EQUAL_UINT(j,   s->j);
}

void TEST_ASSERT_EQUAL_TOKEN_STRING(P4_String s, P4_Token* token) {
    TEST_ASSERT_NOT_NULL(token);
    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING(s, tokenstr);
}

void TEST_ASSERT_EQUAL_TOKEN_RULE(P4_RuleID id, P4_Token* token) {
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_NOT_NULL(token->expr);
    TEST_ASSERT_EQUAL_UINT(id, token->expr->id);
}

void TEST_ADD_WHITESPACE(P4_Grammar* grammar, P4_RuleID id) {
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoice(grammar, id, 3)
    );
    P4_Expression* ws = P4_GetGrammarRule(grammar, id);
    TEST_ASSERT_NOT_NULL(ws);
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(ws, 0, P4_CreateLiteral(" ", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(ws, 1, P4_CreateLiteral("\t", true))
    );
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_SetMember(ws, 2, P4_CreateLiteral("\n", true))
    );
    P4_SetExpressionFlag(ws, P4_FLAG_SPACED | P4_FLAG_LIFTED);
}

# endif
