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

# ifndef P4_LANG_MUSTACHE_H
# define P4_LANG_MUSTACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

typedef enum {
    P4_JSONEntry        = 1,
    P4_JSONWhitespace,
    P4_JSONObject,
    P4_JSONObjectItem,
    P4_JSONArray,
    P4_JSONString,
    P4_JSONTrue,
    P4_JSONFalse,
    P4_JSONNull,
    P4_JSONNumber,
    P4_JSONEscape,
    P4_JSONUnicodeEscape,
    P4_JSONMinus,
    P4_JSONPlus,
    P4_JSONIntegral,
    P4_JSONFractional,
    P4_JSONExponent,
} P4_JSONRuleID;

/*
 * Entry = Whitespace* (Object / Array / String / True / False / Null / Number) Whitespace* # LIFTED
 * Array = "[" Entry ("," Entry)* "]" / "[" "]"
 * Object = "{" ObjectItem ("," ObjectItem)* "}" / "{" "}"
 * ObjectItem = String ":" Entry
 * String = "\"" ([0x20-0x21] / [0x23-0x7f] / Escape)* "\""
 * Escape = "\\" ("\"" / "/" / "\\" / "b" / "f" / "n" / "r" / "t" / UnicodeEscape)
 * UnicodeEscape = "u" ([0-9][a-f][A-F]){4}
 * True = "true"
 * False = "false"
 * Null = "null"
 * Number = Minus? Integral Fractional? Exponent?
 * Minus = "-"
 * Plus = "+"
 * Integral = "0" / [1-9] [0-9]+
 * Fractional = "." [0-9]+
 * Exponent = i"E" (Minus / Plus)? [0-9]+
 * Whitespace = " " / "\t" / "\r" / "\n"
 */
P4_PUBLIC(P4_Grammar*)  P4_CreateJSONGrammar() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    if (P4_Ok != P4_AddLiteral(grammar, P4_JSONMinus, "-", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_JSONPlus, "+", true))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_JSONIntegral, 2,
        P4_CreateLiteral("0", true),
        P4_CreateSequenceWithMembers(2,
            P4_CreateRange('1', '9'),
            P4_CreateZeroOrMore(P4_CreateRange('0', '9'))
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONIntegral, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONFractional, 2,
        P4_CreateLiteral(".", true),
        P4_CreateOnceOrMore(P4_CreateRange('0', '9'))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONFractional, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONExponent, 3,
        P4_CreateLiteral("e", false),
        P4_CreateZeroOrOnce(
            P4_CreateChoiceWithMembers(2,
                P4_CreateReference(P4_JSONPlus),
                P4_CreateReference(P4_JSONMinus)
            )
        ),
        P4_CreateOnceOrMore(P4_CreateRange('0', '9'))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONExponent, P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONNumber, 4,
        P4_CreateZeroOrOnce(P4_CreateReference(P4_JSONMinus)),
        P4_CreateReference(P4_JSONIntegral),
        P4_CreateZeroOrOnce(P4_CreateReference(P4_JSONFractional)),
        P4_CreateZeroOrOnce(P4_CreateReference(P4_JSONExponent))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONNumber, P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_JSONNull, "null", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_JSONFalse, "false", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_JSONTrue, "true", true))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONUnicodeEscape, 2,
        P4_CreateLiteral("u", true),
        P4_CreateRepeatExact(
            P4_CreateChoiceWithMembers(3,
                P4_CreateRange('0', '9'),
                P4_CreateRange('a', 'f'),
                P4_CreateRange('A', 'F')
            ), 4
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONUnicodeEscape, P4_FLAG_TIGHT | P4_FLAG_SQUASHED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONEscape, 2,
        P4_CreateLiteral("\\", true),
        P4_CreateChoiceWithMembers(9,
            P4_CreateLiteral("\"", true),
            P4_CreateLiteral("/", true),
            P4_CreateLiteral("\\", true),
            P4_CreateLiteral("b", true),
            P4_CreateLiteral("f", true),
            P4_CreateLiteral("n", true),
            P4_CreateLiteral("r", true),
            P4_CreateLiteral("t", true),
            P4_CreateReference(P4_JSONUnicodeEscape)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONUnicodeEscape, P4_FLAG_TIGHT | P4_FLAG_SQUASHED | P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONString, 3,
        P4_CreateLiteral("\"", true),
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(3,
                P4_CreateRange(0x20, 0x21),
                /* 0x22: " */
                P4_CreateRange(0x23, 0x7f),
                P4_CreateReference(P4_JSONEscape)
            )
        ),
        P4_CreateLiteral("\"", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONString, P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_JSONArray, 2,
        P4_CreateSequenceWithMembers(4,
            P4_CreateLiteral("[", true),
            P4_CreateReference(P4_JSONEntry),
            P4_CreateZeroOrMore(
                P4_CreateSequenceWithMembers(2,
                    P4_CreateLiteral(",", true),
                    P4_CreateReference(P4_JSONEntry)
                )
            ),
            P4_CreateLiteral("]", true)
        ),
        P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("[", true),
            P4_CreateLiteral("]", true)
        )
    ))
        goto finalize;


    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONObjectItem, 3,
        P4_CreateReference(P4_JSONString),
        P4_CreateLiteral(":", true),
        P4_CreateReference(P4_JSONEntry)
    ))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_JSONObject, 2,
        P4_CreateSequenceWithMembers(4,
            P4_CreateLiteral("{", true),
            P4_CreateReference(P4_JSONObjectItem),
            P4_CreateZeroOrMore(
                P4_CreateSequenceWithMembers(2,
                    P4_CreateLiteral(",", true),
                    P4_CreateReference(P4_JSONObjectItem)
                )
            ),
            P4_CreateLiteral("}", true)
        ),
        P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("{", true),
            P4_CreateLiteral("}", true)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_JSONEntry, 3,
        P4_CreateZeroOrMore(P4_CreateReference(P4_JSONWhitespace)),
        P4_CreateChoiceWithMembers(7,
            P4_CreateReference(P4_JSONObject),
            P4_CreateReference(P4_JSONArray),
            P4_CreateReference(P4_JSONString),
            P4_CreateReference(P4_JSONTrue),
            P4_CreateReference(P4_JSONFalse),
            P4_CreateReference(P4_JSONNull),
            P4_CreateReference(P4_JSONNumber)
        ),
        P4_CreateZeroOrMore(P4_CreateReference(P4_JSONWhitespace))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONEntry, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_JSONWhitespace, 4,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral("\n", true),
        P4_CreateLiteral("\t", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_JSONWhitespace, P4_FLAG_SPACED | P4_FLAG_LIFTED))
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
