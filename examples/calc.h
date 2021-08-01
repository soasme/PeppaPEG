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
 * Example: Write a Calculator Parser using Peppa PEG.
 *
 * Compile:
 *
 *      $ gcc examples/calc.c peppapeg.c && ./a.out
 *      1+2*3+4/2  * (1+1)
 *      11
 *      1*-1
 *      -1
 *      1/0
 *      Eval Error=6
 *      a/1
 *      Parse Error=2
*/

# ifndef P4_LANG_CALC_H
# define P4_LANG_CALC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../peppapeg.h"

typedef enum {
    P4_CalcStatement    = 1,
    P4_CalcTerm         = 2,
    P4_CalcFactor       = 3,
    P4_CalcUnary        = 4,
    P4_CalcPrimary      = 5,
    P4_CalcInteger      = 6,
    P4_CalcAddSign      = 7,
    P4_CalcMinusSign    = 8,
    P4_CalcMulSign      = 9,
    P4_CalcDivSign      = 10,
    P4_CalcWhitespace   = 11,
    P4_CalcEol          = 12,
} P4_CalcRuleID;

P4_Grammar*  P4_CreateCalcGrammar() {
    return P4_LoadGrammar(
        "statement = term eol;\n"

        "@nonterminal\n"
        "term = factor ((add / minus) factor)*;\n"

        "@nonterminal\n"
        "factor = unary ((mul / div) unary)*;\n"

        "@nonterminal\n"
        "unary = add unary / minus unary / primary;\n"

        "@lifted\n"
        "primary = integer / \"(\" term \")\";\n"

        "@squashed @tight\n"
        "integer = [0-9]+;\n"

        "add = \"+\";\n"
        "minus = \"-\";\n"
        "mul = \"*\";\n"
        "div = \"/\";\n"

        "@spaced @lifted\n"
        "whitespace = \" \" / \"\\t\";\n"

        "eol = \";\";\n"
    );
}

P4_Error P4_CalcEval(P4_Node* node, long* result) {
    P4_Error err = P4_Ok;
    P4_Node* tmp = NULL;
    char sign   = '+';
    long val = 0;
    char* intstr = NULL;

    if (strcmp(node->rule_name, "statement") == 0) {
        return P4_CalcEval(node->head, result);
    } else if (strcmp(node->rule_name, "term") == 0 || strcmp(node->rule_name, "factor") == 0) {
        if ((err = P4_CalcEval(node->head, &val)) != P4_Ok)
            return err;
        *result = val;
        for (tmp = node->head->next; tmp != NULL; tmp = tmp->next) {
            if (strcmp(tmp->rule_name, "add") == 0)
                sign = '+';
            else if (strcmp(tmp->rule_name, "minus") == 0)
                sign = '-';
            else if (strcmp(tmp->rule_name, "mul") == 0)
                sign = '*';
            else if (strcmp(tmp->rule_name, "div") == 0)
                sign = '/';
            else {
                if ((err = P4_CalcEval(tmp, &val)) != P4_Ok)
                    return err;
                if (sign == '+')
                    *result += val;
                else if (sign == '-')
                    *result -= val;
                else if (sign == '*')
                    *result *= val;
                else if (sign == '/') {
                    if (val == 0) return P4_ValueError;
                    *result /= val;
                }
            }
        }
        return P4_Ok;
    } else if (strcmp(node->rule_name, "unary") == 0) {
        if (node->head == node->tail)
            return P4_CalcEval(node->head, result);
        else {
            long val = 0;
            if ((err = P4_CalcEval(node->tail, &val)) != P4_Ok)
                return err;
            if (strcmp(node->head->rule_name, "add") == 0)
                *result = val;
            else if (strcmp(node->head->rule_name, "minus") == 0)
                *result = -val;
            else
                return P4_ValueError;
            return P4_Ok;
        }
    } else if (strcmp(node->rule_name, "integer") == 0) {
        intstr = P4_CopyNodeString(node);
        *result = atol(intstr);
        free(intstr);
        return P4_Ok;
    } else {
        return P4_ValueError;

    }
}

P4_Grammar*  P4_CreateCalcGrammarLowLevel() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    P4_Error err = P4_Ok;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcStatement, "statement", 3,
        P4_CreateStartOfInput(),
        P4_CreateReference("term"),
        P4_CreateEndOfInput()
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcTerm, "term", 2,
        P4_CreateReference("factor"),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference("add"),
                    P4_CreateReference("minus")
                ),
                P4_CreateReference("factor")
            )
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcFactor, "factor", 2,
        P4_CreateReference("unary"),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference("mul"),
                    P4_CreateReference("div")
                ),
                P4_CreateReference("unary")
            )
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcUnary, "unary", 3,
        P4_CreateSequenceWithMembers(2, P4_CreateReference("add"), P4_CreateReference("unary")),
        P4_CreateSequenceWithMembers(2, P4_CreateReference("minus"), P4_CreateReference("unary")),
        P4_CreateReference("primary")
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcMinusSign, "minus", "-", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcAddSign, "add", "+", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcMulSign, "mul", "*", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcDivSign, "div", "/", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcPrimary, "primary", 2,
        P4_CreateReference("integer"),
        P4_CreateSequenceWithMembers(3,
            P4_CreateLiteral("(", true),
            P4_CreateReference("term"),
            P4_CreateLiteral(")", true)
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddOnceOrMore(grammar, P4_CalcInteger, "integer", P4_CreateRange('0', '9', 1))) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcEol, "eol", 3,
        P4_CreateLiteral("\n", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral(";", true)
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcWhitespace, "whitespace", 2,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true)
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, "integer", P4_FLAG_TIGHT | P4_FLAG_SQUASHED)) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, "primary", P4_FLAG_LIFTED)) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, "whitespace", P4_FLAG_SPACED | P4_FLAG_LIFTED)) != P4_Ok)
        goto finalize;

    return grammar;

finalize:
    P4_DeleteGrammar(grammar);
    return NULL;
}

#ifdef __cplusplus
}
#endif

# endif
