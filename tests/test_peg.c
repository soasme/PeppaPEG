#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"


# define ASSERT_BAD_GRAMMAR(rule, err) do {\
    P4_Result(P4_GrammarPtr) result = P4_LoadGrammarResult((rule)); \
    const char* actual_err = P4_ResultUnwrapErr(P4_GrammarPtr)(&result); \
    TEST_ASSERT_EQUAL_STRING((err), actual_err); \
    P4_FREE((void*)actual_err); \
} while (0);

# define ASSERT_PEG_PARSE(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    if ((code) == P4_Ok) { \
        P4_Token* root = P4_GetSourceAst(source); \
        FILE *f = fopen("check.json","w"); \
        P4_JsonifySourceAst(grammar, f, root); \
        fclose(f); \
        P4_String s = read_file("check.json"); \
        TEST_ASSERT_EQUAL_STRING((output), s); \
        free(s); \
    } else { \
        TEST_ASSERT_EQUAL_STRING((output), source->errmsg); \
    } \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

#define ASSERT_EVAL_GRAMMAR(peg_rules, entry_name, source_code, parse_code, ast) do { \
    P4_Result(P4_GrammarPtr) grammar_res = P4_LoadGrammarResult((peg_rules)); \
    if (P4_ResultIsErr(P4_GrammarPtr)(&grammar_res)) { \
        const char* errmsg = P4_ResultUnwrapErr(P4_GrammarPtr)(&grammar_res); \
        TEST_ASSERT_EQUAL_STRING(ast, errmsg); \
        P4_FREE((char*)errmsg); \
        break; \
    }\
    P4_Grammar*     grammar = P4_ResultUnwrap(P4_GrammarPtr)(&grammar_res); \
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

void test_number(void) {
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "123", P4_Ok, "[{\"slice\":[0,3],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleNumber, "a", P4_MatchError, "expect number, line 1:0, char 0");
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
    ASSERT_PEG_PARSE(P4_PegRuleChar, "\"", P4_MatchError, "expect char, line 1:0, char 0");
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

    ASSERT_PEG_PARSE(P4_PegRuleRange, "[\\p{__}]", P4_MatchError, "expect char, line 1:1, char 1")
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
    ASSERT_PEG_PARSE(P4_PegRuleReference, "4PEG", P4_MatchError, "expect reference, line 1:0, char 0");
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
    ASSERT_PEG_PARSE(P4_PegRuleRuleName, "4PEG", P4_MatchError, "expect reference, line 1:0, char 0");
}

void test_rule_flag(void) {
    ASSERT_PEG_PARSE(P4_PegRuleDecorator, "@scoped", P4_Ok, "[{\"slice\":[0,7],\"type\":\"decorator\"}]");
    ASSERT_PEG_PARSE(P4_PegRuleDecorator, "@whatever", P4_MatchError, "expect decorator, line 1:0, char 0");
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

    ASSERT_PEG_PARSE(P4_PegRuleRule, "a = \"1\"", P4_MatchError, "expect rule, line 1:0, char 0");
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

void test_eval_literal(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\";", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"hello world\";", "R1", "hello world", P4_Ok, "[{\"slice\":[0,11],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"こんにちは世界\";", "R1", "こんにちは世界", P4_Ok, "[{\"slice\":[0,21],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"你好，世界\";", "R1", "你好，世界", P4_Ok, "[{\"slice\":[0,15],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"你好, World\";", "R1", "你好, World", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"نامهای\";", "R1", "نامهای", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"Peppa PEG 🐷\";", "R1", "Peppa PEG 🐷", P4_Ok, "[{\"slice\":[0,14],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\u{4f60}\\u{597d}, world\";", "R1", "你好, world", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\n\";", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\r\";", "R1", "\r", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\t\";", "R1", "\t", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"   \";", "R1", "   ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\";", "R1", "  ", P4_MatchError, "Literal should have at least one character.");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\u{110000}\";", "R1", "???", P4_MatchError, "Input has invalid character.");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\u{0}\";", "R1", "???", P4_MatchError, "Input has invalid character.");
}

