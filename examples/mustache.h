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

#include "../peppa.h"

P4_Error P4_MustacheCallback(P4_Grammar* grammar, P4_Expression* rule, P4_Node* node) {
    if (rule
            && P4_GetRuleName(rule)
            && strcmp(P4_GetRuleName(rule), "tag") == 0
            && node
            && node->head
            && node->head->next
            && strcmp(node->head->next->rule_name, "set_delimiter") == 0) {
        P4_String opener = NULL, closer = NULL;
        P4_Expression* opener_expr = NULL;
        P4_Expression* closer_expr = NULL;

        opener = P4_CopyNodeString(node->head->next->head);
        if (opener == NULL)
            goto finalize_set_delimiter;

        closer = P4_CopyNodeString(node->head->next->tail);
        if (closer == NULL)
            goto finalize_set_delimiter;

        opener_expr = P4_CreateLiteral(opener, true);
        if (opener_expr == NULL)
            goto finalize_set_delimiter;

        closer_expr = P4_CreateLiteral(closer, true);
        if (closer_expr == NULL)
            goto finalize_set_delimiter;

        P4_Error err = P4_Ok;
        if ((err = P4_ReplaceGrammarRule(grammar, "opener", opener_expr)) != P4_Ok)
            goto finalize_set_delimiter;

        if ((err = P4_ReplaceGrammarRule(grammar, "closer", closer_expr)) != P4_Ok)
            goto finalize_set_delimiter;

        node->head->rule_name  = P4_GetRuleName(opener_expr);
        node->tail->rule_name  = P4_GetRuleName(closer_expr);

        P4_FREE(opener);
        P4_FREE(closer);

        return P4_Ok;

finalize_set_delimiter:
        P4_FREE(opener);
        P4_FREE(closer);
        P4_DeleteExpression(opener_expr);
        P4_DeleteExpression(closer_expr);
        return P4_MemoryError;
    }

    return P4_Ok;
}

/*
 * Entry = SOI Line*
 * Line = (Tag / Text)* (NewLine / EOI) # FLAG: TIGHT, Callback: UpdateIndent, TrimNodes
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

P4_Grammar*  P4_CreateMustacheGrammar() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, "whitespace", 2,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "whitespace",
        P4_FLAG_SPACED | P4_FLAG_LIFTED
    ))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, "opener", "{{", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, "closer", "}}", true))
        goto finalize;

    if (P4_Ok != P4_AddZeroOrMore(grammar, "non_closer",
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference("closer"),
                    P4_CreateReference("newline")
                )
            ),
            P4_CreateRange(1, 0x10ffff, 1)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "non_closer", P4_FLAG_SQUASHED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "tag", 3,
        P4_CreateReference("opener"),
        P4_CreateReference("tag_content"),
        P4_CreateReference("closer")
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "tag", P4_FLAG_SCOPED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "set_delimiter", 4,
        P4_CreateLiteral("=", true),
        P4_CreateReference("new_opener"),
        P4_CreateReference("new_closer"),
        P4_CreateLiteral("=", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, "new_opener",
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(2,
                    P4_CreateReference("whitespace"),
                    P4_CreateReference("closer")
                )
            ),
            P4_CreateRange('!', 0x10ffff, 1) /* ! ~ */
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "new_opener", P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, "new_closer",
        P4_CreateSequenceWithMembers(2,
            P4_CreateNegative(
                P4_CreateChoiceWithMembers(3,
                    P4_CreateReference("whitespace"),
                    P4_CreateReference("closer"),
                    P4_CreateLiteral("=", true)
                )
            ),
            P4_CreateRange('!', 0x10ffff, 1) /* ! ~ */
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "new_closer", P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "comment", 2,
        P4_CreateLiteral("!", true),
        P4_CreateReference("non_closer")
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "unescaped", 2,
        P4_CreateLiteral("&", true),
        P4_CreateReference("non_closer")
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "triple_unescaped", 3,
        P4_CreateLiteral("{", true),
        P4_CreateReference("non_closer"),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "section_open", 2,
        P4_CreateChoiceWithMembers(2,
            P4_CreateLiteral("#", true),
            P4_CreateLiteral("^", true)
        ),
        P4_CreateReference("non_closer")
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "section_close", 2,
        P4_CreateLiteral("/", true),
        P4_CreateReference("non_closer")
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "partial", 2,
        P4_CreateLiteral(">", true),
        P4_CreateReference("non_closer")
    ))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, "variable", "non_closer"))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, "tag_content", 8,
        P4_CreateReference("set_delimiter"),
        P4_CreateReference("comment"),
        P4_CreateReference("unescaped"),
        P4_CreateReference("triple_unescaped"),
        P4_CreateReference("section_open"),
        P4_CreateReference("section_close"),
        P4_CreateReference("partial"),
        P4_CreateReference("variable")
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "tag_content", P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, "newline", 2,
        P4_CreateLiteral("\n", true),
        P4_CreateLiteral("\r\n", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "newline", P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "text", 2,
        P4_CreateZeroOrMore(
            P4_CreateSequenceWithMembers(2,
                P4_CreateNegative(
                    P4_CreateChoiceWithMembers(2,
                        P4_CreateReference("opener"),
                        P4_CreateReference("newline")
                    )
                ),
                P4_CreateRange(1, 0x10ffff, 1) /* ANY */
            )
        ),
        P4_CreateZeroOrOnce(P4_CreateReference("newline"))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "new_closer", P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "line", 2,
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(2,
                P4_CreateReference("tag"),
                P4_CreateReference("text")
            )
        ),
        P4_CreateChoiceWithMembers(2,
            P4_CreateReference("newline"),
            P4_CreateReference("eoi")
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "line", P4_FLAG_TIGHT | P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddGrammarRule(grammar, "soi", P4_CreateStartOfInput()))
        goto finalize;

    if (P4_Ok != P4_AddGrammarRule(grammar, "eoi", P4_CreateEndOfInput()))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, "entry", 2,
        P4_CreateReference("soi"),
        P4_CreateZeroOrMore(P4_CreateReference("line"))
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_SetGrammarCallback(grammar, &P4_MustacheCallback, NULL))
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


