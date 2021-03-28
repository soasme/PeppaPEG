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

# define SETUP_EVAL(entry, input) \
    P4_Grammar* grammar = P4_CreateP4GenGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source)); \
    P4_Token* root = P4_GetSourceAst(source);

# define TEARDOWN_EVAL() \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar);

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

void test_rule_flag(void) {
    check_parse(P4_P4GenDecorator, "@scoped", "[{\"slice\":[0,7],\"type\":\"decorator\"}]");
}

void test_rule(void) {
    check_parse(P4_P4GenRule, "a = \"1\";", "["
        "{\"slice\":[0,8],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,7],\"type\":\"literal\"}"
        "]}"
    "]");
    check_parse(P4_P4GenRule, "a = [1-9];", "["
        "{\"slice\":[0,10],\"type\":\"rule\",\"children\":["
            "{\"slice\":[0,1],\"type\":\"name\"},"
            "{\"slice\":[4,9],\"type\":\"range\",\"children\":["
                "{\"slice\":[5,6],\"type\":\"char\"},"
                "{\"slice\":[7,8],\"type\":\"char\"}]"
        "}]}"
    "]");

    check_parse(P4_P4GenRule, "a = \"0x\" [1-9];", "["
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

    check_parse(P4_P4GenRule, "a = \"0x\"/[1-9];", "["
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

    check_parse(P4_P4GenRule, "@lifted @squashed @spaced @nonterminal @scoped @tight\na = \"1\";", "["
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
    check_parse(P4_P4GenGrammar,
        "one = \"1\";\n"
        "entry = one one;",
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

#define ASSERT_EVAL(entry, input, expect_t, expect) do { \
        SETUP_EVAL((entry), (input)); \
        expect_t value = 0; \
        if (root) P4_P4GenEval(root, &value); \
        TEST_ASSERT_EQUAL((expect), value); \
        TEARDOWN_EVAL(); \
} while (0);

void test_eval_flag(void) {
    ASSERT_EVAL(P4_P4GenDecorator, "@squashed", P4_ExpressionFlag, P4_FLAG_SQUASHED);
    ASSERT_EVAL(P4_P4GenDecorator, "@scoped", P4_ExpressionFlag, P4_FLAG_SCOPED);
    ASSERT_EVAL(P4_P4GenDecorator, "@spaced", P4_ExpressionFlag, P4_FLAG_SPACED);
    ASSERT_EVAL(P4_P4GenDecorator, "@lifted", P4_ExpressionFlag, P4_FLAG_LIFTED);
    ASSERT_EVAL(P4_P4GenDecorator, "@tight", P4_ExpressionFlag, P4_FLAG_TIGHT);
    ASSERT_EVAL(P4_P4GenDecorator, "@nonterminal", P4_ExpressionFlag, P4_FLAG_NON_TERMINAL);
}

void test_eval_flags(void) {
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@squashed @lifted", P4_ExpressionFlag, P4_FLAG_SQUASHED | P4_FLAG_LIFTED);
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@squashed @lifted @squashed", P4_ExpressionFlag, P4_FLAG_SQUASHED | P4_FLAG_LIFTED);
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@spaced", P4_ExpressionFlag, P4_FLAG_SPACED);
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@spaced @squashed @lifted", P4_ExpressionFlag,
            P4_FLAG_SPACED | P4_FLAG_SQUASHED | P4_FLAG_LIFTED);
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@nonterminal", P4_ExpressionFlag, P4_FLAG_NON_TERMINAL);
    ASSERT_EVAL(P4_P4GenRuleDecorators, "@whatever", P4_ExpressionFlag, 0);
}

void test_eval_num(void) {
    ASSERT_EVAL(P4_P4GenNumber, "0", long, 0);
    ASSERT_EVAL(P4_P4GenNumber, "1", long, 1);
    ASSERT_EVAL(P4_P4GenNumber, "12", long, 12);
    ASSERT_EVAL(P4_P4GenNumber, "999", long, 999);
    ASSERT_EVAL(P4_P4GenNumber, "10000", long, 10000);
}

void test_eval_char(void) {
    ASSERT_EVAL(P4_P4GenChar, "a", P4_Rune, 'a');
    ASSERT_EVAL(P4_P4GenChar, "A", P4_Rune, 'A');
    ASSERT_EVAL(P4_P4GenChar, "\\b", P4_Rune, '\b');
    ASSERT_EVAL(P4_P4GenChar, "\\t", P4_Rune, '\t');
    ASSERT_EVAL(P4_P4GenChar, "\\n", P4_Rune, '\n');
    ASSERT_EVAL(P4_P4GenChar, "\\f", P4_Rune, '\f');
    ASSERT_EVAL(P4_P4GenChar, "\\r", P4_Rune, '\r');
    ASSERT_EVAL(P4_P4GenChar, "\\\\", P4_Rune, '\\');
    ASSERT_EVAL(P4_P4GenChar, "ç", P4_Rune, 0x00e7);
    ASSERT_EVAL(P4_P4GenChar, "Ç", P4_Rune, 0x00C7);
    ASSERT_EVAL(P4_P4GenChar, "你", P4_Rune, 0x4f60); /* https://www.compart.com/en/unicode/U+4F60 */
    ASSERT_EVAL(P4_P4GenChar, "🐷", P4_Rune, 0x1f437); /* https://www.compart.com/en/unicode/U+1F437 */
    ASSERT_EVAL(P4_P4GenChar, "\\u4f60", P4_Rune, 0x4f60);
    ASSERT_EVAL(P4_P4GenChar, "\\u0041", P4_Rune, 'A');
    ASSERT_EVAL(P4_P4GenChar, "\\u0061", P4_Rune, 'a');
    ASSERT_EVAL(P4_P4GenChar, "\\u000a", P4_Rune, '\n');
    ASSERT_EVAL(P4_P4GenChar, "\\u000A", P4_Rune, '\n');
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
    RUN_TEST(test_rule_flag);
    RUN_TEST(test_rule);
    RUN_TEST(test_grammar);

    RUN_TEST(test_eval_flag);
    RUN_TEST(test_eval_flags);
    RUN_TEST(test_eval_num);
    RUN_TEST(test_eval_char);

    return UNITY_END();
}
