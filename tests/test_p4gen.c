#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"
#include "../p4gen.c"

void check_parse_failed(P4_RuleID entry, P4_String input, P4_Error err) {
    P4_Grammar* grammar = P4_CreateP4GenGrammar();
    P4_Source* source = P4_CreateSource(input, entry);
    TEST_ASSERT_EQUAL(err, P4_Parse(grammar, source));
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void check_parse(P4_RuleID entry, P4_String input, P4_String output) {
    P4_Grammar* grammar = P4_CreateP4GenGrammar();
    P4_Source* source = P4_CreateSource(input, entry);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    P4_Token* root = P4_GetSourceAst(source);
    FILE *f = fopen("check.json","w");
    P4_JsonifySourceAst(f, root, P4_P4GenKindToName);
    fclose(f);
    P4_String s = read_file("check.json");
    TEST_ASSERT_EQUAL_STRING(s, output);
    free(s);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void test_number(void) {
    check_parse(P4_P4GenNumber, "0", "[{\"slice\":[0,1],\"type\":\"number\"}]");
    check_parse(P4_P4GenNumber, "1", "[{\"slice\":[0,1],\"type\":\"number\"}]");
    check_parse(P4_P4GenNumber, "123", "[{\"slice\":[0,3],\"type\":\"number\"}]");
}

void test_char(void) {
    check_parse(P4_P4GenChar, "a", "[{\"slice\":[0,1],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "A", "[{\"slice\":[0,1],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "好", "[{\"slice\":[0,3],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "👌", "[{\"slice\":[0,4],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "\\t", "[{\"slice\":[0,2],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "\\\"", "[{\"slice\":[0,2],\"type\":\"char\"}]");
    check_parse(P4_P4GenChar, "\\u0020", "[{\"slice\":[0,6],\"type\":\"char\"}]");
    check_parse_failed(P4_P4GenChar, "\"", P4_MatchError);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_number);
    RUN_TEST(test_char);

    return UNITY_END();
}
