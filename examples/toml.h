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
 * Example: Write a TOML Parser using Peppa PEG.
 *
 * https://toml.io/en/v1.0.0
 *
*/

# ifndef P4_LANG_TOML_H
# define P4_LANG_TOML_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

void P4_TomlFormatNode(FILE* stream, P4_Node* node) {
}

P4_Error P4_TomlEvalInteger(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node) {
    node->userdata = node->head->userdata;
    printf("integer: %lld\n", node->userdata);
    P4_DeleteNodeChildren(node);
    return P4_Ok;
}

P4_Error P4_TomlEvalDigit(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node) {
    if (node->text[node->slice.start.pos] <= '9')
        node->userdata = (void*) (int64_t) (node->text[node->slice.start.pos] - '0');
    else if (node->text[node->slice.start.pos] <= 'f')
        node->userdata = (void*) (int64_t) (node->text[node->slice.start.pos] - 'a');
    else if (node->text[node->slice.start.pos] <= 'F')
        node->userdata = (void*) (int64_t) (node->text[node->slice.start.pos] - 'A');
    return P4_Ok;
}

P4_Error P4_TomlEvalDecInt(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node) {
    int64_t num = (int64_t) node->tail->userdata;
    const char* rule_name = node->head->rule_name;
    if (strcmp(rule_name, "minus")) num = -num;
    node->userdata = (void*)num;
    return P4_Ok;
}

P4_Error P4_TomlEvalUnsignedInt(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node, int base) {
    P4_Node* child = NULL;
    int64_t num = 0;

    if (node->head == NULL) { /* "0" */
        num = 0;
    } else {
        for (child = node->head; child != NULL; child = child->next) {
            num = num * base + (int64_t) (child->userdata);
        }
    }
    node->userdata = (void*) num;
    return P4_Ok;
}

P4_Error P4_TomlCallback(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node) {
    const char* rule_name = P4_GetRuleName(rule);
    if (rule_name == NULL || node == NULL) return P4_Ok;
    if (strcmp(rule_name, "one_nine") == 0) return P4_TomlEvalDigit(grammar, rule, node);
    if (strcmp(rule_name, "zero_seven") == 0) return P4_TomlEvalDigit(grammar, rule, node);
    if (strcmp(rule_name, "zero_one") == 0) return P4_TomlEvalDigit(grammar, rule, node);
    if (strcmp(rule_name, "HEXDIG") == 0) return P4_TomlEvalDigit(grammar, rule, node);
    if (strcmp(rule_name, "DIGIT") == 0) return P4_TomlEvalDigit(grammar, rule, node);
    if (strcmp(rule_name, "unsigned_dec_int") == 0) return P4_TomlEvalUnsignedInt(grammar, rule, node, 10);
    if (strcmp(rule_name, "hex_int") == 0) return P4_TomlEvalUnsignedInt(grammar, rule, node, 16);
    if (strcmp(rule_name, "oct_int") == 0) return P4_TomlEvalUnsignedInt(grammar, rule, node, 8);
    if (strcmp(rule_name, "bin_int") == 0) return P4_TomlEvalUnsignedInt(grammar, rule, node, 2);
    if (strcmp(rule_name, "dec_int") == 0) return P4_TomlEvalDecInt(grammar, rule, node);
    if (strcmp(rule_name, "integer") == 0) return P4_TomlEvalInteger(grammar, rule, node);
    return P4_Ok;
}

