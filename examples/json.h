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
 * Example: Write a JSON Parser using Peppa PEG.
 *
 * https://tools.ietf.org/html/rfc7159
 *
*/

# ifndef P4_LANG_JSON_H
# define P4_LANG_JSON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppa.h"

P4_Grammar*  P4_CreateJSONGrammar() {
    return P4_LoadGrammar(
        "@lifted\n"
        "entry = &. value !.;\n"

        "@lifted\n"
        "value = object / array / string / number / true / false / null;\n"

        "object = \"{\" (item (\",\" item)*)? \"}\";\n"
        "item = string \":\" value;\n"

        "array = \"[\" (value (\",\" value)*)? \"]\";\n"

        "@tight\n"
        "string = \"\\\"\" ([\\u0020-\\u0021] / [\\u0023-\\u005b] / [\\u005d-\\U0010ffff] / escape )* \"\\\"\";\n"

        "true = \"true\";\n"
        "false = \"false\";\n"
        "null = \"null\";\n"

        "@tight @squashed\n"
        "number = minus? integral fractional? exponent?;\n"

        "@tight @squashed @lifted\n"
        "escape = \"\\\\\" (\"\\\"\" / \"/\" / \"\\\\\" / \"b\" / \"f\" / \"n\" / \"r\" / \"t\" / unicode);\n"

        "@tight @squashed"
        "unicode = \"u\" ([0-9] / [a-f] / [A-F]){4};\n"

        "minus = \"-\";\n"
        "plus = \"+\";\n"

        "@squashed @tight\n"
        "integral = \"0\" / [1-9] [0-9]*;\n"

        "@squashed @tight\n"
        "fractional = \".\" [0-9]+;\n"

        "@tight"
        "exponent = i\"e\" (plus / minus)? [0-9]+;\n"

        "@spaced @lifted\n"
        "whitespace = \" \" / \"\\r\" / \"\\n\" / \"\\t\";\n"
    );
}

#ifdef __cplusplus
}
#endif

# endif
