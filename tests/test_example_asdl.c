#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/adsl.h"

# define ASSERT_ASDL(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreateAsdlGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), P4_GetErrorMessage(source)); \
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

# define ASSERT_ASDL_PASS(entry, input, code) do { \
    P4_Grammar* grammar = P4_CreateAsdlGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), P4_GetErrorMessage(source)); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_id (void) {
    ASSERT_ASDL(P4_AsdlRuleId, "mod", P4_Ok, "[{\"slice\":[0,3],\"type\":\"type_id\"}]");
    ASSERT_ASDL(P4_AsdlRuleId, "Statement", P4_Ok, "[{\"slice\":[0,9],\"type\":\"constructor_id\"}]");
}

void test_field(void) {
    ASSERT_ASDL(P4_AsdlRuleField, "int? end_lineno", P4_Ok, "["
        "{\"slice\":[0,15],\"type\":\"field\",\"children\":["
            "{\"slice\":[0,3],\"type\":\"type_id\"},"
            "{\"slice\":[3,4],\"type\":\"question\"},"
            "{\"slice\":[5,15],\"type\":\"type_id\"}"
        "]}"
    "]");
    ASSERT_ASDL(P4_AsdlRuleField, "int end_lineno", P4_Ok, "["
        "{\"slice\":[0,14],\"type\":\"field\",\"children\":["
            "{\"slice\":[0,3],\"type\":\"type_id\"},"
            "{\"slice\":[4,14],\"type\":\"type_id\"}"
        "]}"
    "]");
}

void test_fields(void) {
    ASSERT_ASDL(P4_AsdlRuleFields, "(int lineno, int? end_lineno)", P4_Ok, "["
        "{\"slice\":[0,29],\"type\":\"fields\",\"children\":["
            "{\"slice\":[1,11],\"type\":\"field\",\"children\":["
                "{\"slice\":[1,4],\"type\":\"type_id\"},"
                "{\"slice\":[5,11],\"type\":\"type_id\"}"
            "]},"
            "{\"slice\":[13,28],\"type\":\"field\",\"children\":["
                "{\"slice\":[13,16],\"type\":\"type_id\"},"
                "{\"slice\":[16,17],\"type\":\"question\"},"
                "{\"slice\":[18,28],\"type\":\"type_id\"}"
            "]}"
        "]}"
    "]");
}

void test_constructor(void) {
    ASSERT_ASDL(P4_AsdlRuleConstructor, "Expression(expr body)", P4_Ok, "["
        "{\"slice\":[0,21],\"type\":\"constructor\",\"children\":["
            "{\"slice\":[0,10],\"type\":\"constructor_id\"},"
            "{\"slice\":[10,21],\"type\":\"fields\",\"children\":["
                "{\"slice\":[11,20],\"type\":\"field\",\"children\":["
                    "{\"slice\":[11,15],\"type\":\"type_id\"},"
                    "{\"slice\":[16,20],\"type\":\"type_id\"}"
                "]}"
            "]}"
        "]}"
    "]");
}

void test_sum(void) {
    ASSERT_ASDL(P4_AsdlRuleType, "Expression(expr body)|Statement(stmt* body)", P4_Ok, "["
        "{\"slice\":[0,43],\"type\":\"sum\",\"children\":["
            "{\"slice\":[0,21],\"type\":\"constructor\",\"children\":["
                "{\"slice\":[0,10],\"type\":\"constructor_id\"},"
                "{\"slice\":[10,21],\"type\":\"fields\",\"children\":["
                    "{\"slice\":[11,20],\"type\":\"field\",\"children\":["
                        "{\"slice\":[11,15],\"type\":\"type_id\"},"
                        "{\"slice\":[16,20],\"type\":\"type_id\"}"
                    "]}"
                "]}"
            "]},"
            "{\"slice\":[22,43],\"type\":\"constructor\",\"children\":["
                "{\"slice\":[22,31],\"type\":\"constructor_id\"},"
                "{\"slice\":[31,43],\"type\":\"fields\",\"children\":["
                    "{\"slice\":[32,42],\"type\":\"field\",\"children\":["
                        "{\"slice\":[32,36],\"type\":\"type_id\"},"
                        "{\"slice\":[36,37],\"type\":\"asterisk\"},"
                        "{\"slice\":[38,42],\"type\":\"type_id\"}"
                    "]}"
                "]}"
            "]}"
        "]}"
    "]");
    ASSERT_ASDL(P4_AsdlRuleType, "Load", P4_Ok, "["
        "{\"slice\":[0,4],\"type\":\"sum\",\"children\":["
            "{\"slice\":[0,4],\"type\":\"constructor\",\"children\":["
                "{\"slice\":[0,4],\"type\":\"constructor_id\"}"
            "]}"
        "]}"
    "]");
}

