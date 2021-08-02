/*
 * Peppa PEG -  Ultra lightweight PEG Parser in ANSI C.
 *
 * MIT License
 *
 * Copyright (c) 2021 Ju
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Example: Write a DOT Parser using Peppa PEG.
 *
 * https://graphviz.org/doc/info/lang.html
 *
*/

# ifndef P4_LANG_DOT_H
# define P4_LANG_DOT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

P4_Grammar* P4_CreateDotGrammar() {
    return P4_LoadGrammar(
        "@lifted\n"
        "entry = &. graph+ !.;\n"

        "graph = keyword_strict? (keyword_graph / keyword_digraph) ID? \"{\" stmts \"}\";\n"

        "@squashed\n"
        "ID = identifier / number / html_string / string;\n"

        "@squashed @tight\n"
        "identifier = ([a-z]/[A-Z]/\"_\") ([a-z]/[A-Z]/[0-9]/\"_\")*;\n"

        "@squashed @tight\n"
        "number = \"-\"? ((\".\" [0-9]+) / [0-9]+ (\".\" [0-9]*)?);\n"

        "@squashed @tight\n"
        "string = \"\\\"\" (!\"\\\"\" . / \"\\\\\\\"\")+ \"\\\"\";\n"

        "@nonterminal\n"
        "node_id = ID port?;\n"

        "node_stmt = node_id attr_list?;\n"

        "port = \":\" compass_pt / \":\" ID (\":\" compass_pt)?;\n"

        "compass_pt = \"ne\" / \"nw\" / \"n\" / \"se\" / \"sw\" / \"s\" / \"e\" / \"w\" / \"c\" / \"_\";\n"

        "attribute = ID \"=\" ID;\n"

        "@lifted\n"
        "a_list = attribute ((\";\" / \",\") attribute)*;\n"

        "attr_list = (\"[\" a_list \"]\")+;\n"

        "attr_stmt = (keyword_graph / keyword_node / keyword_edge) attr_list;\n"
        "keyword_graph = \"graph\";\n"
        "keyword_node= \"node\";\n"
        "keyword_edge= \"edge\";\n"

        "@lifted\n"
        "stmt = edge_stmt / subgraph / attr_stmt / attribute / node_stmt;\n"

        "@spaced @lifted\n"
        "ws = \" \" / \"\\t\" / \"\\n\" / \"\\r\";"

        "directed = \"->\";"
        "undirected = \"--\";"

        "@lifted\n"
        "edgeop = directed / undirected;\n"

        "keyword_subgraph = \"subgraph\";\n"

        "subgraph = (keyword_subgraph ID?)? \"{\" stmts? \"}\";\n"
        "stmts = stmt (\";\"? stmt)*;\n"

        "edge_stmt = (node_id / subgraph) (edgeop (node_id / subgraph))+ attr_list?;"

        "keyword_digraph = \"digraph\";\n"
        "keyword_strict = \"strict\";\n"

        "@spaced @squashed @lifted\n"
        "comment = macro_comment / c_comment / block_comment;\n"
        "macro_comment = \"#\" (!\"\\n\" .)* \"\\n\"?;\n"
        "c_comment = \"//\" (!\"\\n\" .)* \"\\n\"?;\n"
        "block_comment = \"/*\" (!\"*/\" .)* \"*/\";\n"

        "@squashed\n"
        "html_string = \"<\" ((!(\"<\" / \">\") .)+/ html_string)* \">\";\n"
    );
}

#ifdef __cplusplus
}
#endif

# endif
