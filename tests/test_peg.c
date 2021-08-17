#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"


# define ASSERT_BAD_GRAMMAR(rule, err) do {\
    P4_Result* result = &(P4_Result){0}; \
    TEST_ASSERT_NOT_EQUAL(P4_Ok, P4_LoadGrammarResult((rule), result)); \
    TEST_ASSERT_NULL(result->grammar); \
    TEST_ASSERT_EQUAL_STRING((err), result->errmsg); \
} while (0);

#define ASSERT_EVAL_GRAMMAR(peg_rules, entry_name, source_code, parse_code, ast) do { \
    P4_Grammar*     grammar = P4_LoadGrammar((peg_rules)); \
    TEST_ASSERT_NOT_NULL_MESSAGE(grammar, "peg rule should be valid peg code."); \
    P4_Expression*  entry = P4_GetGrammarRule(grammar, (entry_name)); \
    TEST_ASSERT_NOT_NULL_MESSAGE(entry, "peg entry rule should created."); \
    P4_Source*      source  = P4_CreateSource((source_code), (entry_name)); \
    TEST_ASSERT_EQUAL_MESSAGE( \
            (parse_code), P4_Parse(grammar, source), \
            "source code should be correctly parsed"); \
    if ((parse_code) != P4_Ok) { \
        TEST_ASSERT_EQUAL_STRING((ast), P4_GetErrorMessage(source)); \
        P4_DeleteSource(source); \
        P4_DeleteGrammar(grammar); \
        break; \
    } \
    P4_Node*       ast_node = P4_GetSourceAst(source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(f, ast_node, NULL); \
    fclose(f); \
    P4_String s = read_file("check.json"); TEST_ASSERT_EQUAL_STRING((ast), s); free(s); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

# define ASSERT_PEG_PARSE(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    if ((code) == P4_Ok) { \
        P4_Node* root = P4_GetSourceAst(source); \
        FILE *f = fopen("check.json","w"); \
        P4_JsonifySourceAst(f, root, NULL); \
        fclose(f); \
        P4_String s = read_file("check.json"); \
        TEST_ASSERT_EQUAL_STRING((output), s); \
        free(s); \
    } else { \
        TEST_ASSERT_EQUAL_STRING((output), P4_GetErrorMessage(source)); \
    } \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_number(void) {
    ASSERT_PEG_PARSE("number", "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE("number", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE("number", "123", P4_Ok, "[{\"slice\":[0,3],\"type\":\"number\"}]");
    ASSERT_PEG_PARSE("number", "a", P4_MatchError, "line 1:1, expect number");
}

void test_char(void) {
    ASSERT_PEG_PARSE("char", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "好", P4_Ok, "[{\"slice\":[0,3],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "👌", P4_Ok, "[{\"slice\":[0,4],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "\\t", P4_Ok, "[{\"slice\":[0,2],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "\\\"", P4_Ok, "[{\"slice\":[0,2],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "\\u0020", P4_Ok, "[{\"slice\":[0,6],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "\\U0010ffff", P4_Ok, "[{\"slice\":[0,10],\"type\":\"char\"}]");
    ASSERT_PEG_PARSE("char", "\"", P4_MatchError, "line 1:1, expect char");
}

void test_literal(void) {
    ASSERT_PEG_PARSE("literal", "\"a\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"A\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"好\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"👌\"", P4_Ok, "[{\"slice\":[0,6],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"\\t\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"\\\"\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"\\u0020\"", P4_Ok, "[{\"slice\":[0,8],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"\\U0010ffff\"", P4_Ok, "[{\"slice\":[0,12],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("literal", "\"hello world\"", P4_Ok, "[{\"slice\":[0,13],\"type\":\"literal\"}]");
}

void test_insensitive(void) {
    ASSERT_PEG_PARSE("insensitive", "i\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"A\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"好\"", P4_Ok, "[{\"slice\":[0,6],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,6],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"👌\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"\\t\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"\\\"\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"\\u0020\"", P4_Ok, "[{\"slice\":[0,9],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,9],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("insensitive", "i\"hello world\"", P4_Ok, "[{\"slice\":[0,14],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,14],\"type\":\"literal\"}]}]");
}

void test_range(void) {
    ASSERT_PEG_PARSE("range", "[a-z]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE("range", "[1-9]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE("range", "[\\u0020-\\u0030]", P4_Ok, "\
[{\"slice\":[0,15],\"type\":\"range\",\"children\":[\
{\"slice\":[1,7],\"type\":\"char\"},\
{\"slice\":[8,14],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE("range", "[1-9..2]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"},\
{\"slice\":[6,7],\"type\":\"number\"}]}]");

    ASSERT_PEG_PARSE("range", "[]", P4_CutError, "line 1:2, expect range")
    ASSERT_PEG_PARSE("range", "[\"a\"]", P4_CutError, "line 1:2, expect range")
    ASSERT_PEG_PARSE("range", "[\\p{__}]", P4_CutError, "line 1:5, expect range_category")
    ASSERT_PEG_PARSE("range", "[\\p{C}]", P4_Ok, "\
[{\"slice\":[0,7],\"type\":\"range\",\"children\":[\
{\"slice\":[4,5],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE("range", "[\\p{Co}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE("range", "[\\p{Cs}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE("range", "[\\p{Cf}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
    ASSERT_PEG_PARSE("range", "[\\p{Cc}]", P4_Ok, "\
[{\"slice\":[0,8],\"type\":\"range\",\"children\":[\
{\"slice\":[4,6],\"type\":\"range_category\"}]}]");
# ifdef ENABLE_UNISTR
    ASSERT_PEG_PARSE("range", "[\\p{White space}]", P4_Ok, "\
[{\"slice\":[0,17],\"type\":\"range\",\"children\":[\
{\"slice\":[4,15],\"type\":\"range_category\"}]}]");
# endif
}

void test_reference(void) {
    ASSERT_PEG_PARSE("reference", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE("reference", "JsonEntry", P4_Ok, "[{\"slice\":[0,9],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE("reference", "P4", P4_Ok, "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE("reference", "P4_PegRule", P4_Ok, "[{\"slice\":[0,10],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE("reference", "4PEG", P4_MatchError, "line 1:1, expect reference");
}

void test_back_reference(void) {
    ASSERT_PEG_PARSE("back_reference", "\\1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"back_reference\",\"children\":[{\"slice\":[1,2],\"type\":\"number\"}]}]");
    ASSERT_PEG_PARSE("back_reference", "\\a", P4_CutError, "line 1:2, expect number");
}

void test_positive(void) {
    ASSERT_PEG_PARSE("positive", "&\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("positive", "&&\"a\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"positive\",\"children\":[{\"slice\":[1,5],\"type\":\"positive\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_negative(void) {
    ASSERT_PEG_PARSE("negative", "!\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("negative", "!!\"a\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"negative\",\"children\":[{\"slice\":[1,5],\"type\":\"negative\",\"children\":[{\"slice\":[2,5],\"type\":\"literal\"}]}]}]");
}

void test_choice(void) {
    ASSERT_PEG_PARSE("choice", "\"a\"/\"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("choice", "\"a\" / \"b\"", P4_Ok, "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
}

void test_sequence(void) {
    ASSERT_PEG_PARSE("sequence", "\"a\" \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
}

void test_repeat(void) {
    ASSERT_PEG_PARSE("repeat", "\"a\"*", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"+", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"?", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"{1,}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"{,1}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"{1,1}", P4_Ok, "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("repeat", "\"a\"{1}", P4_Ok, "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_dot(void) {
    ASSERT_PEG_PARSE("dot", ".", P4_Ok, "[{\"slice\":[0,1],\"type\":\"dot\"}]");
}

void test_expression(void) {
    ASSERT_PEG_PARSE("expression", "\"a\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    ASSERT_PEG_PARSE("expression", "[a-z]", P4_Ok, "\
[{\"slice\":[0,5],\"type\":\"range\",\"children\":[\
{\"slice\":[1,2],\"type\":\"char\"},\
{\"slice\":[3,4],\"type\":\"char\"}]}]");
    ASSERT_PEG_PARSE("expression", "s1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"reference\"}]");
    ASSERT_PEG_PARSE("expression", "&\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"positive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("expression", "!\"a\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"negative\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\" / \"b\"", P4_Ok, "[{\"slice\":[0,9],\"type\":\"choice\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,9],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\" \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("expression", "var \"b\"", P4_Ok, "[{\"slice\":[0,7],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,7],\"type\":\"literal\"}]}]");
    ASSERT_PEG_PARSE("expression", "var name", P4_Ok, "[{\"slice\":[0,8],\"type\":\"sequence\",\"children\":[{\"slice\":[0,3],\"type\":\"reference\"},{\"slice\":[4,8],\"type\":\"reference\"}]}]");
    ASSERT_PEG_PARSE("expression", "v1 v2 / v3 v4", P4_Ok, "[{\"slice\":[0,13],\"type\":\"choice\",\"children\":[{\"slice\":[0,6],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[3,5],\"type\":\"reference\"}]},{\"slice\":[8,13],\"type\":\"sequence\",\"children\":[{\"slice\":[8,10],\"type\":\"reference\"},{\"slice\":[11,13],\"type\":\"reference\"}]}]}]");
    ASSERT_PEG_PARSE("expression", "v1 (v2/v3) v4", P4_Ok, "[{\"slice\":[0,13],\"type\":\"sequence\",\"children\":[{\"slice\":[0,2],\"type\":\"reference\"},{\"slice\":[4,9],\"type\":\"choice\",\"children\":[{\"slice\":[4,6],\"type\":\"reference\"},{\"slice\":[7,9],\"type\":\"reference\"}]},{\"slice\":[11,13],\"type\":\"reference\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"*", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zeroormore\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"+", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"onceormore\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"?", P4_Ok, "[{\"slice\":[0,4],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,4],\"type\":\"zerooronce\"}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"{1,}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmin\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"{,1}", P4_Ok, "[{\"slice\":[0,7],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,7],\"type\":\"repeatmax\",\"children\":[{\"slice\":[5,6],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"{1,1}", P4_Ok, "[{\"slice\":[0,8],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,8],\"type\":\"repeatminmax\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"},{\"slice\":[6,7],\"type\":\"number\"}]}]}]");
    ASSERT_PEG_PARSE("expression", "\"a\"{1}", P4_Ok, "[{\"slice\":[0,6],\"type\":\"repeat\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[3,6],\"type\":\"repeatexact\",\"children\":[{\"slice\":[4,5],\"type\":\"number\"}]}]}]");
}

void test_rule_name(void) {
    ASSERT_PEG_PARSE("name", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE("name", "JsonEntry", P4_Ok, "[{\"slice\":[0,9],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE("name", "P4", P4_Ok, "[{\"slice\":[0,2],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE("name", "P4_PegRule", P4_Ok, "[{\"slice\":[0,10],\"type\":\"name\"}]");
    ASSERT_PEG_PARSE("name", "4PEG", P4_MatchError, "line 1:1, expect reference");
}

void test_rule_flag(void) {
    ASSERT_PEG_PARSE("decorator", "@scoped", P4_Ok, "[{\"slice\":[0,7],\"type\":\"decorator\"}]");
    ASSERT_PEG_PARSE("decorator", "@whatever", P4_CutError, "line 1:2, expect decorator");
}

void test_rule(void) {
    ASSERT_PEG_PARSE("rule", "a = \"1\";", P4_Ok, "["
        "{\"slice\":[0,8],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,7],\"type\":\"literal\"}"
        "]}"
    "]");
    ASSERT_PEG_PARSE("rule", "a = [1-9];", P4_Ok, "["
        "{\"slice\":[0,10],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,9],\"type\":\"range\",\"children\":["
                "{\"slice\":[5,6],\"type\":\"char\"},"
                "{\"slice\":[7,8],\"type\":\"char\"}]"
        "}]}"
    "]");

    ASSERT_PEG_PARSE("rule", "a = \"0x\" [1-9];", P4_Ok, "["
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

    ASSERT_PEG_PARSE("rule", "a = \"0x\"/[1-9];", P4_Ok, "["
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

    ASSERT_PEG_PARSE("rule", "@lifted @squashed @spaced @nonterminal @scoped @tight\na = \"1\";", P4_Ok, "["
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

    ASSERT_PEG_PARSE("rule", "a = \"1\"", P4_CutError, "line 1:8, expect rule (char ';')");
}

void test_cut(void) {
    /* ASSERT_PEG_PARSE("grammar", "a=[\x1];\n", P4_CutError, ""); */
}

void test_grammar(void) {
    ASSERT_PEG_PARSE("grammar",
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
    ASSERT_PEG_PARSE("grammar", "a = \"1\";\nb = \"2\"", P4_CutError, "line 2:8, expect rule (char ';')");
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
    ASSERT_EVAL_GRAMMAR("R1 = \"\\x48\\x65\\x6c\\x6c\\x6f, world\";", "R1", "Hello, world", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\u4f60\\u597d, world\";", "R1", "你好, world", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    /* ASSERT_EVAL_GRAMMAR("R1 = \"\\xe4\\xbd\\xa0\\xe5\\xa5\\xbd, world\";", "R1", "你好, world", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]"); */ /* Not sure if this one should be the expected behavior. */
    ASSERT_EVAL_GRAMMAR("R1 = \"\\n\";", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\r\";", "R1", "\r", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"\\t\";", "R1", "\t", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"   \";", "R1", "   ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
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
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\x48\\x65\\x6c\\x6c\\x6f, world\";", "R1", "HELLO, WORLD", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\u4f60\\u597d, world\";", "R1", "你好, WORLD", P4_Ok, "[{\"slice\":[0,13],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"Hello Worìd\";", "R1", "HELLO WORÌD", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\n\";", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\r\";", "R1", "\r", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"\\t\";", "R1", "\t", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"   \";", "R1", "   ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" i\\0;", "R1", "aA", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" i\\0;", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
}

void test_eval_range(void) {
    ASSERT_EVAL_GRAMMAR("R1 = [0-9];", "R1", "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [a-z];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [A-Z];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\x30-\\x39];", "R1", "0", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\x41-\\x5A];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\x61-\\x7A];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\u0001-\\U0010ffff];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [你-好];", "R1", "你", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "2", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "3", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [1-9..2];", "R1", "4", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Cc}];", "R1", "\n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Cc}];", "R1", "\b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{L}];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{L}]+;", "R1", "HELLOWORÌD", P4_Ok, "[{\"slice\":[0,11],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Lu}];", "R1", "A", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Lu}];", "R1", "a", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Ll}];", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Ll}];", "R1", "A", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zl}];", "R1", "\xE2\x80\xA8", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zl}];", "R1", " ", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zp}];", "R1", "\xE2\x80\xA9", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zp}];", "R1", " ", P4_MatchError, "line 1:1, expect R1");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zs}];", "R1", "\xC2\xA0", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Zs}];", "R1", " ", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Z}];", "R1", " ", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Nd}];", "R1", "０", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{Nl}];", "R1", "Ⅵ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{No}];", "R1", "¼", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "０", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "Ⅵ", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = [\\p{N}];", "R1", "¼", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
}

void test_eval_sequence(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" \"b\" \"c\";", "R1", "abc", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"0x\" [1-9];", "R1", "0x1", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = R2 R2;R2 = \"a\";", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"R2\"},"
        "{\"slice\":[1,2],\"type\":\"R2\"}"
    "]}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" \"b\" \"c\";", "R1", "ab", P4_MatchError, "line 1:3, expect R1 (char 'c')");
}

void test_eval_choice(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "c", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\" / \"b\" / \"c\";", "R1", "d", P4_MatchError, "line 1:1, expect R1");
}

void test_eval_positive(void) {
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" \"apple\";", "R1", "apple", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"b\" \"apple\";", "R1", "beef", P4_MatchError, "line 1:1, expect R1 (len 5)");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "aPPLE", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = &\"a\" i\"apple\";", "R1", "APPLE", P4_MatchError, "line 1:1, expect R1 (char 'a')");
}

void test_eval_negative(void) {
    ASSERT_EVAL_GRAMMAR("R1 = !\"a\" [a-f];", "R1", "b", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = !\"b\" [a-f];", "R1", "b", P4_MatchError, "line 1:1, expect R1");
}

void test_eval_cut(void) {
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "[]", P4_Ok,
        "[{\"slice\":[0,2],\"type\":\"R1\",\"children\":[{\"slice\":[0,2],\"type\":\"R2\"}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "null", P4_Ok,
        "[{\"slice\":[0,4],\"type\":\"R1\",\"children\":[{\"slice\":[0,4],\"type\":\"R3\"}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "[", P4_CutError, "line 1:2, expect R2 (char ']')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "1", P4_MatchError, "line 1:1, expect R1"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "[null]", P4_CutError, "line 1:2, expect R2 (char ']')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut R3 \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "[null", P4_CutError, "line 1:6, expect R2 (char ']')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut R08 \"]\";\n"
        "R3 = \"null\";\n"
        "R08 = [0-8];\n",
        "R1", "[9", P4_CutError, "line 1:2, expect R08"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut R08 \"]\";\n"
        "R3 = \"null\";\n"
        "R08 = [0-8];\n",
        "R1", "[8", P4_CutError, "line 1:3, expect R2 (char ']')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut R08 \"]\";\n"
        "R3 = \"null\";\n"
        "R08 = [0-8];\n",
        "R1", "[8]", P4_Ok,
        "[{\"slice\":[0,3],\"type\":\"R1\",\"children\":[{\"slice\":[0,3],\"type\":\"R2\",\"children\":[{\"slice\":[1,2],\"type\":\"R08\"}]}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2 / R3;\n"
        "R2 = \"[\" @cut [0-8] \"]\";\n"
        "R3 = \"null\";\n",
        "R1", "[9", P4_CutError, "line 1:2, expect R2"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut \"]\" / \"null\";\n",
        "R1", "[", P4_CutError, "line 1:2, expect R1 (char ']')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2*;"
        "R2 = \"1\" @cut \"2\";",
        "R1", "121", P4_CutError, "line 1:4, expect R2 (char '2')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2*;"
        "R2 = \"1\" @cut \"2\";",
        "R1", "121", P4_CutError, "line 1:4, expect R2 (char '2')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R2*;"
        "R2 = R12 / R21;"
        "R12 = \"1\" @cut \"2\";"
        "R21 = \"2\" @cut \"1\";",
        "R1", "122", P4_CutError, "line 1:4, expect R21 (char '1')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" !R21 [0-9]+ \"]\";"
        "R21 = \"2\" @cut \"1\";",
        "R1", "[2]", P4_Ok,
        "[{\"slice\":[0,3],\"type\":\"R1\"}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" R1_Inner \"]\";"
        "R1_Inner = !R21 [0-9]+;"
        "R21 = \"2\" @cut \"1\";",
        "R1", "[21]", P4_MatchError,
        "line 1:2, expect R1_Inner"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut R1_Inner+ \"]\";"
        "R1_Inner = \"1\" / \"2\";",
        "R1", "[1]", P4_Ok,
        "[{\"slice\":[0,3],\"type\":\"R1\",\"children\":[{\"slice\":[1,2],\"type\":\"R1_Inner\"}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut (RB / RP) . \"]\";"
        "RB = \"{\" @cut \"}\";"
        "RP = \"(\" \")\";",
        "R1", "[{]", P4_CutError,
        "line 1:3, expect RB (char '}')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut (RB / RP) . \"]\";"
        "RB = \"{\" @cut \"}\";"
        "RP = \"(\" \")\";",
        "R1", "[{}.]", P4_Ok,
        "[{\"slice\":[0,5],\"type\":\"R1\",\"children\":[{\"slice\":[1,3],\"type\":\"RB\"}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut (RB / RP) (!\"]\" .) \"]\";"
        "RB = \"{\" @cut \"}\";"
        "RP = \"(\" \")\";",
        "R1", "[()]", P4_CutError,
        "line 1:4, expect R1"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = \"[\" @cut (RB / RP) (!\"]\" .) \"]\";"
        "RB = \"{\" @cut \"}\";"
        "RP = \"(\" \")\";",
        "R1", "[()A]", P4_Ok,
        "[{\"slice\":[0,5],\"type\":\"R1\",\"children\":[{\"slice\":[1,3],\"type\":\"RP\"}]}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = &(\"a\" @cut \"b\") [a-z]{3};",
        "R1", "zzz", P4_MatchError,
        "line 1:1, expect R1 (char 'a')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = &(\"a\" @cut \"b\") [a-z]{3};",
        "R1", "abz", P4_Ok,
        "[{\"slice\":[0,3],\"type\":\"R1\"}]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = !(\"a\" @cut \"b\") [a-z]{3};",
        "R1", "abz", P4_MatchError,
        "line 1:1, expect R1"
    );

}

void test_eval_repeat(void) {
    ASSERT_EVAL_GRAMMAR("R1 = (\"\\n\" / \"\\r\")+;", "R1", "\r\n\r\n", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "1A9F", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "FFFFFF", P4_Ok, "[{\"slice\":[0,6],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "HHHH", P4_MatchError, "line 1:1, expect R1 (insufficient repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = ([0-9] / [a-f] / [A-F])+;", "R1", "", P4_MatchError, "line 1:1, expect R1 (at least 1 repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"*;", "R1", "aaaaa", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"*;", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"+;", "R1", "aaaaa", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"+;", "R1", "", P4_MatchError, "line 1:1, expect R1 (at least 1 repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"?;", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"?;", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "a", P4_MatchError, "line 1:2, expect R1 (at least 2 repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "a", P4_MatchError, "line 1:2, expect R1 (at least 2 repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{2,};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "", P4_Ok, "[]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aa", P4_Ok, "[{\"slice\":[0,2],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{,3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aa", P4_MatchError, "line 1:3, expect R1 (at least 3 repetitions)");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"{3};", "R1", "aaaa", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
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

void test_eval_back_reference(void) {
    ASSERT_EVAL_GRAMMAR(
        "R1 = R23 \\0;"
        "R23 = \"2\" / \"3\";",
        "R1", "22", P4_Ok,
        "["
            "{\"slice\":[0,2],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R23\"},"
                "{\"slice\":[1,2],\"type\":\"R23\"}"
            "]}"
        "]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R23 \\0;"
        "R23 = \"2\" / \"3\";",
        "R1", "23", P4_MatchError,
        "line 1:2, expect R1 (char '2')"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = (R2 \\0) \\0;"
        "R2 = \"2\" / \"3\";",
        "R1", "2222", P4_Ok,
        "["
            "{\"slice\":[0,4],\"type\":\"R1\",\"children\":["
                "{\"slice\":[0,1],\"type\":\"R2\"},"
                "{\"slice\":[1,2],\"type\":\"R2\"}"
            "]}"
        "]"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R23 \\10;"
        "R23 = \"2\" / \"3\";",
        "R1", "2222", P4_IndexError,
        "line 1:2, expect R1 (\\10 out reached)"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R23 \\1;"
        "R23 = \"2\" / \"3\";",
        "R1", "2222", P4_IndexError,
        "line 1:2, expect R1 (\\1 out reached)"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = R23 \\0 \\1;"
        "R23 = \"2\" / \"3\";",
        "R1", "223", P4_PegError,
        "line 1:3, expect R1 (backref point to backref)"
    );
    ASSERT_EVAL_GRAMMAR(
        "R1 = (\"2\" / \"3\") \\0 \\0;"
        "R23 = \"2\" / \"3\";",
        "R1", "333", P4_Ok,
        "[{\"slice\":[0,3],\"type\":\"R1\"}]"
    );
}

void test_eval_dot(void) {
    ASSERT_EVAL_GRAMMAR("R1 = .;", "R1", "好", P4_Ok, "[{\"slice\":[0,3],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = .;", "R1", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"* \"b\";", "R1", "aaab", P4_Ok, "[{\"slice\":[0,4],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"a\"* !.;", "R1", "aaab", P4_MatchError, "line 1:4, expect R1");
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

        "R1", "x   x x\t\t\nx", P4_MatchError, "line 1:2, expect R1 (insufficient repetitions)"
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

void test_eval_bad_grammar(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = \"a\"",
        "CutError: grammar syntax error: line 1:9, expect rule (char ';')."
    );
    ASSERT_BAD_GRAMMAR(
        "R1 = \"\\x3\";",
        "CutError: grammar syntax error: line 1:10, expect two_hexdigits (insufficient repetitions)."
    );
    ASSERT_BAD_GRAMMAR(
        "R1 = \"\\u123z\";",
        "CutError: grammar syntax error: line 1:12, expect four_hexdigits (insufficient repetitions)."
    );
    ASSERT_BAD_GRAMMAR(
        "R1 = \"\\U123a123z\";",
        "CutError: grammar syntax error: line 1:16, expect eight_hexdigits (insufficient repetitions)."
    );
}

void test_eval_bad_grammar_decorator(void) {
    ASSERT_BAD_GRAMMAR(
        "@some_random_decorator R1 = \"a\";",
        "CutError: grammar syntax error: line 1:2, expect decorator."
    );
}

void test_eval_bad_grammar_literal(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = \";",
        "CutError: grammar syntax error: line 1:8, expect literal (char '\"')."
    );
    ASSERT_BAD_GRAMMAR(
        "R1 = \"\";",
        "PegError: literal rule should have at least one character. char 5-7: \"\"."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = i\";",
        "CutError: grammar syntax error: line 1:9, expect literal (char '\"')."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = i\"\";",
        "PegError: literal rule should have at least one character. char 6-8: \"\"."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = \"\\U00110000\";",
        "PegError: char 0 is invalid. char 5-17: \"\\U00110000\"."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = \"\\u0000\";",
        "PegError: char 0 is invalid. char 5-13: \"\\u0000\"."
    );
}

void test_eval_bad_grammar_range(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = [];",
        "CutError: grammar syntax error: line 1:7, expect range."
    );

    /* lower > upper. */

    ASSERT_BAD_GRAMMAR(
        "R1 = [9-0];",
        "PegError: range lower 0x39 is greater than upper 0x30. char 5-10: [9-0]."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = [z-a];",
        "PegError: range lower 0x7a is greater than upper 0x61. char 5-10: [z-a]."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = [\\U0010ffff-\\u0001];",
        "PegError: range lower 0x10ffff is greater than upper 0x1. char 5-24: [\\U0010ffff-\\u0001]."
    );

    /* lower / upper / stride is zero. */

    ASSERT_BAD_GRAMMAR(
        "R1 = [\\u0000-\\U0010ffff];",
        "PegError: range lower 0x0, upper 0x10ffff, stride 0x1 must be all non-zeros. char 5-24: [\\u0000-\\U0010ffff]."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = [a-f..0];",
        "PegError: range lower 0x61, upper 0x66, stride 0x0 must be all non-zeros. char 5-13: [a-f..0]."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = [\\u0001-\\U0010ffff..0];",
        "PegError: range lower 0x1, upper 0x10ffff, stride 0x0 must be all non-zeros. char 5-27: [\\u0001-\\U0010ffff..0]."
    );

    ASSERT_BAD_GRAMMAR(
        "R1 = [\\u0001-\\U00110000];",
        "PegError: range lower 0x1, upper 0x110000 must be less than 0x10ffff. char 5-24: [\\u0001-\\U00110000]."
    );

}

void test_eval_bad_grammar_repeat(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = [0-9]{3,2};",
        "PegError: repeat min 3 is greater than max 2. char 5-15: [0-9]{3,2}."
    );
    ASSERT_BAD_GRAMMAR(
        "R1 = [0-9]{m,n};",
        "CutError: grammar syntax error: line 1:11, expect rule (char ';')."
    );
}

void test_eval_bad_grammar_reference(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = R2;",
        "NameError: reference R2 is undefined."
    );
}

void test_eval_bad_grammar_negative(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = !;",
        "CutError: grammar syntax error: line 1:7, expect primary."
    );
}

void test_eval_bad_grammar_positive(void) {
    ASSERT_BAD_GRAMMAR(
        "R1 = &;",
        "CutError: grammar syntax error: line 1:7, expect primary."
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
    RUN_TEST(test_back_reference);
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
    RUN_TEST(test_cut);
    RUN_TEST(test_grammar);

    RUN_TEST(test_eval_literal);
    RUN_TEST(test_eval_insensitive);
    RUN_TEST(test_eval_range);
    RUN_TEST(test_eval_sequence);
    RUN_TEST(test_eval_choice);
    RUN_TEST(test_eval_positive);
    RUN_TEST(test_eval_negative);
    RUN_TEST(test_eval_cut);
    RUN_TEST(test_eval_repeat);
    RUN_TEST(test_eval_reference);
    RUN_TEST(test_eval_back_reference);
    RUN_TEST(test_eval_dot);
    RUN_TEST(test_eval_comment);
    RUN_TEST(test_eval_flags);

    RUN_TEST(test_eval_bad_grammar);
    RUN_TEST(test_eval_bad_grammar_decorator);
    RUN_TEST(test_eval_bad_grammar_literal);
    RUN_TEST(test_eval_bad_grammar_range);
    RUN_TEST(test_eval_bad_grammar_repeat);
    RUN_TEST(test_eval_bad_grammar_reference);
    RUN_TEST(test_eval_bad_grammar_negative);
    RUN_TEST(test_eval_bad_grammar_positive);

    return UNITY_END();
}
