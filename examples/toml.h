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

P4_Grammar*  P4_CreateTomlGrammar() {
    return P4_LoadGrammar(

        "toml = expression (newline expression)*;\n"
        "@lifted expression = ws (keyval / table)? ws comment?;\n"

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
        "underscore = \"_\";\n"
        "one_nine = [1-9];\n"
        "zero_seven = [0-7];\n"
        "zero_one = [0-1];\n"
        "hex_prefix = i\"0x\";\n"
        "oct_prefix = i\"0o\";\n"
        "bin_prefix = i\"0b\";\n"
        "dec_int = (minus / plus)? unsigned_dec_int;\n"
        "unsigned_dec_int = \"0\" / one_nine (DIGIT / underscore DIGIT)*;\n"
        "hex_int = hex_prefix HEXDIG (HEXDIG / underscore HEXDIG)*;\n"
        "oct_int = oct_prefix zero_seven (zero_seven / underscore zero_seven)*;\n"
        "bin_int = bin_prefix zero_one (zero_one / underscore zero_one)*;\n"
        "@squashed integer = hex_int / oct_int / bin_int / dec_int;\n"

        /* Float */
        "@squashed float = special_float / dec_int (exp / frac exp?);"
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
        "@lifted @squashed comment = comment_start comment_body;\n"

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
}

typedef struct P4_TomlDateTime P4_TomlDateTime;
typedef struct P4_TomlArray P4_TomlArray;
typedef struct P4_TomlTable P4_TomlTable;
typedef struct P4_TomlKeyVal P4_TomlKeyVal;
typedef struct P4_TomlValue P4_TomlValue;

struct P4_TomlDateTime {
    int *year, *month, *day;
    int *hour, *minute, *second, *millisecond;
    char *timezone;
    struct {
        int year, month, day;
        int hour, minute, second, millisecond;
        char timezone[10];
    } __internal__;
};

struct P4_TomlValue {
    char kind; /* 's', 'b', 'i', 'f', 'd', 'a', 't' */
    union {
        P4_String           s;
        bool                b;
        int64_t             i;
        double              f;
        P4_TomlDateTime*    d;
        P4_TomlArray*       a;
        P4_TomlTable*       t;
    } v;
};

struct P4_TomlKeyVal {
    const char*     key;
    P4_TomlValue*   val;
};

struct P4_TomlTable {
    size_t          count;
    P4_TomlKeyVal*  items;
};

struct P4_TomlArray {
    size_t          count;
    P4_TomlValue*   items;
};

P4_Error
P4_TransformTomlBoolean(P4_Node* node, P4_TomlValue* v) {
    v->kind = 'b';
    v->v.b = node->text[node->slice.start.pos] == 't';
    printf("transform: kind='b', b=%d\n", v->v.b);
    return P4_Ok;
}

P4_Error
P4_TransformTomlValue(P4_Node* node, P4_TomlValue* v) {
    return P4_TransformTomlBoolean(node, v);
}

P4_Error
P4_TransformTomlUnquotedKey(P4_Node* node, P4_String* key) {
    *key = P4_CopyNodeString(node);
    printf("transform: unquoted_key=%s\n", *key);
    return P4_Ok;
}

P4_Error
P4_TransformTomlKeyVal(P4_Node* node, P4_TomlKeyVal* kv) {
    P4_Error err = P4_Ok;

    char* key = NULL;
    if ((err = P4_TransformTomlUnquotedKey(node->head, &key)) != P4_Ok)
        return err;

    kv->key = key;

    if ((kv->val = P4_MALLOC(sizeof(P4_TomlValue))) == NULL)
        return P4_MemoryError;

    if ((err = P4_TransformTomlValue(node->tail, kv->val)) != P4_Ok) {
        P4_FREE(kv->val);
        return err;
    }

    return P4_Ok;
}

P4_Error
P4_TransformToml(P4_Node* node, P4_TomlTable* table) {
    table->count = P4_GetNodeChildrenCount(node);
    printf("transform: count=%lu\n", table->count);
    if ((table->items = P4_MALLOC(table->count * sizeof(P4_TomlKeyVal))) == NULL) {
        return P4_MemoryError;
    };

    P4_Error err = P4_Ok;
    P4_Node* child = node->head;
    size_t i = 0;

    while (child != NULL) {
        if ((err = P4_TransformTomlKeyVal(child, &table->items[i])) != P4_Ok)
            goto finalize;
        child = node->next;
    }

    return P4_Ok;
finalize:
    table->count = 0;
    P4_FREE(table->items);
    table->items = NULL;
    return err;
}


/*
 * toml         => P4_TomlTable.
 * keyval       => P4_TomlKeyVal.
 * unquoted_key => P4_TomlKeyVal.key.
 * integer      => P4_TomlValue.i.
 * ml_basic_string => P4_TomlValue.s.
 * basic_string => P4_TomlValue.s.
 * literal_string => P4_TomlValue.s.
 * ml_literal_string => P4_TomlValue.s.
 * float => P4_TomlValue.f.
 * boolean => P4_TomlValue.b.
 * boolean => P4_TomlValue.b.
 * local_date => P4_TomlValue.d.
 * local_time => P4_TomlValue.d.
 * local_date_time => P4_TomlValue.d.
 * offset_date_time => P4_TomlValue.d.
 * std_table => P4_TomlValue.t
 * array_table => P4_TomlValue.t
 * array => P4_TomlValue.a
 */

#ifdef __cplusplus
}
#endif

# endif
