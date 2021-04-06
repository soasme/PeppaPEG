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

P4_Error P4_CalcEval(P4_Token* token, long* result) {
    P4_Error err = P4_Ok;
    P4_Token* tmp = NULL;
    char sign   = '+';
    long val = 0;
    char* intstr = NULL;

    switch (token->rule_id) {
        case P4_CalcStatement:
            return P4_CalcEval(token->head, result);
        case  P4_CalcTerm:
        case  P4_CalcFactor:
            if ((err = P4_CalcEval(token->head, &val)) != P4_Ok)
                return err;
            *result = val;
            for (tmp = token->head->next; tmp != NULL; tmp = tmp->next) {
                switch (tmp->rule_id) {
                    case P4_CalcAddSign:
                        sign = '+'; break;
                    case P4_CalcMinusSign:
                        sign = '-'; break;
                    case P4_CalcMulSign:
                        sign = '*'; break;
                    case P4_CalcDivSign:
                        sign = '/'; break;
                    default:
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
                        break;
                }
            }
            return P4_Ok;
        case P4_CalcUnary:
            if (token->head == token->tail)
                return P4_CalcEval(token->head, result);
            else {
                long val = 0;
                if ((err = P4_CalcEval(token->tail, &val)) != P4_Ok)
                    return err;
                if (token->head->rule_id == P4_CalcAddSign)
                    *result = val;
                else if (token->head->rule_id == P4_CalcMinusSign)
                    *result = -val;
                else
                    return P4_ValueError;
                return P4_Ok;
            }
        case P4_CalcInteger:
            intstr = P4_CopyTokenString(token);
            *result = atol(intstr);
            free(intstr);
            return P4_Ok;
        default:
            return P4_ValueError;
    }
}

P4_Grammar*  P4_CreateCalcGrammar() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    P4_Error err = P4_Ok;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcStatement, 3,
        P4_CreateStartOfInput(),
        P4_CreateReference(P4_CalcTerm),
        P4_CreateEndOfInput()
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcTerm, 2,
        P4_CreateReference(P4_CalcFactor),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference(P4_CalcAddSign),
                    P4_CreateReference(P4_CalcMinusSign)
                ),
                P4_CreateReference(P4_CalcFactor)
            )
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddSequenceWithMembers(grammar, P4_CalcFactor, 2,
        P4_CreateReference(P4_CalcUnary),
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference(P4_CalcMulSign),
                    P4_CreateReference(P4_CalcDivSign)
                ),
                P4_CreateReference(P4_CalcUnary)
            )
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcUnary, 3,
        P4_CreateSequenceWithMembers(2, P4_CreateReference(P4_CalcAddSign), P4_CreateReference(P4_CalcUnary)),
        P4_CreateSequenceWithMembers(2, P4_CreateReference(P4_CalcMinusSign), P4_CreateReference(P4_CalcUnary)),
        P4_CreateReference(P4_CalcPrimary)
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcMinusSign, "-", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcAddSign, "+", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcMulSign, "*", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddLiteral(grammar, P4_CalcDivSign, "/", true)) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcPrimary, 2,
        P4_CreateReference(P4_CalcInteger),
        P4_CreateSequenceWithMembers(3,
            P4_CreateLiteral("(", true),
            P4_CreateReference(P4_CalcTerm),
            P4_CreateLiteral(")", true)
        )
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddOnceOrMore(grammar, P4_CalcInteger, P4_CreateRange('0', '9', 1))) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcEol, 3,
        P4_CreateLiteral("\n", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral(";", true)
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_AddChoiceWithMembers(grammar, P4_CalcWhitespace, 2,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true)
    )) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, P4_CalcInteger, P4_FLAG_TIGHT | P4_FLAG_SQUASHED)) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, P4_CalcPrimary, P4_FLAG_LIFTED)) != P4_Ok)
        goto finalize;

    if ((err = P4_SetGrammarRuleFlag(grammar, P4_CalcWhitespace, P4_FLAG_SPACED | P4_FLAG_LIFTED)) != P4_Ok)
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
