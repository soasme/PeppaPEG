#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"


# define ASSERT_BAD_GRAMMAR(rule, err) do {\
    P4_Result* result = &(P4_Result){0}; \
    TEST_ASSERT_NOT_EQUAL(P4_Ok, P4_LoadGrammarResult((rule), result)); \
    TEST_ASSERT_NULL(result->grammar); \
    TEST_ASSERT_EQUAL_STRING((err), result->errmsg); \
} while (0);

# define ASSERT_PEG_PARSE(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    if ((code) == P4_Ok) { \
        P4_Node* root = P4_GetSourceAst(source); \
        char filename[30] = {0}; \
        sprintf(filename, "test-peg-%d.json", __LINE__); \
        FILE *f = fopen(filename,"w"); \
        P4_JsonifySourceAst(f, root, NULL); \
        fclose(f); \
        P4_String s = read_file(filename); \
        TEST_ASSERT_EQUAL_STRING((output), s); \
        free(s); \
        remove(filename); \
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

void test_left_recursion(void) {
    ASSERT_PEG_PARSE("left_recursion", "\"x\" | x \"x\"", P4_Ok, "[{\"slice\":[0,11],\"type\":\"left_recursion\",\"children\":[{\"slice\":[0,3],\"type\":\"literal\"},{\"slice\":[6,7],\"type\":\"reference\"},{\"slice\":[8,11],\"type\":\"literal\"}]}]");
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

    ASSERT_BAD_GRAMMAR(
        "R1 = [\\p{This is a bad category}];",
        "PegError: invalid range category. char 5-33: [\\p{This is a bad category}]."
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
    RUN_TEST(test_left_recursion);
    RUN_TEST(test_sequence);
    RUN_TEST(test_repeat);
    RUN_TEST(test_dot);
    RUN_TEST(test_expression);
    RUN_TEST(test_rule_name);
    RUN_TEST(test_rule_flag);
    RUN_TEST(test_rule);
    RUN_TEST(test_grammar);

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
