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

# endif
