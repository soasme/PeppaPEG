#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/p4gen.h"

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

void test_repeat(void) {
    check_parse(P4_P4GenRepeat, "\"a\"*", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"+", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"?", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"{1,}", "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"{,1}", "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"{1,1}", "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenRepeat, "\"a\"{1}", "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_expression(void) {
    check_parse(P4_P4GenExpression, "\"a\"", "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    check_parse(P4_P4GenExpression, "[a-z]", "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    check_parse(P4_P4GenExpression, "s1", "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    check_parse(P4_P4GenExpression, "&\"a\"", "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenExpression, "!\"a\"", "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\" / \"b\"", "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\" \"b\"", "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenExpression, "var \"b\"", "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenExpression, "var name", "[{\"slice\":[0,8],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,8],\"type\":\"reference\"}]}]");
    check_parse(P4_P4GenExpression, "v1 v2 / v3 v4", "[{\"slice\":[0,13],\"type\":\"choice\",\"children\":[{\"slice\":[0,6],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[3,5],\"type\":\"reference\"}]},{\"slice\":[8,13],\"type\":\"sequence\",\"children\":[{\"slice\":[8,10],\"type\":\"reference\"},{\"slice\":[11,13],\"type\":\"reference\"}]}]}]");
    check_parse(P4_P4GenExpression, "v1 (v2/v3) v4", "[{\"slice\":[0,13],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[4,9],\"type\":\"choice\",\"children\":[{\"slice\":[4,6],\"type\":\"reference\"},{\"slice\":[7,9],\"type\":\"reference\"}]},{\"slice\":[11,13],\"type\":\"reference\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\"*", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\"+", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\"?", "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    check_parse(P4_P4GenExpression, "\"a\"{1,}", "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenExpression, "\"a\"{,1}", "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenExpression, "\"a\"{1,1}", "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    check_parse(P4_P4GenExpression, "\"a\"{1}", "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_rule_name(void) {
    check_parse(P4_P4GenRuleName, "a", "[{\"slice\":[0,1],\"type\":\"name\"}]");
    check_parse(P4_P4GenRuleName, "JsonEntry", "[{\"slice\":[0,9],\"type\":\"name\"}]");
    check_parse(P4_P4GenRuleName, "P4", "[{\"slice\":[0,2],\"type\":\"name\"}]");
    check_parse(P4_P4GenRuleName, "P4_P4Gen", "[{\"slice\":[0,8],\"type\":\"name\"}]");
    check_parse_failed(P4_P4GenRuleName, "4PEG", P4_MatchError);
}

void test_rule(void) {
    check_parse(P4_P4GenRule, "a = \"1\"", "[{\"slice\":[0,7],\"type\":\"rule\",\"children\":[{\"slice\":[0,1],\"type\":\"name\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenRule, "a = [1-9]", "[{\"slice\":[0,9],\"type\":\"rule\",\"children\":[{\"slice\":[0,1],\"type\":\"name\"},{\"slice\":[4,9],\"type\":\"range\",\"children\":[{\"slice\":[5,6],\"type\":\"char\"},{\"slice\":[7,8],\"type\":\"char\"}]}]}]");
    check_parse(P4_P4GenRule, "a = \"0x\" [1-9]", "[{\"slice\":[0,14],\"type\":\"rule\",\"children\":[{\"slice\":[0,1],\"type\":\"name\"},{\"slice\":[4,14],\"type\":\"sequence\",\"children\":[{\"slice\":[4,8],\"type\":\"literal\"},{\"slice\":[9,14],\"type\":\"range\",\"children\":[{\"slice\":[10,11],\"type\":\"char\"},{\"slice\":[12,13],\"type\":\"char\"}]}]}]}]");
    check_parse(P4_P4GenRule, "a = \"0x\"/[1-9]", "[{\"slice\":[0,14],\"type\":\"rule\",\"children\":[{\"slice\":[0,1],\"type\":\"name\"},{\"slice\":[4,14],\"type\":\"choice\",\"children\":[{\"slice\":[4,8],\"type\":\"literal\"},{\"slice\":[9,14],\"type\":\"range\",\"children\":[{\"slice\":[10,11],\"type\":\"char\"},{\"slice\":[12,13],\"type\":\"char\"}]}]}]}]");
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
    RUN_TEST(test_repeat);
    RUN_TEST(test_expression);
    RUN_TEST(test_rule_name);
    RUN_TEST(test_rule);

    return UNITY_END();
}
