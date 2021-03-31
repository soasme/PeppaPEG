#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/p4gen.h"

void check_parse_failed(P4_RuleID entry, P4_String input, P4_Error err) {
    P4_Grammar* grammar = P4_CreatePegGrammar();
    P4_Source* source = P4_CreateSource(input, entry);
    TEST_ASSERT_EQUAL(err, P4_Parse(grammar, source));
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

void check_parse(P4_RuleID entry, P4_String input, P4_String output) {
    P4_Grammar* grammar = P4_CreatePegGrammar();
    P4_Source* source = P4_CreateSource(input, entry);
    TEST_ASSERT_EQUAL(P4_Ok, P4_Parse(grammar, source));
    P4_Token* root = P4_GetSourceAst(source);
    FILE *f = fopen("check.json","w");
    P4_JsonifySourceAst(f, root, P4_PegKindToName);
    fclose(f);
    P4_String s = read_file("check.json");
    TEST_ASSERT_EQUAL_STRING(output, s);
    free(s);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
}

# define SETUP_EVAL(entry, input) \
    P4_Grammar* grammar = P4_CreatePegGrammar(); \
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

void test_literal(void) {
    check_parse(P4_P4GenLiteral, "\"a\"", "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"A\"", "[{\"slice\":[0,3],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"好\"", "[{\"slice\":[0,5],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"👌\"", "[{\"slice\":[0,6],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\t\"", "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\\"\"", "[{\"slice\":[0,4],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"\\u0020\"", "[{\"slice\":[0,8],\"type\":\"literal\"}]");
    check_parse(P4_P4GenLiteral, "\"hello world\"", "[{\"slice\":[0,13],\"type\":\"literal\"}]");
}

void test_insensitive(void) {
    check_parse(P4_P4GenInsensitiveLiteral, "i\"a\"", "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"A\"", "[{\"slice\":[0,4],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,4],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"好\"", "[{\"slice\":[0,6],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,6],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"👌\"", "[{\"slice\":[0,7],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,7],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"\\t\"", "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"\\\"\"", "[{\"slice\":[0,5],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,5],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"\\u0020\"", "[{\"slice\":[0,9],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,9],\"type\":\"literal\"}]}]");
    check_parse(P4_P4GenInsensitiveLiteral, "i\"hello world\"", "[{\"slice\":[0,14],\"type\":\"insensitive\",\"children\":[{\"slice\":[1,14],\"type\":\"literal\"}]}]");
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
    check_parse(P4_PegGrammar,
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
        if (root) P4_PegEval(root, &value); \
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
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"a\"", "a", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"hello world\"", "hello world", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"你好, World\"", "你好, World", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"Peppa PEG 🐷\"", "Peppa PEG 🐷", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"\\u4f60\\u597d, world\"", "你好, world", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"\\n\"", "\n", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"\\r\"", "\r", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"\\t\"", "\t", true);
    ASSERT_EVAL_LITERAL(P4_P4GenLiteral, "\"   \"", "   ", true);
}

void test_eval_insensitive(void) {
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"a\"", "a", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"hello world\"", "hello world", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"你好, World\"", "你好, World", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"Peppa PEG 🐷\"", "Peppa PEG 🐷", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"\\u4f60\\u597d, world\"", "你好, world", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"\\n\"", "\n", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"\\r\"", "\r", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"\\t\"", "\t", false);
    ASSERT_EVAL_LITERAL(P4_P4GenInsensitiveLiteral, "i\"   \"", "   ", false);
}

#define ASSERT_EVAL_RANGE(entry, input, expect_lower, expect_upper) do { \
        SETUP_EVAL((entry), (input)); \
        P4_Expression* value = 0; \
        if (root) P4_PegEval(root, &value); \
        TEST_ASSERT_EQUAL(P4_Range, (value)->kind); \
        TEST_ASSERT_EQUAL(expect_lower, (value)->lower); \
        TEST_ASSERT_EQUAL(expect_upper, (value)->upper); \
        P4_DeleteExpression(value); TEARDOWN_EVAL(); \
} while (0);

void test_eval_range(void) {
    ASSERT_EVAL_RANGE(P4_P4GenRange, "[0-9]", '0', '9');
    ASSERT_EVAL_RANGE(P4_P4GenRange, "[a-z]", 'a', 'z');
    ASSERT_EVAL_RANGE(P4_P4GenRange, "[A-Z]", 'A', 'Z');
    ASSERT_EVAL_RANGE(P4_P4GenRange, "[\\u0001-\\uffff]", 0x1, 0xffff);
    ASSERT_EVAL_RANGE(P4_P4GenRange, "[你-好]", 0x4f60, 0x597d);
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
    ASSERT_EVAL_CONTAINER(P4_P4GenSequence, "\"a\" \"b\" \"c\"", P4_Sequence, 3);
    ASSERT_EVAL_CONTAINER(P4_P4GenSequence, "\"0x\" [1-9]", P4_Sequence, 2);
}

