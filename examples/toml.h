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
        "@lifted val = boolean / datetime / array / inline_table / string;\n"

        "@lifted string = basic_string / literal_string;\n"

        /* Basic String */
        "basic_string = \"\\\"\" basic_char* \"\\\"\";\n"
        "@lifted basic_char = basic_unescaped / escaped;\n"
        "@lifted basic_unescaped = wschar / \"!\" / [\\u{0023}-\\u{005B}] / [\\u{005D}-\\u{007E}] / non_ascii;\n"
        "@scoped @squashed escaped = \"\\\\\" (\"\\\"\" / \"\\\\\" / \"b\" / \"f\" / \"n\" / \"r\" / \"t\" / \"u\" HEXDIG{4} / \"U\" HEXDIG{8});\n"

        /* Literal String */
        "literal_string = \"'\" literal_char* \"'\";\n"
        "@lifted literal_char = \"\\t\" / [\\u{0020}-\\u{0026}] / [\\u{0028}-\\u{007E}] / non_ascii;\n"

        /* Boolean */
        "boolean = \"true\" / \"false\";\n"

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

        "non_eol = \"\\t\" / [\\u{0020}-\\u{007F}] / non_ascii;\n"
        "non_ascii = [\\u{0080}-\\u{D7FF}] / [\\u{E000}-\\u{10FFFF}];\n"

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

#ifdef __cplusplus
}
#endif

# endif
