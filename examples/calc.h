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
 *      $ gcc examples/calc.c peppa.c && ./a.out
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
#include "../peppa.h"

P4_Grammar*  P4_CreateCalcGrammar() {
    return P4_LoadGrammar(
        "statement = term eol;\n"

        "@nonterminal\n"
        "term = factor | term  (add / minus) factor;\n"

        "@nonterminal\n"
        "factor = unary | factor (mul / div) unary;\n"

        "@nonterminal\n"
        "unary = primary / (add / minus)? unary;\n"

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

        "eol = \"\\n\" / !.;\n"
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
    } else if (strcmp(node->rule_name, "term") == 0) {
        if ((err = P4_CalcEval(node->head, &val)) != P4_Ok)
            return err;

        *result = val;

        if ((err = P4_CalcEval(node->tail, &val)) != P4_Ok)
            return err;

        if (strcmp(node->head->next->rule_name, "add") == 0) {
            *result += val;
        } else if (strcmp(node->head->next->rule_name, "minus") == 0) {
            *result -= val;
        } else {
            return P4_ValueError;
        }

        return P4_Ok;
    } else if (strcmp(node->rule_name, "factor") == 0) {
        if ((err = P4_CalcEval(node->head, &val)) != P4_Ok)
            return err;

        *result = val;

        if ((err = P4_CalcEval(node->tail, &val)) != P4_Ok)
            return err;

        if (strcmp(node->head->next->rule_name, "mul") == 0) {
            *result *= val;
        } else if (strcmp(node->head->next->rule_name, "div") == 0) {
            if (val == 0) {
                return P4_ValueError;
            }
            *result /= val;
        } else {
            return P4_ValueError;
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
        P4_FREE(intstr);
        return P4_Ok;
    } else {
        return P4_ValueError;

    }
}

#ifdef __cplusplus
}
#endif

# endif
