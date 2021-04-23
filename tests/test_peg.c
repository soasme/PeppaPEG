#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

# define ASSERT_PEG_PARSE(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    P4_Token* root = P4_GetSourceAst(source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(grammar, f, root); \
    fclose(f); \
    P4_String s = read_file("check.json"); \
    TEST_ASSERT_EQUAL_STRING((output), s); \
    free(s); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

# define SETUP_EVAL(entry, input) \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source)); \
    P4_Token* root = P4_GetSourceAst(source);

# define TEARDOWN_EVAL() \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar);

void test_number(void) {
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "123", P4_Ok, "[{\"slice\":[0,3],\"type\":\"number\"}]");
}

void test_char(void) {
    ASSERT_PEG_PARSE(P4_PegRuleChar, "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "好", P4_Ok, "[{\"slice\":[0,3],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "👌", P4_Ok, "[{\"slice\":[0,4],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\\t", P4_Ok, "[{\"slice\":[0,2],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\\\"", P4_Ok, "[{\"slice\":[0,2],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\\u{20}", P4_Ok, "[{\"slice\":[0,6],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\\u{0020}", P4_Ok, "[{\"slice\":[0,8],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\\u{10ffff}", P4_Ok, "[{\"slice\":[0,10],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\"", P4_MatchError, "[]");
}

void test_literal(void) {
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"a\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"A\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"好\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"👌\"", P4_Ok, "[{\"slice\":[0,6],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"\\t\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"\\\"\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"\\u{20}\"", P4_Ok, "[{\"slice\":[0,8],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"\\u{0020}\"", P4_Ok, "[{\"slice\":[0,10],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"\\u{10ffff}\"", P4_Ok, "[{\"slice\":[0,12],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleLiteral, "\"hello world\"", P4_Ok, "[{\"slice\":[0,13],\"type\":\"literal\"}]");
}

void test_insensitive(void) {
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"A\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"好\"", P4_Ok, "[{\"slice\":[0,6],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,6],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"👌\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"\\t\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"\\\"\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"\\u{0020}\"", P4_Ok, "[{\"slice\":[0,11],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,11],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleInsensitiveLiteral, "i\"hello world\"", P4_Ok, "[{\"slice\":[0,14],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,14],\"type\":\"literal\"}]}]");
}

void test_range(void) {
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[a-z]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[1-9]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\u{0020}-\\u{0030}]", P4_Ok, "\
[{\"slice\":[0,19],\"type\":\"range\",\"children\":[\
{\"slice\":[1,9],\"type\":\"char\"},\
{\"slice\":[10,18],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[1-9..2]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"},\
{\"slice\":[6,7],\"type\":\"number\"}]}]");

    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{__}]", P4_MatchError, "[]")
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{C}]", P4_Ok, "\
[{\"slice\":[0,7],\"type\":\"range\",\"children\":[\
{\"slice\":[4,5],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{Co}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{Cs}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{Cf}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{Cc}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
}

void test_reference(void) {
    ASSERT_PEG_PARSE(P4_PegRuleReference, "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleReference, "JsonEntry", P4_Ok, "[{\"slice\":[0,9],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleReference, "P4", P4_Ok, "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleReference, "P4_PegRule", P4_Ok, "[{\"slice\":[0,10],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleReference, "4PEG", P4_MatchError, "[]");
}

void test_positive(void) {
    ASSERT_PEG_PARSE(P4_PegRulePositive, "&\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRulePositive, "&&\"a\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"positive\",\"children\":[{\"slice\":[1,5],\"type\":\"positive\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_negative(void) {
    ASSERT_PEG_PARSE(P4_PegRuleNegative, "!\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleNegative, "!!\"a\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"negative\",\"children\":[{\"slice\":[1,5],\"type\":\"negative\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_choice(void) {
    ASSERT_PEG_PARSE(P4_PegRuleChoice, "\"a\"/\"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleChoice, "\"a\" / \"b\"", P4_Ok, "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
}

void test_sequence(void) {
    ASSERT_PEG_PARSE(P4_PegRuleSequence, "\"a\" \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
}

void test_repeat(void) {
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"*", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"+", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"?", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"{1,}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"{,1}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"{1,1}", P4_Ok, "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleRepeat, "\"a\"{1}", P4_Ok, "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_dot(void) {
    ASSERT_PEG_PARSE(P4_PegRuleDot, ".", P4_Ok, "[{\"slice\":[0,1],\"type\":\"dot\"}]");
}

void test_expression(void) {
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "[a-z]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "s1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "&\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "!\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\" / \"b\"", P4_Ok, "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\" \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "var \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "var name", P4_Ok, "[{\"slice\":[0,8],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,8],\"type\":\"reference\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "v1 v2 / v3 v4", P4_Ok, "[{\"slice\":[0,13],\"type\":\"choice\",\"children\":[{\"slice\":[0,6],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[3,5],\"type\":\"reference\"}]},{\"slice\":[8,13],\"type\":\"sequence\",\"children\":[{\"slice\":[8,10],\"type\":\"reference\"},{\"slice\":[11,13],\"type\":\"reference\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "v1 (v2/v3) v4", P4_Ok, "[{\"slice\":[0,13],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[4,9],\"type\":\"choice\",\"children\":[{\"slice\":[4,6],\"type\":\"reference\"},{\"slice\":[7,9],\"type\":\"reference\"}]},{\"slice\":[11,13],\"type\":\"reference\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"*", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"+", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"?", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"{1,}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"{,1}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"{1,1}", P4_Ok, "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE(P4_PegRuleExpression, "\"a\"{1}", P4_Ok, "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_rule_name(void) {
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "JsonEntry", P4_Ok, "[{\"slice\":[0,9],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "P4", P4_Ok, "[{\"slice\":[0,2],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "P4_PegRule", P4_Ok, "[{\"slice\":[0,10],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "4PEG", P4_MatchError, "[]");
}

void test_rule_flag(void) {
    ASSERT_PEG_PARSE(P4_PegRuleDecorator, "@scoped", P4_Ok, "[{\"slice\":[0,7],\"type\":\"decorator\"}]");
}

void test_rule(void) {
    ASSERT_PEG_PARSE(P4_PegRuleRule, "a = \"1\";", P4_Ok, "["
        "{\"slice\":[0,8],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,7],\"type\":\"literal\"}"
        "]}"
    "]");
    ASSERT_PEG_PARSE(P4_PegRuleRule, "a = [1-9];", P4_Ok, "["
        "{\"slice\":[0,10],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,9],\"type\":\"range\",\"children\":["
                "{\"slice\":[5,6],\"type\":\"char\"},"
                "{\"slice\":[7,8],\"type\":\"char\"}]"
        "}]}"
    "]");

    ASSERT_PEG_PARSE(P4_PegRuleRule, "a = \"0x\" [1-9];", P4_Ok, "["
        "{\"slice\":[0,15],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,14],\"type\":\"sequence\",\"children\":["
                "{\"slice\":[4,8],\"type\":\"literal\"},"
                "{\"slice\":[9,14],\"type\":\"range\",\"children\":["
                    "{\"slice\":[10,11],\"type\":\"char\"},"
                    "{\"slice\":[12,13],\"type\":\"char\"}"
                "]}"
            "]}"
        "]}"
    "]");

    ASSERT_PEG_PARSE(P4_PegRuleRule, "a = \"0x\"/[1-9];", P4_Ok, "["
        "{\"slice\":[0,15],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,14],\"type\":\"choice\",\"children\":["
                "{\"slice\":[4,8],\"type\":\"literal\"},"
                "{\"slice\":[9,14],\"type\":\"range\",\"children\":["
                    "{\"slice\":[10,11],\"type\":\"char\"},"
                    "{\"slice\":[12,13],\"type\":\"char\"}"
                "]}"
            "]}"
        "]}"
    "]");

    ASSERT_PEG_PARSE(P4_PegRuleRule, "@lifted @squashed @spaced @nonterminal @scoped @tight\na = \"1\";", P4_Ok, "["
        "{\"slice\":[0,62],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,53],\"type\":\"decorators\",\"children\":["
                "{\"slice\":[0,7],\"type\":\"decorator\"},"
                "{\"slice\":[8,17],\"type\":\"decorator\"},"
                "{\"slice\":[18,25],\"type\":\"decorator\"},"
                "{\"slice\":[26,38],\"type\":\"decorator\"},"
                "{\"slice\":[39,46],\"type\":\"decorator\"},"
                "{\"slice\":[47,53],\"type\":\"decorator\"}"
            "]},"
            "{\"slice\":[54,55],\"type\":\"name\"},"
            "{\"slice\":[58,61],\"type\":\"literal\"}"
        "]}"
    "]");
}

void test_grammar(void) {
    ASSERT_PEG_PARSE(P4_PegGrammar,
        "one = \"1\";\n"
        "entry = one one;", P4_Ok,
        "["
            "{\"slice\":[0,27],\"type\":\"grammar\",\"children\":["
                "{\"slice\":[0,10],\"type\":\"rule\",\"children\":["
                    "{\"slice\":[0,3],\"type\":\"name\"},"
                    "{\"slice\":[6,9],\"type\":\"literal\"}"
                "]},"
                "{\"slice\":[11,27],\"type\":\"rule\",\"children\":["
                    "{\"slice\":[11,16],\"type\":\"name\"},"
                    "{\"slice\":[19,26],\"type\":\"sequence\",\"children\":["
                        "{\"slice\":[19,22],\"type\":\"reference\"},"
                        "{\"slice\":[23,26],\"type\":\"reference\"}"
                    "]}"
                "]}"
            "]}"
        "]"
    );
}

#define ASSERT_EVAL2(entry, input, expect_t, expect_value, expect_reason) do { \
        SETUP_EVAL((entry), (input)); \
        P4_EvalResult r = {0}; \
        if (root) P4_PegEval(root, &r); \
        TEST_ASSERT_EQUAL((expect_value), r.expect_t ); \
        TEST_ASSERT_EQUAL_STRING((expect_reason), r.reason ); \
        TEARDOWN_EVAL(); \
} while (0);

#define ASSERT_EVAL(entry, input, expect_t, expect) do { \
        SETUP_EVAL((entry), (input)); \
        expect_t value = 0; \
        if (root) P4_PegEval(root, &value); \
        TEST_ASSERT_EQUAL((expect), value); \
        TEARDOWN_EVAL(); \
} while (0);

void test_eval_flag(void) {
    ASSERT_EVAL2(P4_PegRuleDecorator, "@squashed", flag, P4_FLAG_SQUASHED, "");
    ASSERT_EVAL2(P4_PegRuleDecorator, "@scoped", flag, P4_FLAG_SCOPED, "");
    ASSERT_EVAL2(P4_PegRuleDecorator, "@spaced", flag, P4_FLAG_SPACED, "");
    ASSERT_EVAL2(P4_PegRuleDecorator, "@lifted", flag, P4_FLAG_LIFTED, "");
    ASSERT_EVAL2(P4_PegRuleDecorator, "@tight", flag, P4_FLAG_TIGHT, "");
    ASSERT_EVAL2(P4_PegRuleDecorator, "@nonterminal", flag, P4_FLAG_NON_TERMINAL, "");
}

void test_eval_flags(void) {
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@squashed @lifted", flag, P4_FLAG_SQUASHED | P4_FLAG_LIFTED, "");
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@squashed @lifted @squashed", flag, P4_FLAG_SQUASHED | P4_FLAG_LIFTED, "");
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@spaced", flag, P4_FLAG_SPACED, "");
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@spaced @squashed @lifted", flag,
            P4_FLAG_SPACED | P4_FLAG_SQUASHED | P4_FLAG_LIFTED, "");
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@nonterminal", flag, P4_FLAG_NON_TERMINAL, "");
    ASSERT_EVAL2(P4_PegRuleRuleDecorators, "@whatever", flag, 0, "");
}

void test_eval_num(void) {
    ASSERT_EVAL2(P4_PegRuleNumber, "0", size, 0, "");
    ASSERT_EVAL2(P4_PegRuleNumber, "1", size, 1, "");
    ASSERT_EVAL2(P4_PegRuleNumber, "12", size, 12, "");
    ASSERT_EVAL2(P4_PegRuleNumber, "999", size, 999, "");
    ASSERT_EVAL2(P4_PegRuleNumber, "10000", size, 10000, "");
}

void test_eval_char(void) {
    ASSERT_EVAL(P4_PegRuleChar, "a", P4_Rune, 'a');
    ASSERT_EVAL(P4_PegRuleChar, "A", P4_Rune, 'A');
    ASSERT_EVAL(P4_PegRuleChar, "\\b", P4_Rune, '\b');
    ASSERT_EVAL(P4_PegRuleChar, "\\t", P4_Rune, '\t');
    ASSERT_EVAL(P4_PegRuleChar, "\\n", P4_Rune, '\n');
    ASSERT_EVAL(P4_PegRuleChar, "\\f", P4_Rune, '\f');
    ASSERT_EVAL(P4_PegRuleChar, "\\r", P4_Rune, '\r');
    ASSERT_EVAL(P4_PegRuleChar, "\\\\", P4_Rune, '\\');
    ASSERT_EVAL(P4_PegRuleChar, "ç", P4_Rune, 0x00e7);
    ASSERT_EVAL(P4_PegRuleChar, "Ç", P4_Rune, 0x00C7);
    ASSERT_EVAL(P4_PegRuleChar, "你", P4_Rune, 0x4f60); /* https://www.compart.com/en/unicode/U+4F60 */
    ASSERT_EVAL(P4_PegRuleChar, "🐷", P4_Rune, 0x1f437); /* https://www.compart.com/en/unicode/U+1F437 */
    ASSERT_EVAL(P4_PegRuleChar, "\\u{4f60}", P4_Rune, 0x4f60);
    ASSERT_EVAL(P4_PegRuleChar, "\\u{a}", P4_Rune, '\n');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{A}", P4_Rune, '\n');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{41}", P4_Rune, 'A');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{61}", P4_Rune, 'a');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{0041}", P4_Rune, 'A');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{0061}", P4_Rune, 'a');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{000a}", P4_Rune, '\n');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{000A}", P4_Rune, '\n');
    ASSERT_EVAL(P4_PegRuleChar, "\\u{10ffff}", P4_Rune, 0x10ffff);
}

#define ASSERT_EVAL_LITERAL(entry, input, expect_lit, expect_sensitive) do { \
        SETUP_EVAL((entry), (input)); \
        P4_Expression* value = 0; \
        if (root) P4_PegEval(root, &value); \
        TEST_ASSERT_EQUAL(P4_Literal, (value)->kind); \
        TEST_ASSERT_EQUAL_STRING(expect_lit, (value)->literal); \
        TEST_ASSERT_EQUAL(expect_sensitive, (value)->sensitive); \
        P4_DeleteExpression(value); TEARDOWN_EVAL(); \
} while (0);

void test_eval_literal(void) {
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"a\"", "a", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"hello world\"", "hello world", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"你好, World\"", "你好, World", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"Peppa PEG 🐷\"", "Peppa PEG 🐷", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"\\u{4f60}\\u{597d}, world\"", "你好, world", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"\\n\"", "\n", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"\\r\"", "\r", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"\\t\"", "\t", true);
    ASSERT_EVAL_LITERAL(P4_PegRuleLiteral, "\"   \"", "   ", true);
}

void test_eval_insensitive(void) {
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"a\"", "a", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"hello world\"", "hello world", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"你好, World\"", "你好, World", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"Peppa PEG 🐷\"", "Peppa PEG 🐷", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"\\u{4f60}\\u{597d}, world\"", "你好, world", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"\\n\"", "\n", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"\\r\"", "\r", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"\\t\"", "\t", false);
    ASSERT_EVAL_LITERAL(P4_PegRuleInsensitiveLiteral, "i\"   \"", "   ", false);
}

#define ASSERT_EVAL_RANGE(entry, input, expect_lower, expect_upper) do { \
        SETUP_EVAL((entry), (input)); \
        P4_Expression* value = 0; \
        if (root) P4_PegEval(root, &value); \
        TEST_ASSERT_EQUAL(P4_Range, (value)->kind); \
        TEST_ASSERT_EQUAL(expect_lower, (value)->ranges[0].lower); \
        TEST_ASSERT_EQUAL(expect_upper, (value)->ranges[0].upper); \
        P4_DeleteExpression(value); TEARDOWN_EVAL(); \
} while (0);

void test_eval_range(void) {
    ASSERT_EVAL_RANGE(P4_PegRuleRange, "[0-9]", '0', '9');
    ASSERT_EVAL_RANGE(P4_PegRuleRange, "[a-z]", 'a', 'z');
    ASSERT_EVAL_RANGE(P4_PegRuleRange, "[A-Z]", 'A', 'Z');
    ASSERT_EVAL_RANGE(P4_PegRuleRange, "[\\u{0001}-\\u{10ffff}]", 0x1, 0x10ffff);
    ASSERT_EVAL_RANGE(P4_PegRuleRange, "[你-好]", 0x4f60, 0x597d);
}

#define ASSERT_EVAL_CONTAINER(entry, input, expect_kind, expect_count) do { \
    SETUP_EVAL((entry), (input)); \
    P4_Expression* value = 0; \
    if (root) P4_PegEval(root, &value); \
    TEST_ASSERT_EQUAL( (expect_kind), value->kind ); \
    TEST_ASSERT_EQUAL( (expect_count), value->count ); \
    TEST_ASSERT_NOT_NULL( value->members ); \
    size_t i = 0; \
    for (i = 0; i < value->count; i++) { \
        P4_Expression* member = P4_GetMember(value, i); \
        TEST_ASSERT_NOT_NULL( member ); \
    } \
    P4_DeleteExpression(value); \
    TEARDOWN_EVAL(); \
} while (0);

void test_eval_sequence(void) {
    ASSERT_EVAL_CONTAINER(P4_PegRuleSequence, "\"a\" \"b\" \"c\"", P4_Sequence, 3);
    ASSERT_EVAL_CONTAINER(P4_PegRuleSequence, "\"0x\" [1-9]", P4_Sequence, 2);
}

void test_eval_choice(void) {
    ASSERT_EVAL_CONTAINER(P4_PegRuleChoice, "\"a\" / \"b\" / \"c\"", P4_Choice, 3);
    ASSERT_EVAL_CONTAINER(P4_PegRuleChoice, "\"0x\" / [1-9]", P4_Choice, 2);
}

#define ASSERT_EVAL_LOOKAHEAD(entry, input, expect_kind) do { \
    SETUP_EVAL((entry), (input)); \
    P4_Expression* value = 0; \
    if (root) P4_PegEval(root, &value); \
    TEST_ASSERT_EQUAL( (expect_kind), value->kind ); \
    TEST_ASSERT_NOT_NULL( value->ref_expr ); \
    P4_DeleteExpression(value); \
    TEARDOWN_EVAL(); \
} while (0);

void test_eval_positive(void) {
    ASSERT_EVAL_LOOKAHEAD(P4_PegRulePositive, "& \"a\"", P4_Positive);
    ASSERT_EVAL_LOOKAHEAD(P4_PegRulePositive, "& [0-9]", P4_Positive);
}

void test_eval_negative(void) {
    ASSERT_EVAL_LOOKAHEAD(P4_PegRuleNegative, "! \"a\"", P4_Negative);
    ASSERT_EVAL_LOOKAHEAD(P4_PegRuleNegative, "! [0-9]", P4_Negative);
}

#define ASSERT_EVAL_REPEAT(entry, input, min, max) do { \
    SETUP_EVAL((entry), (input)); \
    P4_Expression* value = 0; \
    if (root) P4_PegEval(root, &value); \
    TEST_ASSERT_EQUAL( P4_Repeat, value->kind ); \
    TEST_ASSERT_NOT_NULL( value->repeat_expr ); \
    TEST_ASSERT_EQUAL( (min), value->repeat_min ); \
    TEST_ASSERT_EQUAL( (max), value->repeat_max ); \
    P4_DeleteExpression(value); \
    TEARDOWN_EVAL(); \
} while (0);

void test_eval_repeat(void) {
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"*", 0, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"?", 0, 1);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"+", 1, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"{2,3}", 2, 3);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"{2,}", 2, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"{,3}", 0, 3);
    ASSERT_EVAL_REPEAT(P4_PegRuleRepeat, "\"a\"{3}", 3, 3);
}

#define ASSERT_EVAL_REFERENCE(entry, input, expect_ref_id) do { \
    SETUP_EVAL((entry), (input)); \
    P4_Expression* value = 0; \
    if (root) P4_PegEval(root, &value); \
    TEST_ASSERT_EQUAL( P4_Reference, value->kind ); \
    TEST_ASSERT_EQUAL_STRING( (input), value->reference ); \
    TEST_ASSERT_EQUAL( (expect_ref_id), value->ref_id); \
    P4_DeleteExpression(value); \
    TEARDOWN_EVAL(); \
} while (0);

void test_eval_reference(void) {
    ASSERT_EVAL_REFERENCE(P4_PegRuleReference, "a", SIZE_MAX);
    ASSERT_EVAL_REFERENCE(P4_PegRuleReference, "xyz", SIZE_MAX);
    ASSERT_EVAL_REFERENCE(P4_PegRuleReference, "CONST", SIZE_MAX);
}

#define ASSERT_EVAL_GRAMMAR(peg_rules, entry_name, source_code, parse_code, ast) do { \
    P4_Grammar*     grammar = P4_LoadGrammar((peg_rules)); \
    TEST_ASSERT_NOT_NULL_MESSAGE(grammar, "peg rule should be valid peg code."); \
    P4_Expression*  entry = P4_GetGrammarRuleByName(grammar, (entry_name)); \
    TEST_ASSERT_NOT_NULL_MESSAGE(entry, "peg entry rule should created."); \
    P4_Source*      source  = P4_CreateSource((source_code), entry->id); \
    TEST_ASSERT_EQUAL_MESSAGE( \
            (parse_code), P4_Parse(grammar, source), \
            "source code should be correctly parsed"); \
    P4_Token*       ast_token = P4_GetSourceAst(source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(grammar, f, ast_token); \
    fclose(f); \
    P4_String s = read_file("check.json"); TEST_ASSERT_EQUAL_STRING((ast), s); free(s); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_eval_grammar(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"こんにちは世界\";", "R1", "こんにちは世界", P4_Ok, "[{\"slice\":[0,21],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"你好，世界\";", "R1", "你好，世界", P4_Ok, "[{\"slice\":[0,15],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"نامهای\";", "R1", "نامهای", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"hello\";", "R1", "hello", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"hello\";", "R1", "HELLO", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "2", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "3", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "4", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Cc}];", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Cc}];", "R1", "\b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{L}];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{L}]+;", "R1", "HELLOWORÌD", P4_Ok, "[{\"slice\":[0,11],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Lu}];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Lu}];", "R1", "a", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Ll}];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Ll}];", "R1", "A", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zl}];", "R1", "\xE2\x80\xA8", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zl}];", "R1", " ", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zp}];", "R1", "\xE2\x80\xA9", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zp}];", "R1", " ", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zs}];", "R1", "\xC2\xA0", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zs}];", "R1", " ", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Z}];", "R1", " ", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Nd}];", "R1", "０", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Nl}];", "R1", "Ⅵ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{No}];", "R1", "¼", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "０", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "Ⅵ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "¼", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = (\"\\n\" / \"\\r\")+;", "R1", "\r\n\r\n", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "1A9F", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "FFFFFF", P4_Ok, "[{\"slice\":[0,6],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "HHHH", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = !\"a\" [a-f];", "R1", "b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = !\"b\" [a-f];", "R1", "b", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" \"apple\";", "R1", "apple", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"b\" \"apple\";", "R1", "beef", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "aPPLE", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "APPLE", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = .;", "R1", "好", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"* !.;", "R1", "aaab", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"*;", "R1", "aaab", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"1\"# R1 = \"3\";\n;", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("# R1 = \"2\";\nR1 = \"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"1\";##", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");

    ASSERT_EVAL_GRAMMAR(
        "R1 = R2; "
        "R2 = \"1\";",

        "R1", "1", P4_Ok,

        "["
            "{\"slice\":[0,1],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R2\"}"
            "]}"
        "]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 R2;"
        "R2 = [1-1];",

        "R1", "11", P4_Ok,

        "["
            "{\"slice\":[0,2],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R2\"},"
                "{\"slice\":[1,2],\"type\":\"R2\"}"
            "]}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;"
        "R2 = \"1\";"
        "R3 = \"2\";",

        "R1", "1", P4_Ok,

        "["
            "{\"slice\":[0,1],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R2\"}"
            "]}"
        "]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;"
        "R2 = \"1\";"
        "R3 = \"2\";",

        "R1", "2", P4_Ok,

        "["
            "{\"slice\":[0,1],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R3\"}"
            "]}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@squashed\n"
        "R1 = R2 R2;"
        "R2 = \"X\";",

        "R1", "XX", P4_Ok,

        "["
            "{\"slice\":[0,2],\"type\":\"R1\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@lifted\n"
        "R1 = R2 R2;"
        "R2 = \"X\";",

        "R1", "XX", P4_Ok,

        "["
            "{\"slice\":[0,1],\"type\":\"R2\"},"
            "{\"slice\":[1,2],\"type\":\"R2\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@nonterminal\n"
        "R1 = R2+;"
        "R2 = \"X\";",

        "R1", "X", P4_Ok,

        "["
            "{\"slice\":[0,1],\"type\":\"R2\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@nonterminal\n"
        "R1 = R2+;"
        "R2 = \"X\";",

        "R1", "XX", P4_Ok,

        "["
            "{\"slice\":[0,2],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R2\"},"
                "{\"slice\":[1,2],\"type\":\"R2\"}"
            "]}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "R1 = [a-z]+;"

        "@spaced\n"
        "R2 = \" \" / \"\\t\" / \"\\r\" / \"\\n\";",

        "R1", "xxx", P4_Ok,

        "["
            "{\"slice\":[0,3],\"type\":\"R1\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "R1 = [a-z]+;"
        "@spaced R2 = \" \";"
        "@spaced R3 = \"\\t\";"
        "@spaced R4 = \"\\r\";"
        "@spaced R5 = \"\\n\";",

        "R1", "xxx", P4_Ok,

        "["
            "{\"slice\":[0,3],\"type\":\"R1\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "R1 = [a-z]+;"

        "@spaced @lifted\n"
        "R2 = \" \" / \"\\t\" / \"\\r\" / \"\\n\";",

        "R1", "x   x x\t\t\nx", P4_Ok,

        "["
            "{\"slice\":[0,11],\"type\":\"R1\"}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@tight\n"
        "R1 = [a-z]{4};"

        "@spaced @lifted\n"
        "R2 = \" \" / \"\\t\" / \"\\r\" / \"\\n\";",

        "R1", "x   x x\t\t\nx", P4_MatchError, "[]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@tight\n"
        "R1 = R2 R3;"
        "R2 = [a-z]{2};"
        "@scoped\n"
        "R3 = [a-z]{2};"
        "@spaced @lifted\n"
        "R4 = \" \" / \"\\t\" / \"\\r\" / \"\\n\";",

        "R1", "xxxx", P4_Ok, "["
            "{\"slice\":[0,4],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,2],\"type\":\"R2\"},"
                "{\"slice\":[2,4],\"type\":\"R3\"}"
            "]}"
        "]"
    );

    ASSERT_EVAL_GRAMMAR(
        "@tight\n"
        "R1 = R2 R3;"
        "R2 = [a-z]{2};"
        "@scoped\n"
        "R3 = [a-z]{2};"
        "@spaced @lifted\n"
        "R4 = \" \" / \"\\t\" / \"\\r\" / \"\\n\";",

        "R1", "xxx  x", P4_Ok, "["
            "{\"slice\":[0,6],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,2],\"type\":\"R2\"},"
                "{\"slice\":[2,6],\"type\":\"R3\"}"
            "]}"
        "]"
    );

}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_number);
    RUN_TEST(test_char);
    RUN_TEST(test_literal);
    RUN_TEST(test_insensitive);
    RUN_TEST(test_range);
    RUN_TEST(test_reference);
    RUN_TEST(test_positive);
    RUN_TEST(test_negative);
    RUN_TEST(test_choice);
    RUN_TEST(test_sequence);
    RUN_TEST(test_repeat);
    RUN_TEST(test_dot);
    RUN_TEST(test_expression);
    RUN_TEST(test_rule_name);
    RUN_TEST(test_rule_flag);
    RUN_TEST(test_rule);
    RUN_TEST(test_grammar);

    RUN_TEST(test_eval_flag);
    RUN_TEST(test_eval_flags);
    RUN_TEST(test_eval_num);
    RUN_TEST(test_eval_char);
    RUN_TEST(test_eval_literal);
    RUN_TEST(test_eval_insensitive);
    RUN_TEST(test_eval_range);
    RUN_TEST(test_eval_sequence);
    RUN_TEST(test_eval_choice);
    RUN_TEST(test_eval_positive);
    RUN_TEST(test_eval_negative);
    RUN_TEST(test_eval_repeat);
    RUN_TEST(test_eval_reference);
    RUN_TEST(test_eval_grammar);

    return UNITY_END();
}
