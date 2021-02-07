# ifndef P4_TEST_COMMON
# define P4_TEST_COMMON

#include "unity/src/unity.h"

#include "../peppapeg.h"

void TEST_ASSERT_EQUAL_SLICE(P4_Slice* s, size_t i, size_t j) {
    TEST_ASSERT_EQUAL_UINT(i,   s->i);
    TEST_ASSERT_EQUAL_UINT(j,   s->j);
}

# endif
