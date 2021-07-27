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
        "expression = ws keyval? ws comment?;\n"

        /* Key-Value pairs */
        "keyval = key keyval_sep val;\n"
        "keyval_sep = ws \"=\" ws;\n"

        /* Key */
        "@lifted key = unquoted_key;\n"
        "@squashed unquoted_key = (ALPHA / DIGIT / \"-\" / \"_\"){1,};\n"

        /* Value */
        "@lifted val = boolean;\n"

        /* Boolean */
        "boolean = \"true\" / \"false\";\n"

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
