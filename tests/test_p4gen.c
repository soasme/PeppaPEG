#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/p4gen.c"

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
    TEST_ASSERT_EQUAL_STRING(output, s);
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

void test_string(void) {
    check_parse(P4_P4GenLiteral, "\"a\"", "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"A\"", "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"好\"", "[{\"slice\":[0,5],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"👌\"", "[{\"slice\":[0,6],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\t\"", "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\\"\"", "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\u0020\"", "[{\"slice\":[0,8],\"type\":\"literal\"}]");
}

void test_range(void) {
    check_parse(P4_P4GenRange, "[a-z]", "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    check_parse(P4_P4GenRange, "[1-9]", "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    check_parse(P4_P4GenRange, "[\\u0020-\\u0030]", "\
[{\"slice\":[0,15],\"type\":\"range\",\"children\":[\
{\"slice\":[1,7],\"type\":\"char\"},\
{\"slice\":[8,14],\"type\":\"char\"}]}]");
    check_parse(P4_P4GenRange, "[1-9..2]", "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"},\
{\"slice\":[6,7],\"type\":\"number\"}]}]");
}

void test_reference(void) {
    check_parse(P4_P4GenReference, "a", "[{\"slice\":[0,1],\"type\":\"reference\"}]");
    check_parse(P4_P4GenReference, "JsonEntry", "[{\"slice\":[0,9],\"type\":\"reference\"}]");
    check_parse(P4_P4GenReference, "P4", "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    check_parse(P4_P4GenReference, "P4_P4Gen", "[{\"slice\":[0,8],\"type\":\"reference\"}]");
    check_parse_failed(P4_P4GenReference, "4PEG", P4_MatchError);
}

void test_positive(void) {
    check_parse(P4_P4GenPositive, "&\"a\"", "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenPositive, "&&\"a\"", "[{\"slice\":[0,5],\"type\":\"positive\",\"children\":[{\"slice\":[1,5],\"type\":\"positive\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_negative(void) {
    check_parse(P4_P4GenNegative, "!\"a\"", "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenNegative, "!!\"a\"", "[{\"slice\":[0,5],\"type\":\"negative\",\"children\":[{\"slice\":[1,5],\"type\":\"negative\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_choice(void) {
    check_parse(P4_P4GenChoice, "\"a\"/\"b\"", "[{\"slice\":[0,7],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenChoice, "\"a\" / \"b\"", "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
}

void test_sequence(void) {
    check_parse(P4_P4GenSequence, "\"a\" \"b\"", "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_number);
    RUN_TEST(test_char);
    RUN_TEST(test_string);
    RUN_TEST(test_range);
    RUN_TEST(test_reference);
    RUN_TEST(test_positive);
    RUN_TEST(test_negative);
    RUN_TEST(test_choice);
    RUN_TEST(test_sequence);

    return UNITY_END();
}
