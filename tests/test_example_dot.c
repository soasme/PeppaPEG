#include <stdio.h>
#include <string.h>
#include "unity/src/unity.h"
#include "common.h"
#include "../examples/dot.h"

# define ASSERT_DOT(entry, input, code, output) do { \
    P4_Grammar* grammar = P4_CreateDotGrammar(); \
    P4_Source* source = P4_CreateSource((input), (entry)); \
    TEST_ASSERT_EQUAL_MESSAGE((code), P4_Parse(grammar, source), "unexpected parse grammar return code"); \
    P4_Node* root = P4_GetSourceAst(source); \
    char filename[30] = {0};\
    sprintf(filename, "test-example-dot-%d.json", __LINE__); \
    FILE *f = fopen(filename,"w"); \
    P4_JsonifySourceAst(f, root, NULL); \
    fclose(f); \
    P4_String s = read_file(filename); \
    TEST_ASSERT_EQUAL_STRING((output), s); \
    free(s); \
    remove(filename); \
    P4_DeleteSource(source); \
    P4_DeleteGrammar(grammar); \
} while (0);

void test_id(void) {
    ASSERT_DOT("ID", "a", P4_Ok, "[{\"slice\":[0,1],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "abc", P4_Ok, "[{\"slice\":[0,3],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "_a", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "a_", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "a1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "a_1", P4_Ok, "[{\"slice\":[0,3],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "_a_1", P4_Ok, "[{\"slice\":[0,4],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "1", P4_Ok, "[{\"slice\":[0,1],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", ".1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "-1", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "-.1", P4_Ok, "[{\"slice\":[0,3],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "111.000", P4_Ok, "[{\"slice\":[0,7],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "-111.000", P4_Ok, "[{\"slice\":[0,8],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "\"a\"", P4_Ok, "[{\"slice\":[0,3],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "\"你好 世界\"", P4_Ok, "[{\"slice\":[0,15],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "\"a\\\"\"", P4_Ok, "[{\"slice\":[0,4],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "\"a\nb\"", P4_Ok, "[{\"slice\":[0,5],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "<table>", P4_Ok, "[{\"slice\":[0,7],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "<<table>>", P4_Ok, "[{\"slice\":[0,9],\"type\":\"ID\"}]");
    ASSERT_DOT("ID", "<<table></table>>", P4_Ok, "[{\"slice\":[0,17],\"type\":\"ID\"}]");
}

void test_compass_pt(void) {
    ASSERT_DOT("compass_pt", "s", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "e", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "n", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "w", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "c", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "_", P4_Ok, "[{\"slice\":[0,1],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "ne", P4_Ok, "[{\"slice\":[0,2],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "se", P4_Ok, "[{\"slice\":[0,2],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "nw", P4_Ok, "[{\"slice\":[0,2],\"type\":\"compass_pt\"}]");
    ASSERT_DOT("compass_pt", "sw", P4_Ok, "[{\"slice\":[0,2],\"type\":\"compass_pt\"}]");
}

void test_port(void) {
    ASSERT_DOT("port", ":se", P4_Ok, "[{\"slice\":[0,3],\"type\":\"port\",\"children\":["
        "{\"slice\":[1,3],\"type\":\"compass_pt\"}"
    "]}]");
    ASSERT_DOT("port", ":ID:se", P4_Ok, "[{\"slice\":[0,6],\"type\":\"port\",\"children\":["
        "{\"slice\":[1,3],\"type\":\"ID\"},"
        "{\"slice\":[4,6],\"type\":\"compass_pt\"}"
    "]}]");
    ASSERT_DOT("port", ":ID", P4_Ok, "[{\"slice\":[0,3],\"type\":\"port\",\"children\":["
        "{\"slice\":[1,3],\"type\":\"ID\"}"
    "]}]");
}

void test_node_id(void) {
    ASSERT_DOT("node_id", "id", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
    ASSERT_DOT("node_id", "id \t", P4_Ok, "[{\"slice\":[0,2],\"type\":\"ID\"}]");
}

void test_node_stmt(void) {
    ASSERT_DOT("stmt", "id", P4_Ok, "[{\"slice\":[0,2],\"type\":\"node_stmt\",\"children\":["
        "{\"slice\":[0,2],\"type\":\"ID\"}"
    "]}]");

    ASSERT_DOT("stmt", "nnnn[style=filled]", P4_Ok, "[{\"slice\":[0,18],\"type\":\"node_stmt\",\"children\":["
        "{\"slice\":[0,4],\"type\":\"ID\"},"
        "{\"slice\":[4,18],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[5,17],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[5,10],\"type\":\"ID\"},"
                "{\"slice\":[11,17],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");

    ASSERT_DOT("stmt", "nnnn[style=filled,color=pink]", P4_Ok, "[{\"slice\":[0,29],\"type\":\"node_stmt\",\"children\":["
        "{\"slice\":[0,4],\"type\":\"ID\"},"
        "{\"slice\":[4,29],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[5,17],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[5,10],\"type\":\"ID\"},"
                "{\"slice\":[11,17],\"type\":\"ID\"}"
            "]},"
            "{\"slice\":[18,28],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[18,23],\"type\":\"ID\"},"
                "{\"slice\":[24,28],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
}

void test_attr_stmt(void) {
    ASSERT_DOT("stmt", "graph[fontname=\"Handlee\"]", P4_Ok, "[{\"slice\":[0,25],\"type\":\"attr_stmt\",\"children\":["
        "{\"slice\":[0,5],\"type\":\"keyword_graph\"},"
        "{\"slice\":[5,25],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[6,24],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[6,14],\"type\":\"ID\"},"
                "{\"slice\":[15,24],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
    ASSERT_DOT("stmt", "node [fontname=\"Handlee\"]", P4_Ok, "[{\"slice\":[0,25],\"type\":\"attr_stmt\",\"children\":["
        "{\"slice\":[0,4],\"type\":\"keyword_node\"},"
        "{\"slice\":[5,25],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[6,24],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[6,14],\"type\":\"ID\"},"
                "{\"slice\":[15,24],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
    ASSERT_DOT("stmt", "edge [fontname=\"Handlee\"]", P4_Ok, "[{\"slice\":[0,25],\"type\":\"attr_stmt\",\"children\":["
        "{\"slice\":[0,4],\"type\":\"keyword_edge\"},"
        "{\"slice\":[5,25],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[6,24],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[6,14],\"type\":\"ID\"},"
                "{\"slice\":[15,24],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
}

void test_edgeop(void) {
    ASSERT_DOT("edgeop", "->", P4_Ok, "[{\"slice\":[0,2],\"type\":\"directed\"}]");
    ASSERT_DOT("edgeop", "--", P4_Ok, "[{\"slice\":[0,2],\"type\":\"undirected\"}]");
}

void test_subgraph(void) {
    ASSERT_DOT("stmt", "{}", P4_Ok, "[{\"slice\":[0,2],\"type\":\"subgraph\"}]");
    ASSERT_DOT("stmt", "subgraph{}", P4_Ok, "[{\"slice\":[0,10],\"type\":\"subgraph\",\"children\":["
        "{\"slice\":[0,8],\"type\":\"keyword_subgraph\"}"
    "]}]");
    ASSERT_DOT("stmt", "subgraph ID {}", P4_Ok, "[{\"slice\":[0,14],\"type\":\"subgraph\",\"children\":["
        "{\"slice\":[0,8],\"type\":\"keyword_subgraph\"},"
        "{\"slice\":[9,11],\"type\":\"ID\"}"
    "]}]");
    ASSERT_DOT("stmt", "subgraph ID {style=filled}", P4_Ok, "[{\"slice\":[0,26],\"type\":\"subgraph\",\"children\":["
        "{\"slice\":[0,8],\"type\":\"keyword_subgraph\"},"
        "{\"slice\":[9,11],\"type\":\"ID\"},"
        "{\"slice\":[13,25],\"type\":\"stmts\",\"children\":["
            "{\"slice\":[13,25],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[13,18],\"type\":\"ID\"},"
                "{\"slice\":[19,25],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
}

void test_edge_stmt(void) {
    ASSERT_DOT("stmt", "a -> b", P4_Ok, "[{\"slice\":[0,6],\"type\":\"edge_stmt\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"ID\"},"
        "{\"slice\":[2,4],\"type\":\"directed\"},"
        "{\"slice\":[5,6],\"type\":\"ID\"}"
    "]}]");
    ASSERT_DOT("stmt", "a -- b", P4_Ok, "[{\"slice\":[0,6],\"type\":\"edge_stmt\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"ID\"},"
        "{\"slice\":[2,4],\"type\":\"undirected\"},"
        "{\"slice\":[5,6],\"type\":\"ID\"}"
    "]}]");
    ASSERT_DOT("stmt", "a -- b -> c", P4_Ok, "[{\"slice\":[0,11],\"type\":\"edge_stmt\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"ID\"},"
        "{\"slice\":[2,4],\"type\":\"undirected\"},"
        "{\"slice\":[5,6],\"type\":\"ID\"},"
        "{\"slice\":[7,9],\"type\":\"directed\"},"
        "{\"slice\":[10,11],\"type\":\"ID\"}"
    "]}]");
    ASSERT_DOT("stmt", "a -> b[color=\"#fff\"]", P4_Ok, "[{\"slice\":[0,20],\"type\":\"edge_stmt\",\"children\":["
        "{\"slice\":[0,1],\"type\":\"ID\"},"
        "{\"slice\":[2,4],\"type\":\"directed\"},"
        "{\"slice\":[5,6],\"type\":\"ID\"},"
        "{\"slice\":[6,20],\"type\":\"attr_list\",\"children\":["
            "{\"slice\":[7,19],\"type\":\"attribute\",\"children\":["
                "{\"slice\":[7,12],\"type\":\"ID\"},"
                "{\"slice\":[13,19],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
}

void test_graph(void) {
    ASSERT_DOT("entry", "graph G {a--b}", P4_Ok, "[{\"slice\":[0,14],\"type\":\"graph\",\"children\":["
        "{\"slice\":[0,5],\"type\":\"keyword_graph\"},"
        "{\"slice\":[6,7],\"type\":\"ID\"},"
        "{\"slice\":[9,13],\"type\":\"stmts\",\"children\":["
            "{\"slice\":[9,13],\"type\":\"edge_stmt\",\"children\":["
                "{\"slice\":[9,10],\"type\":\"ID\"},"
                "{\"slice\":[10,12],\"type\":\"undirected\"},"
                "{\"slice\":[12,13],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
    ASSERT_DOT("entry", "digraph G {a->b}", P4_Ok, "[{\"slice\":[0,16],\"type\":\"graph\",\"children\":["
        "{\"slice\":[0,7],\"type\":\"keyword_digraph\"},"
        "{\"slice\":[8,9],\"type\":\"ID\"},"
        "{\"slice\":[11,15],\"type\":\"stmts\",\"children\":["
            "{\"slice\":[11,15],\"type\":\"edge_stmt\",\"children\":["
                "{\"slice\":[11,12],\"type\":\"ID\"},"
                "{\"slice\":[12,14],\"type\":\"directed\"},"
                "{\"slice\":[14,15],\"type\":\"ID\"}"
            "]}"
        "]}"
    "]}]");
}

void test_comment(void) {
    ASSERT_DOT("stmt", "#test\n{#test\n}#test\n", P4_Ok, "[{\"slice\":[0,14],\"type\":\"subgraph\"}]");
    ASSERT_DOT("stmt", "//test\n{//test\n}//test\n", P4_Ok, "[{\"slice\":[0,16],\"type\":\"subgraph\"}]");
    ASSERT_DOT("stmt", "/*{}*/{/**/}/**/\n", P4_Ok, "[{\"slice\":[0,12],\"type\":\"subgraph\"}]");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_id);
    RUN_TEST(test_compass_pt);
    RUN_TEST(test_port);
    RUN_TEST(test_node_id);
    RUN_TEST(test_node_stmt);
    RUN_TEST(test_attr_stmt);
    RUN_TEST(test_subgraph);
    RUN_TEST(test_edgeop);
    RUN_TEST(test_edge_stmt);
    RUN_TEST(test_graph);
    RUN_TEST(test_comment);
    return UNITY_END();
}
