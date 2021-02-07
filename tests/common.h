# ifndef P4_TEST_COMMON
# define P4_TEST_COMMON

#include "unity/src/unity.h"

#include "../peppapeg.h"

#define autofree __attribute__ ((cleanup (cleanup_freep)))

P4_PRIVATE(inline void)
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

# endif
