.. _tutcalc:

Tutorial: Calculator
====================

In this tutorial, we will write a calculator. It supports

* Positive integers.
* Negative integers.
* Addition operator (+).
* Subtraction operator (-).
* Multiplication operator (*).
* Division operator (/).

The calculator can take arbitrary valid string calc expressions and yield the result.

Step 1: Define RuleIDs
----------------------

Let's create a new file "calc.h" and define some enums as RuleIDs.

.. code-block:: c

    #include <stdio.h>
    #include <stdlib.h>
    #include "peppapeg.h"

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

Step 2: Create Grammar
----------------------

We'll create the grammar using :c:func:`P4_LoadGrammar`.

The order of rules must be same with `P4_CalcRuleID`.

.. code-block:: c

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

To extend a little bit,

* Rule `statement` is the entry.
* Rule `term` can parse inputs like `a + b - c + d ...` or `a`.
* Rule `factor` can parse inputs like `a * b / c * d ...` or `a`.
* Rule `unary` can parse inputs like `+a`, `++a`, `-a`, `a`, etc.
* Rule `primary` will have no corresponding token in the ast.
* Rule `integer` are some squashed digits, which avoids creating tokens for each digit.
* Rule `whitespace` allows arbitrary number of whitespace or tab in between expressions.

Step 3: Eval Ast
----------------

Next, let's evaluate the token tree. We traverse the AST and calculate the result.

.. code-block:: c

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

Step 4: Parse
-------------

The main function calls all the above:

* Create the grammar.
* Create the source.
* Parse the source using the grammar.
* Eval the source ast.
* Print the evaluated result.

.. code-block:: c

    #include "calc.h"

    int main() {
        P4_Grammar* grammar = P4_CreateCalcGrammar();
        P4_Source*  source  = NULL;
        P4_Error    error   = P4_Ok;
        long        result  = 0;
        char        line[256];

        printf("Type statement to continue. Type ^C to quit.\n");

        while (fgets(line, sizeof(line), stdin)) {
            source = P4_CreateSource(line, P4_CalcStatement);
            if ((error = P4_Parse(grammar, source)) != P4_Ok) {
                printf("error: parse: %d\n", error);
            } else if ((error = P4_CalcEval(P4_GetSourceAst(source), &result)) != P4_Ok){
                printf("error: eval: %d\n", error);
            } else {
                printf("[Out] %ld\n\n", result);
            }
            P4_DeleteSource(source);
        }

        P4_DeleteGrammar(grammar);
    }

Run:

.. code-block:: console

    $ gcc -o calc calc.c peppapeg.c && ./calc
    Type statement to continue. Type ^C to quit.

    1+2*3;
    [Out] 7

    -1 + 4/2*3 - 1;
    [Out] 4

    5/0;
    error: eval: 6