P4_Grammar*  P4_CreateTomlGrammar() {
    P4_Grammar* grammar = P4_LoadGrammar(

        "toml = expression (newline expression)*;\n"
        "expression = ws (keyval / table)? ws comment?;\n"

        /* Key-Value pairs */
        "keyval = key keyval_sep val;\n"
        "@lifted keyval_sep = ws \"=\" ws;\n"

        /* Key */
        "@lifted key = dotted_key / simple_key;\n"
        "@lifted simple_key = quoted_key / unquoted_key;\n"
        "@squashed unquoted_key = (ALPHA / DIGIT / \"-\" / \"_\"){1,};\n"
        "@squashed quoted_key = basic_string / literal_string;\n"
        "dotted_key = simple_key (dot_sep simple_key)+;\n"
        "@lifted dot_sep = ws \".\" ws;\n"

        /* Value */
        "@lifted val = boolean / datetime / array / inline_table / string / float / integer;\n"

        "@lifted string = ml_literal_string / ml_basic_string / basic_string / literal_string;\n"

        /* Basic String */
        "basic_string = \"\\\"\" basic_char* \"\\\"\";\n"
        "@lifted basic_char = basic_unescaped / escaped;\n"
        "@lifted basic_unescaped = wschar / \"!\" / [\\u0023-\\u005B] / [\\u005D-\\u007E] / non_ascii;\n"
        "@scoped @squashed escaped = \"\\\\\" (\"\\\"\" / \"\\\\\" / \"b\" / \"f\" / \"n\" / \"r\" / \"t\" / \"u\" HEXDIG{4} / \"U\" HEXDIG{8});\n"

        /* Multi-line Basic String */
        "@squashed ml_basic_string = \"\\x22\\x22\\x22\" newline? ml_basic_body \"\\x22\\x22\\x22\";\n"
        "ml_basic_body = mlb_content* (mlb_quotes mlb_content+)*;\n"
        "mlb_content = mlb_char / newline / mlb_escaped_nl;\n"
        "mlb_char = mlb_unescaped / escaped;\n"
        "mlb_quotes = \"\\x22\";\n"
        "mlb_unescaped = wschar / \"\\x21\" / [\\x23-\\x5B] / [\\x5D-\\x7E] / non_ascii;\n"
        "mlb_escaped_nl = escaped ws newline *( wschar / newline );\n"

        /* Literal String */
        "literal_string = \"'\" literal_char* \"'\";\n"
        "@lifted literal_char = \"\\t\" / [\\u0020-\\u0026] / [\\u0028-\\u007E] / non_ascii;\n"

        /* Multi-line literal String */
        "@squashed ml_literal_string = \"'''\" newline? ml_literal_body \"'''\";\n"
        "ml_literal_body = mll_content* (mll_quotes mll_content+)*;\n"
        "mll_content = mll_char / newline;\n"
        "mll_char = \"\\x09\" / [\\x20-\\x26] / [\\x28-\\x7e] / non_ascii;\n"
        "mll_quotes = \"''\" / \"'\";\n"

        /* Boolean */
        "boolean = \"true\" / \"false\";\n"

        /* Integer */
        "minus = \"-\";\n"
        "plus = \"+\";\n"
        "@lifted underscore = \"_\";\n"
        "one_nine = [1-9];\n"
        "zero_seven = [0-7];\n"
        "zero_one = [0-1];\n"
        "@lifted hex_prefix = i\"0x\";\n"
        "@lifted oct_prefix = i\"0o\";\n"
        "@lifted bin_prefix = i\"0b\";\n"
        "dec_int = (minus / plus)? unsigned_dec_int;\n"
        "unsigned_dec_int = \"0\" / one_nine (DIGIT / underscore DIGIT)*;\n"
        "hex_int = hex_prefix HEXDIG (HEXDIG / underscore HEXDIG)*;\n"
        "oct_int = oct_prefix zero_seven (zero_seven / underscore zero_seven)*;\n"
        "bin_int = bin_prefix zero_one (zero_one / underscore zero_one)*;\n"
        "integer = hex_int / oct_int / bin_int / dec_int;\n"

        /* Float */
        "float = special_float / dec_int (exp / frac exp?);"
        "zero_prefixed_int = DIGIT (underscore DIGIT / DIGIT)*;\n"
        "frac = \".\" zero_prefixed_int;\n"
        "exp = i\"e\" (minus / plus)? zero_prefixed_int;\n"
        "special_float = (minus / plus)? (inf / nan);\n"
        "inf = \"inf\";\n"
        "nan = \"nan\";\n"

        /* Date and Time */
        "@squashed date_fullyear = DIGIT{4};\n"
        "@squashed date_month = \"0\" [1-9] / \"1\" [0-2];\n"
        "@squashed date_mday = DIGIT{2};\n"
        "time_delim = i\"t\" / \" \";\n"
        "@squashed time_hour = DIGIT{2};\n"
        "@squashed time_minute = DIGIT{2};\n"
        "@squashed time_second = DIGIT{2};\n"
        "@squashed time_secfrac = DIGIT+;\n"
        "time_numoffset = (\"+\" / \"-\") time_hour \":\" time_minute;\n"
        "time_offset = i\"z\" / time_numoffset;\n"
        "partial_time = time_hour \":\" time_minute \":\" time_second (\".\" time_secfrac)?;\n"
        "full_date = date_fullyear \"-\" date_month \"-\" date_mday;\n"
        "full_time = partial_time time_offset;\n"
        "offset_date_time = full_date time_delim full_time;\n"
        "local_date_time = full_date time_delim partial_time;\n"
        "local_date = full_date;\n"
        "local_time = partial_time;\n"
        "datetime = offset_date_time / local_date_time / local_date / local_time;\n"

        /* Array */
        "array = array_open array_ws array_values? array_ws array_close;\n"
        "@lifted array_values = val (array_ws array_sep array_ws val)*;\n"
        "@lifted array_open = \"[\";\n"
        "@lifted array_close = \"]\";\n"
        "@lifted array_sep = \",\";\n"
        "@lifted @squashed array_ws = (wschar / comment? newline)*;\n"

        /* Inline Table */
        "inline_table = inline_table_open ws inline_table_values? ws inline_table_close;\n"
        "@lifted inline_table_values = keyval (ws inline_table_sep ws keyval)*;\n"
        "@lifted inline_table_open = \"{\";\n"
        "@lifted inline_table_close = \"}\";\n"
        "@lifted inline_table_sep = \",\";\n"

        /* Table */
        "@lifted table = array_table / std_table;\n"

        /* Standard Table */
        "std_table = std_table_open ws key ws std_table_close;\n"
        "@lifted std_table_open = \"[\";\n"
        "@lifted std_table_close = \"]\";\n"

        /* Array Table */
        "array_table = array_table_open ws key ws array_table_close;\n"
        "@lifted array_table_open = \"[[\";\n"
        "@lifted array_table_close = \"]]\";\n"

        /* Comment */
        "comment = comment_start comment_body;\n"

        "comment_start = \"#\";\n"
        "@squashed comment_body = non_eol*;\n"

        "non_eol = \"\\t\" / [\\u0020-\\u007F] / non_ascii;\n"
        "non_ascii = [\\u0080-\\uD7FF] / [\\uE000-\\U0010FFFF];\n"

        /* Newline */
        "newline = \"\\n\" / \"\\r\\n\";\n"

        /* Whitespace */
        "@lifted @squashed ws = wschar*;\n"
        "wschar = \" \" / \"\\t\";\n"

        "ALPHA = [a-z] / [A-Z];\n"
        "DIGIT = [0-9];\n"
        "HEXDIG = [a-f] / [A-F] / [0-9];\n"
    );

    P4_SetGrammarCallback(grammar, P4_TomlCallback, NULL);

    return grammar;
}

#ifdef __cplusplus
}
#endif

# endif
