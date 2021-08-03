# ifndef P4_TEST_COMMON
# define P4_TEST_COMMON

#include "unity/src/unity.h"
#include "unity/src/unity_internals.h"

#include "../peppapeg.h"

#define autofree __attribute__ ((cleanup (cleanup_freep)))

static void
cleanup_freep (void *p)
{
  void **pp = (void **) p;
  if (*pp)
    free (*pp);
}

# define ASSERT_EQUAL_SLICE(s, i, j) do { \
    TEST_ASSERT_EQUAL_UINT_MESSAGE((i),   (s)->start.pos, "unexpected slice start"); \
    TEST_ASSERT_EQUAL_UINT_MESSAGE((j),   (s)->start.pos, "unexpected slice stop"); \
} while (0);

# define ASSERT_EQUAL_NODE_STRING(s, node) do { \
    TEST_ASSERT_NOT_NULL((node)); \
    autofree P4_String nodestr = P4_CopyNodeString((node)); \
    TEST_ASSERT_EQUAL_STRING((s), nodestr); \
} while (0);

# define ASSERT_EQUAL_NODE_RULE(name, node) do { \
    TEST_ASSERT_NOT_NULL((node)); \
    TEST_ASSERT_EQUAL_STRING_MESSAGE((name), (node)->rule_name, "invalid node rule"); \
} while (0);

# define ASSERT_EQUAL_NODE_LINE_OFFSET(startline, startoffset, stopline, stopoffset, node) do { \
    TEST_ASSERT_NOT_NULL(node); \
    TEST_ASSERT_EQUAL_UINT(startline, node->slice.start.lineno); \
    TEST_ASSERT_EQUAL_UINT(startoffset, node->slice.start.offset); \
    TEST_ASSERT_EQUAL_UINT(stopline, node->slice.stop.lineno); \
    TEST_ASSERT_EQUAL_UINT(stopoffset, node->slice.stop.offset); \
} while (0);

void TEST_ADD_WHITESPACE(P4_Grammar* grammar, P4_String name) {
    TEST_ASSERT_EQUAL(
        P4_Ok,
        P4_AddChoice((grammar), (name), 3)
    );
    P4_Expression* ws = P4_GetGrammarRule((grammar), (name));
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

char* read_file(const char* path) {
    FILE* f = fopen (path, "rb");
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek (f, 0, SEEK_SET);
    char* buf = calloc(length+2, sizeof(char));
    TEST_ASSERT_EQUAL(length, fread(buf, sizeof(char), length, f));
    buf[length] = '\0';
    fclose(f);
    return buf;
}

#define RUN_TEST_ARGS(TestFunc, ...) \
{ \
    UNITY_NEW_TEST(#TestFunc) \
    if (TEST_PROTECT()) \
    { \
        setUp(); \
        TestFunc(__VA_ARGS__); \
    } \
    if (TEST_PROTECT() && (!TEST_IS_IGNORED)) \
        tearDown(); \
    UnityConcludeTest(); \
}

# endif