void test_product(void) {
    ASSERT_ASDL(P4_AsdlRuleType, "(a b,c? d) attributes (e f)", P4_Ok, "["
        "{\"slice\":[0,27],\"type\":\"product\",\"children\":["
            "{\"slice\":[0,10],\"type\":\"fields\",\"children\":["
                "{\"slice\":[1,4],\"type\":\"field\",\"children\":["
                    "{\"slice\":[1,2],\"type\":\"type_id\"},"
                    "{\"slice\":[3,4],\"type\":\"type_id\"}"
                "]},"
                "{\"slice\":[5,9],\"type\":\"field\",\"children\":["
                    "{\"slice\":[5,6],\"type\":\"type_id\"},"
                    "{\"slice\":[6,7],\"type\":\"question\"},"
                    "{\"slice\":[8,9],\"type\":\"type_id\"}"
                "]}"
            "]},"
            "{\"slice\":[22,27],\"type\":\"fields\",\"children\":["
                "{\"slice\":[23,26],\"type\":\"field\",\"children\":["
                    "{\"slice\":[23,24],\"type\":\"type_id\"},"
                    "{\"slice\":[25,26],\"type\":\"type_id\"}"
                "]}"
            "]}"
        "]}"
    "]");
}

void test_module(void) {
    ASSERT_ASDL(P4_AsdlRuleModule, "module Test { stmt = Def }", P4_Ok, "["
        "{\"slice\":[0,26],\"type\":\"module\",\"children\":["
            "{\"slice\":[7,11],\"type\":\"constructor_id\"},"
            "{\"slice\":[14,25],\"type\":\"definitions\",\"children\":["
                "{\"slice\":[14,25],\"type\":\"definition\",\"children\":["
                    "{\"slice\":[14,18],\"type\":\"type_id\"},"
                    "{\"slice\":[21,25],\"type\":\"sum\",\"children\":["
                        "{\"slice\":[21,25],\"type\":\"constructor\",\"children\":["
                            "{\"slice\":[21,24],\"type\":\"constructor_id\"}"
                        "]}"
                    "]}"
                "]}"
            "]}"
        "]}"
    "]");
    ASSERT_ASDL_PASS(P4_AsdlRuleEntry,
        "-- ASDL's 4 builtin types are:\n-- identifier, int, string, constant\n\
        module Python {\n\
        mod = Module(stmt* body, type_ignore* type_ignores)\n\
        | Interactive(stmt* body)\n\
        | Expression(expr body)\n\
        | FunctionType(expr* argtypes, expr returns)\n"
        "expr_context = Load | Store | Del\n"
        "boolop = And | Or\n"
        "operator = Add | Sub | Mult | MatMult | Div | Mod | Pow | LShift\n"
        "         | RShift | BitOr | BitXor | BitAnd | FloorDiv\n"
        "unaryop = Invert | Not | UAdd | USub\n"
        "cmpop = Eq | NotEq | Lt | LtE | Gt | GtE | Is | IsNot | In | NotIn\n"
        "comprehension = (expr target, expr iter, expr* ifs, int is_async)\n"
        "excepthandler = ExceptHandler(expr? type, identifier? name, stmt* body)\n"
        "            attributes (int lineno, int col_offset, int? end_lineno, int? end_col_offset)\n"
        "}"
        , P4_Ok
    );
}

void test_python_asdl(void) {
    char* buf = read_file("Python.asdl");
    ASSERT_ASDL_PASS(P4_AsdlRuleEntry, buf, P4_Ok);
    free(buf);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_id);
    RUN_TEST(test_field);
    RUN_TEST(test_fields);
    RUN_TEST(test_constructor);
    RUN_TEST(test_sum);
    RUN_TEST(test_product);
    RUN_TEST(test_module);
    RUN_TEST(test_python_asdl);

    return UNITY_END();
}