void test_eval_insensitive(void) {
    ASSERT_EVAL_GRAMMAR("R1 = i\"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"a\";", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"hello world\";", "R1", "HELLO WORLD", P4_Ok, "[{\"slice\":[0,11],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"こんにちは世界\";", "R1", "こんにちは世界", P4_Ok, "[{\"slice\":[0,21],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"你好，世界\";", "R1", "你好，世界", P4_Ok, "[{\"slice\":[0,15],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"你好, World\";", "R1", "你好, WORLD", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"نامهای\";", "R1", "نامهای", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"Peppa PEG 🐷\";", "R1", "peppa peg 🐷", P4_Ok, "[{\"slice\":[0,14],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\u{4f60}\\u{597d}, world\";", "R1", "你好, WORLD", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"Hello Worìd\";", "R1", "HELLO WORÌD", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\n\";", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\r\";", "R1", "\r", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\t\";", "R1", "\t", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"   \";", "R1", "   ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\";", "R1", "  ", P4_MatchError, "Literal should have at least one character.");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\u{110000}\";", "R1", "???", P4_MatchError, "Input has invalid character.");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\u{0}\";", "R1", "???", P4_MatchError, "Input has invalid character.");
}

void test_eval_range(void) {
    ASSERT_EVAL_GRAMMAR("R1 = [0-9];", "R1", "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [a-z];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [A-Z];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{0001}-\\u{10ffff}];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [你-好];", "R1", "你", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
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
    ASSERT_EVAL_GRAMMAR("R1 = [9-0];", "R1", "0", P4_MatchError, "Range lower is greater than upper.");
    ASSERT_EVAL_GRAMMAR("R1 = [z-a];", "R1", "a", P4_MatchError, "Range lower is greater than upper.");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{10ffff}-\\u{0001}];", "R1", "a", P4_MatchError, "Range lower is greater than upper.");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{0}-\\u{10ffff}];", "R1", "a", P4_MatchError, "Range lower, upper and stride must be all non-zeros.");
    ASSERT_EVAL_GRAMMAR("R1 = [a-f..0];", "R1", "a", P4_MatchError, "Range lower, upper and stride must be all non-zeros.");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{1}-\\u{10ffff}..0];", "R1", "a", P4_MatchError, "Range lower, upper and stride must be all non-zeros.");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{1}-\\u{110000}];", "R1", "a", P4_MatchError, "Range lower and upper must be less than 0x10ffff.");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u{110000}-\\u{111111}];", "R1", "a", P4_MatchError, "Range lower and upper must be less than 0x10ffff.");
}

void test_eval_sequence(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" \"b\" \"c\";", "R1", "abc", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"0x\" [1-9];", "R1", "0x1", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = R2 R2;R2 = \"a\";", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"R2\"},"
        "{\"slice\":[1,2],\"type\":\"R2\"}"
    "]}]");
}

void test_eval_choice(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "c", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "d", P4_MatchError, "[]");
}

void test_eval_positive(void) {
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" \"apple\";", "R1", "apple", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"b\" \"apple\";", "R1", "beef", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "aPPLE", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "APPLE", P4_MatchError, "[]");
}

void test_eval_negative(void) {
    ASSERT_EVAL_GRAMMAR("R1 = !\"a\" [a-f];", "R1", "b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = !\"b\" [a-f];", "R1", "b", P4_MatchError, "[]");
}

void test_eval_repeat(void) {
    ASSERT_EVAL_GRAMMAR("R1 = (\"\\n\" / \"\\r\")+;", "R1", "\r\n\r\n", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "1A9F", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "FFFFFF", P4_Ok, "[{\"slice\":[0,6],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "HHHH", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"*;", "R1", "aaaaa", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"*;", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"+;", "R1", "aaaaa", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"+;", "R1", "", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"?;", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"?;", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "a", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "a", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aa", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3,2};", "R1", "", P4_PegError, "Repeat min is greater than max.");
}

void test_eval_reference(void) {
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
}

void test_eval_dot(void) {
    ASSERT_EVAL_GRAMMAR("R1 = .;", "R1", "好", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = .;", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"* !.;", "R1", "aaab", P4_MatchError, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"* !.;", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
}

void test_eval_comment(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"1\"# R1 = \"3\";\n;", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("# R1 = \"2\";\nR1 = \"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"1\";##", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
}

void test_eval_flags(void) {
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

void test_eval_bad_grammar(void) {
    ASSERT_BAD_GRAMMAR("R1 = \"a\"", "expect rule, line 1:0, char 0");
    ASSERT_BAD_GRAMMAR("R1 = R2;", "Reference name is not resolved.");
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

    RUN_TEST(test_eval_literal);
    RUN_TEST(test_eval_insensitive);
    RUN_TEST(test_eval_range);
    RUN_TEST(test_eval_sequence);
    RUN_TEST(test_eval_choice);
    RUN_TEST(test_eval_positive);
    RUN_TEST(test_eval_negative);
    RUN_TEST(test_eval_repeat);
    RUN_TEST(test_eval_reference);
    RUN_TEST(test_eval_dot);
    RUN_TEST(test_eval_comment);
    RUN_TEST(test_eval_flags);
    RUN_TEST(test_eval_bad_grammar);

    return UNITY_END();
}