void test_eval_choice(void) {
    ASSERT_EVAL_CONTAINER(P4_P4GenChoice, "\"a\" / \"b\" / \"c\"", P4_Choice, 3);
    ASSERT_EVAL_CONTAINER(P4_P4GenChoice, "\"0x\" / [1-9]", P4_Choice, 2);
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
    ASSERT_EVAL_LOOKAHEAD(P4_P4GenPositive, "& \"a\"", P4_Positive);
    ASSERT_EVAL_LOOKAHEAD(P4_P4GenPositive, "& [0-9]", P4_Positive);
}

void test_eval_negative(void) {
    ASSERT_EVAL_LOOKAHEAD(P4_P4GenNegative, "! \"a\"", P4_Negative);
    ASSERT_EVAL_LOOKAHEAD(P4_P4GenNegative, "! [0-9]", P4_Negative);
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
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"*", 0, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"?", 0, 1);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"+", 1, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"{2,3}", 2, 3);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"{2,}", 2, SIZE_MAX);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"{,3}", 0, 3);
    ASSERT_EVAL_REPEAT(P4_P4GenRepeat, "\"a\"{3}", 3, 3);
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
    ASSERT_EVAL_REFERENCE(P4_P4GenReference, "a", SIZE_MAX);
    ASSERT_EVAL_REFERENCE(P4_P4GenReference, "xyz", SIZE_MAX);
    ASSERT_EVAL_REFERENCE(P4_P4GenReference, "CONST", SIZE_MAX);
}

P4_String test_grammar_rule_to_name(P4_RuleID id) {
    static char name[5] = "00000";
    sprintf(name, "R%lu", (unsigned long)id);
    return name;
}

#define ASSERT_EVAL_GRAMMAR(peg_rules, entry_name, source_code, parse_code, ast) do { \
    SETUP_EVAL((P4_PegGrammar), (peg_rules)); \
    P4_Grammar* peg_grammar = 0; \
    TEST_ASSERT_NOT_NULL_MESSAGE( root, "peg rule should be correctly parsed"); \
    TEST_ASSERT_EQUAL_MESSAGE( P4_Ok, P4_PegEval(root, &peg_grammar), "peg rule should be correctly evaluated"); \
    TEST_ASSERT_NOT_NULL_MESSAGE( peg_grammar, "peg grammar should be successfully created" ); \
    P4_Expression* entry_expr = P4_GetGrammarRuleByName(peg_grammar, (entry_name)); \
    TEST_ASSERT_NOT_NULL_MESSAGE( entry_expr, "peg grammar entry should be successfully resolved." ); \
    P4_Source* input_source = P4_CreateSource((source_code), (entry_expr)->id); \
    TEST_ASSERT_NOT_NULL_MESSAGE( input_source, "source code should be correctly created." ); \
    TEST_ASSERT_EQUAL_MESSAGE( (parse_code), P4_Parse(peg_grammar, input_source), "source code should be correctly parsed"); \
    P4_Token* ast_token = P4_GetSourceAst(input_source); \
    FILE *f = fopen("check.json","w"); \
    P4_JsonifySourceAst(f, ast_token, test_grammar_rule_to_name); \
    fclose(f); \
    P4_String s = read_file("check.json"); \
    TEST_ASSERT_EQUAL_STRING((ast), s); \
    free(s); \
    P4_DeleteSource( input_source ); \
    P4_DeleteGrammar( peg_grammar ); \
    TEARDOWN_EVAL(); \
} while (0);

void test_eval_grammar(void) {
    ASSERT_EVAL_GRAMMAR("R1 = \"1\";", "R1", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"こんにちは世界\";", "R1", "こんにちは世界", P4_Ok, "[{\"slice\":[0,21],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"你好，世界\";", "R1", "你好，世界", P4_Ok, "[{\"slice\":[0,15],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = \"نامهای\";", "R1", "نامهای", P4_Ok, "[{\"slice\":[0,12],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"hello\";", "R1", "hello", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
    ASSERT_EVAL_GRAMMAR("R1 = i\"hello\";", "R1", "HELLO", P4_Ok, "[{\"slice\":[0,5],\"type\":\"R1\"}]");
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
