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

void TEST_ASSERT_EQUAL_SLICE(P4_Slice* s, size_t i, size_t j) {
    TEST_ASSERT_EQUAL_UINT(i,   s->start.pos);
    TEST_ASSERT_EQUAL_UINT(j,   s->start.pos);
}

# define ASSERT_EQUAL_SLICE(s, i, j) do { \
    TEST_ASSERT_EQUAL_UINT_MESSAGE((i),   (s)->start.pos, "unexpected slice start"); \
    TEST_ASSERT_EQUAL_UINT_MESSAGE((j),   (s)->start.pos, "unexpected slice stop"); \
} while (0);

void TEST_ASSERT_EQUAL_TOKEN_STRING(P4_String s, P4_Token* token) {
    TEST_ASSERT_NOT_NULL(token);
    autofree P4_String tokenstr = P4_CopyTokenString(token);
    TEST_ASSERT_EQUAL_STRING(s, tokenstr);
}

# define ASSERT_EQUAL_TOKEN_STRING(s, token) do { \
    TEST_ASSERT_NOT_NULL((token)); \
    autofree P4_String tokenstr = P4_CopyTokenString((token)); \
    TEST_ASSERT_EQUAL_STRING((s), tokenstr); \
} while (0);

void TEST_ASSERT_EQUAL_TOKEN_RULE(P4_RuleID id, P4_Token* token) {
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_UINT(id, token->rule_id);
}

# define ASSERT_EQUAL_TOKEN_RULE(id, token) do { \
    TEST_ASSERT_NOT_NULL((token)); \
    TEST_ASSERT_EQUAL_UINT_MESSAGE((id), (token)->rule_id, "invalid token rule"); \
} while (0);


void TEST_ASSERT_EQUAL_TOKEN_LINE_OFFSET(size_t startline, size_t startoffset,
        size_t stopline, size_t stopoffset, P4_Token* token) {
    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_UINT(startline, token->slice.start.lineno);
    TEST_ASSERT_EQUAL_UINT(startoffset, token->slice.start.offset);
    TEST_ASSERT_EQUAL_UINT(stopline, token->slice.stop.lineno);
    TEST_ASSERT_EQUAL_UINT(stopoffset, token->slice.stop.offset);
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
