#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/toml.h"

# define ASSERT_TOML(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreateTomlGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    P4_Node* root = P4_GetSourceAst(source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(grammar, f, root); \
    fclose(f); \
    P4_String s = read_file("check.json"); \
    printf("%s\n%s\n", input, s); \
    free(s); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_valid(void) {
    ASSERT_TOML("toml", "abc = true # comment", P4_Ok, "[]");
    ASSERT_TOML("toml",
        "abc = true\n"
        "xyz = false", P4_Ok, "[]");
    ASSERT_TOML("toml",
        "a.b.c = true\n"
        "x.y.z = false", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = \"xyz\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = \"\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = \"a\\u0031\\U00000032\\n\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "\"abc\" = \"\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "\"a\\u0031c\" = \"\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "a.\"b\".c = \"\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = ''", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 'a\\u0031\\U00000032\\n'", P4_Ok, "[]");
    ASSERT_TOML("toml", "'abc' = ''", P4_Ok, "[]");
    ASSERT_TOML("toml", "a.'b'.c = ''", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 00:00:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 00:00:00.000000", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00.0000", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01t00:00:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01t00:00:00.0000", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01 00:00:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01 00:00:00.0000", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00Z", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00.0000Z", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00+12:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 2000-01-01T00:00:00.0000+12:00", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = []", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = [ # comment\n ]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = [true]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = [true,false]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = [true, false]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = [ true, false ]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = { abc=true, xyz=false }", P4_Ok, "[]");
    ASSERT_TOML("toml", "[abc]", P4_Ok, "[]");
    ASSERT_TOML("toml", "[ a.\"b\".c ]", P4_Ok, "[]");
    ASSERT_TOML("toml", "[[abc]]", P4_Ok, "[]");
    ASSERT_TOML("toml", "[[ a.\"b\".c ]]", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 0", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 1", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 123_456_789", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = +123_456_789", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = -123_456_789", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 0x123", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 0b0001", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 0o123", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = 1.0", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = -1.0", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = '''abc'''", P4_Ok, "[]");
    /* ASSERT_TOML("toml", "abc = '''abc'''''", P4_Ok, "[]"); */
    ASSERT_TOML("toml", "abc = '''a'b'c'''", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = \"\"\"abc\"\"\"", P4_Ok, "[]");
    ASSERT_TOML("toml", "abc = \"\"\"a\"b\"c\"\"\"", P4_Ok, "[]");
    /* ASSERT_TOML("toml", "abc = \"\"\"abc\"\"\"\"\"", P4_Ok, "[]"); */
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_valid);
    return UNITY_END();
}
