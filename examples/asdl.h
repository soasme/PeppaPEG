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
 * Example: Write an ASDL Parser using Peppa PEG.
 *
*/

# ifndef P4_LANG_ASDL_H
# define P4_LANG_ASDL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

typedef enum P4_AsdlRule {
    P4_AsdlRuleEntry = 1,
    P4_AsdlRuleModule,
    P4_AsdlRuleDefinitions,
    P4_AsdlRuleDefinition,
    P4_AsdlRuleId,
    P4_AsdlRuleTypeId,
    P4_AsdlRuleConstructorId,
    P4_AsdlRuleQuestion,
    P4_AsdlRuleAsterisk,
    P4_AsdlRuleField,
    P4_AsdlRuleFields,
    P4_AsdlRuleConstructor,
    P4_AsdlRuleType,
    P4_AsdlRuleSum,
    P4_AsdlRuleProduct,
    P4_AsdlRuleWhitespace,
    P4_AsdlRuleComment,
} P4_AsdlRule;

P4_Grammar* P4_CreateAsdlGrammar() {
    return P4_LoadGrammar(
        "@lifted entry = &. module !.;\n"

        "module = \"module\" id \"{\" definitions \"}\";\n"
        "definitions = definition*;\n"
        "definition = type_id \"=\" type;\n"

        "@lifted   id               = type_id / constructor_id;\n"
        "@squashed @tight type_id          = [a-z] ([a-z] / [A-Z] / [0-9] / \"_\")*;\n"
        "@squashed @tight constructor_id   = [A-Z] ([a-z] / [A-Z] / [0-9] / \"_\")*;\n"

        "question   = \"?\";\n"
        "asterisk   = \"*\";\n"

        "field = type_id (question / asterisk)? id?;\n"
        "fields = \"(\" field (\",\" field)* \")\";\n"
        "constructor = constructor_id fields?;\n"

        "@lifted type = sum / product;\n"
        "sum = constructor (\"|\" constructor)* (\"attributes\" fields)?;\n"
        "product = fields (\"attributes\" fields)?;\n"

        "@spaced @lifted\n"
        "whitespace = \" \" / \"\\t\" / \"\\r\" / \"\\n\";"
        "@spaced @lifted\n"
        "comment = (\"--\" / \"#\") (!\"\\n\" .)* \"\\n\"?;\n"
    );
}

#ifdef __cplusplus
}
#endif

# endif
