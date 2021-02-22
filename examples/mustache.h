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
 * Example: Write a Mustache Parser using Peppa PEG.
 *
 * TODO:
 *
 * + Dynamically set delimiter and trim tokens using rule callback.
 * + Test mustache grammar using mustache official JSON test cases.
*/

# ifndef P4_LANG_MUSTACHE_H
# define P4_LANG_MUSTACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

typedef enum {
    P4_MustacheEntry            = 1,
    P4_MustacheLine             = 2,
    P4_MustacheText             = 3,
    P4_MustacheNewLine          = 4,
    P4_MustacheSOI              = 5,
    P4_MustacheEOI              = 6,
    P4_MustacheTag              = 7,
    P4_MustacheTagContent       = 8,
    P4_MustacheSetDelimiter     = 9,
    P4_MustacheComment          = 10,
    P4_MustacheUnescaped        = 11,
    P4_MustacheTripleUnescaped  = 12,
    P4_MustacheSectionOpen      = 13,
    P4_MustacheSectionClose     = 14,
    P4_MustachePartial          = 15,
    P4_MustacheVariable         = 16,
    P4_MustacheOpener           = 17,
    P4_MustacheCloser           = 18,
    P4_MustacheNewOpener        = 19,
    P4_MustacheNewCloser        = 20,
    P4_MustacheNonCloser        = 21,
    P4_MustacheWhitespace       = 22,
} P4_MustacheRuleID;

/*
 * Entry = SOI Line*
 * Line = (Tag / Text)* (NewLine / EOI) # FLAG: TIGHT, Callback: UpdateIndent, TrimTokens
 * Text = (NEGATIVE(Opener / NewLine) ANY)* Newline? # FLAG: ATOMIC, TIGHT
 * NewLine = "\n" / "\r\n"
 * Tag = Opener TagContent Closer # Callback: SetDelimiter.
 * TagContent = SetDelimiter / Comment / Unescaped / TripleUnescaped / SectionOpen / SectionClose / Partial / Variable
 * SetDelimiter = "="  NewOpener NewCloser "="
 * NewOpener = (NEGATIVE(Whitespace / Closer) ANY)+ # FLAG: SQUASHED, TIGHT
 * NewCloser = (NEGATIVE(Whitespace / "=" / Closer) ANY)+ # FLAG: SQUASHED, TIGHT
 * Comment = "!" NonCloser
 * Unescaped = "&" NonCloser
 * TripleUnescaped = "{" NonCloser "}"
 * SectionOpen = ("#" / "^") NonCloser
 * SectionClose = "/" NonCloser
 * Partial = ">" NonCloser
 * Variable = NonCloser
 * Opener = "{{"
 * Closer = "}}"
 * NonCloser = (NEGATIVE(Closer) ANY)* # FLAG: SQUASHED
 * Whitespace = " " / "\t" # FLAG: SPACED | LIFTED
 */

P4_PUBLIC P4_Grammar*  P4_CreateMustacheGrammar() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_MustacheWhitespace, 2,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheWhitespace,
        P4_FLAG_SPACED | P4_FLAG_LIFTED
    ))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_MustacheOpener, "{{", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_MustacheCloser, "}}", true))
        goto finalize;

    if (P4_Ok != P4_AddZeroOrMore(grammar, P4_MustacheNonCloser,
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference(P4_MustacheCloser),
                    P4_CreateReference(P4_MustacheNewLine)
                )
            ),
            P4_CreateRange(1, 0x10ffff)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheNonCloser, P4_FLAG_SQUASHED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheTag, 3,
        P4_CreateReference(P4_MustacheOpener),
        P4_CreateReference(P4_MustacheTagContent),
        P4_CreateReference(P4_MustacheCloser)
    ))
        goto finalize;


    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheSetDelimiter, 4,
        P4_CreateLiteral("=", true),
        P4_CreateReference(P4_MustacheNewOpener),
        P4_CreateReference(P4_MustacheNewCloser),
        P4_CreateLiteral("=", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_MustacheNewOpener,
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference(P4_MustacheWhitespace),
                    P4_CreateReference(P4_MustacheCloser)
                )
            ),
            P4_CreateRange('!', 0x10ffff) /* ! ~ */
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheNewOpener, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_MustacheNewCloser,
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(3,
                    P4_CreateReference(P4_MustacheWhitespace),
                    P4_CreateReference(P4_MustacheCloser),
                    P4_CreateLiteral("=", true)
                )
            ),
            P4_CreateRange('!', 0x10ffff) /* ! ~ */
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheNewCloser, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheComment, 2,
        P4_CreateLiteral("!", true),
        P4_CreateReference(P4_MustacheNonCloser)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheUnescaped, 2,
        P4_CreateLiteral("&", true),
        P4_CreateReference(P4_MustacheNonCloser)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheTripleUnescaped, 3,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_MustacheNonCloser),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheSectionOpen, 2,
        P4_CreateChoiceWithMembers(2,
            P4_CreateLiteral("#", true),
            P4_CreateLiteral("^", true)
        ),
        P4_CreateReference(P4_MustacheNonCloser)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheSectionClose, 2,
        P4_CreateLiteral("/", true),
        P4_CreateReference(P4_MustacheNonCloser)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustachePartial, 2,
        P4_CreateLiteral(">", true),
        P4_CreateReference(P4_MustacheNonCloser)
    ))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, P4_MustacheVariable, P4_MustacheNonCloser))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_MustacheTagContent, 8,
        P4_CreateReference(P4_MustacheSetDelimiter),
        P4_CreateReference(P4_MustacheComment),
        P4_CreateReference(P4_MustacheUnescaped),
        P4_CreateReference(P4_MustacheTripleUnescaped),
        P4_CreateReference(P4_MustacheSectionOpen),
        P4_CreateReference(P4_MustacheSectionClose),
        P4_CreateReference(P4_MustachePartial),
        P4_CreateReference(P4_MustacheVariable)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheTagContent, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_MustacheNewLine, 2,
        P4_CreateLiteral("\n", true),
        P4_CreateLiteral("\r\n", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheNewLine, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheText, 2,
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateNegative(
                    P4_CreateChoiceWithMembers(2,
                        P4_CreateReference(P4_MustacheOpener),
                        P4_CreateReference(P4_MustacheNewLine)
                    )
                ),
                P4_CreateRange(1, 0x10ffff) /* ANY */
            )
        ),
        P4_CreateZeroOrOnce(P4_CreateReference(P4_MustacheNewLine))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheNewCloser, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheLine, 2,
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(2,
                P4_CreateReference(P4_MustacheTag),
                P4_CreateReference(P4_MustacheText)
            )
        ),
        P4_CreateChoiceWithMembers(2,
            P4_CreateReference(P4_MustacheNewLine),
            P4_CreateReference(P4_MustacheEOI)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_MustacheLine, P4_FLAG_TIGHT | P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddPositive(grammar, P4_MustacheSOI, P4_CreateRange(1, 0x10ffff)))
        goto finalize;

    if (P4_Ok != P4_AddNegative(grammar, P4_MustacheEOI, P4_CreateRange(1, 0x10ffff)))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_MustacheEntry, 2,
        P4_CreateReference(P4_MustacheSOI),
        P4_CreateZeroOrMore(P4_CreateReference(P4_MustacheLine))
    ))
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


