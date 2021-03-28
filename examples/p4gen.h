# ifndef P4_LANG_PEPPA_H
# define P4_LANG_PEPPA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include "../peppapeg.h"

# define SLICE_LEN(s) ((s)->stop.pos - (s)->start.pos)

typedef enum {
    P4_P4GenGrammar         = 1,
    P4_P4GenRule            = 2,
    P4_P4GenRuleDecorators  = 3,
    P4_P4GenRuleName        = 4,
    P4_P4GenExpression,
    P4_P4GenPrimary,
    P4_P4GenDecorator,
    P4_P4GenIdentifier,
    P4_P4GenLiteral,
    P4_P4GenRange,
    P4_P4GenReference,
    P4_P4GenPositive,
    P4_P4GenNegative,
    P4_P4GenSequence,
    P4_P4GenChoice,
    P4_P4GenBackReference,
    P4_P4GenRepeat,
    P4_P4GenRepeatOnceOrMore,
    P4_P4GenRepeatZeroOrMore,
    P4_P4GenRepeatZeroOrOnce,
    P4_P4GenRepeatMin,
    P4_P4GenRepeatMax,
    P4_P4GenRepeatExact,
    P4_P4GenRepeatMinMax,
    P4_P4GenNumber,
    P4_P4GenChar,
    P4_P4GenWhitespace,
} P4_P4GenRuleID;

P4_Grammar* P4_CreateP4GenGrammar ();
P4_String   P4_P4GenKindToName(P4_RuleID);

P4_Grammar* P4_CreateP4GenGrammar () {
    P4_Grammar* grammar = P4_CreateGrammar();
    P4_Error err = 0;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_P4GenNumber, 2,
        P4_CreateLiteral("0", true),
        P4_CreateSequenceWithMembers(2,
            P4_CreateRange('1', '9', 1),
            P4_CreateZeroOrMore(P4_CreateRange('0', '9', 1))
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenNumber, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_P4GenChar, 4,
        P4_CreateRange(0x20, 0x21, 1), /* Can't be 0x22: double quote " */
        P4_CreateRange(0x23, 0x5b, 1), /* Can't be 0x5c: escape leading \ */
        P4_CreateRange(0x5d, 0x10ffff, 1),
        P4_CreateSequenceWithMembers(2,
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
                P4_CreateSequenceWithMembers(2,
                    P4_CreateLiteral("u", true),
                    P4_CreateRepeatExact(
                        P4_CreateChoiceWithMembers(3,
                            P4_CreateRange('0', '9', 1),
                            P4_CreateRange('a', 'f', 1),
                            P4_CreateRange('A', 'F', 1)
                        ), 4
                    )
                )
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenChar, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenLiteral, 3,
        P4_CreateLiteral("\"", true),
        P4_CreateZeroOrMore(P4_CreateReference(P4_P4GenChar)),
        P4_CreateLiteral("\"", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenLiteral, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRange, 6,
        P4_CreateLiteral("[", true),
        P4_CreateReference(P4_P4GenChar),
        P4_CreateLiteral("-", true),
        P4_CreateReference(P4_P4GenChar),
        P4_CreateZeroOrOnce(P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("..", true),
            P4_CreateReference(P4_P4GenNumber)
        )),
        P4_CreateLiteral("]", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenReference, 2,
        P4_CreateChoiceWithMembers(3,
            P4_CreateRange('a', 'z', 1),
            P4_CreateRange('A', 'Z', 1),
            P4_CreateLiteral("_", true)
        ),
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(4,
                P4_CreateRange('a', 'z', 1),
                P4_CreateRange('A', 'Z', 1),
                P4_CreateRange('0', '9', 1),
                P4_CreateLiteral("_", true)
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenReference, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenPositive, 2,
        P4_CreateLiteral("&", true),
        P4_CreateReference(P4_P4GenPrimary)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenNegative, 2,
        P4_CreateLiteral("!", true),
        P4_CreateReference(P4_P4GenPrimary)
    ))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_P4GenRepeatOnceOrMore, "+", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_P4GenRepeatZeroOrMore, "*", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_P4GenRepeatZeroOrOnce, "?", true))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRepeatMin, 4,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_P4GenNumber),
        P4_CreateLiteral(",", true),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRepeatMax, 4,
        P4_CreateLiteral("{", true),
        P4_CreateLiteral(",", true),
        P4_CreateReference(P4_P4GenNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRepeatMinMax, 5,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_P4GenNumber),
        P4_CreateLiteral(",", true),
        P4_CreateReference(P4_P4GenNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRepeatExact, 3,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_P4GenNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRepeat, 2,
        P4_CreateReference(P4_P4GenPrimary),
        P4_CreateZeroOrOnce(
            P4_CreateChoiceWithMembers(7,
                P4_CreateReference(P4_P4GenRepeatOnceOrMore),
                P4_CreateReference(P4_P4GenRepeatZeroOrMore),
                P4_CreateReference(P4_P4GenRepeatZeroOrOnce),
                P4_CreateReference(P4_P4GenRepeatExact),
                P4_CreateReference(P4_P4GenRepeatMinMax),
                P4_CreateReference(P4_P4GenRepeatMin),
                P4_CreateReference(P4_P4GenRepeatMax)
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenRepeat, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_P4GenPrimary, 6,
        P4_CreateReference(P4_P4GenLiteral),
        P4_CreateReference(P4_P4GenRange),
        P4_CreateSequenceWithMembers(2,
            P4_CreateReference(P4_P4GenReference),
            P4_CreateNegative(P4_CreateLiteral("=", true))
        ),
        P4_CreateReference(P4_P4GenPositive),
        P4_CreateReference(P4_P4GenNegative),
        P4_CreateSequenceWithMembers(3,
            P4_CreateLiteral("(", true),
            P4_CreateReference(P4_P4GenChoice),
            P4_CreateLiteral(")", true)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenPrimary, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddJoin(grammar, P4_P4GenChoice, "/", P4_P4GenSequence))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenChoice, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_P4GenSequence,
                P4_CreateReference(P4_P4GenRepeat)))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenSequence, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, P4_P4GenExpression, P4_P4GenChoice))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenExpression, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, P4_P4GenRuleName, P4_P4GenReference))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenRuleName, P4_FLAG_SQUASHED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenDecorator, 2,
        P4_CreateLiteral("@", true),
        P4_CreateChoiceWithMembers(6,
            P4_CreateLiteral("squashed", true),
            P4_CreateLiteral("scoped", true),
            P4_CreateLiteral("spaced", true),
            P4_CreateLiteral("lifted", true),
            P4_CreateLiteral("tight", true),
            P4_CreateLiteral("nonterminal", true)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_AddZeroOrMore(grammar, P4_P4GenRuleDecorators,
            P4_CreateReference(P4_P4GenDecorator)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_P4GenRule, 5,
        P4_CreateReference(P4_P4GenRuleDecorators),
        P4_CreateReference(P4_P4GenRuleName),
        P4_CreateLiteral("=", true),
        P4_CreateReference(P4_P4GenExpression),
        P4_CreateLiteral(";", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_P4GenGrammar, P4_CreateReference(P4_P4GenRule)))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_P4GenWhitespace, 4,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral("\n", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_P4GenWhitespace,
                P4_FLAG_LIFTED | P4_FLAG_SPACED))
        goto finalize;

    return grammar;

finalize:
    P4_DeleteGrammar(grammar);
    return NULL;
}

P4_String   P4_P4GenKindToName(P4_RuleID id) {
    switch (id) {
        case P4_P4GenNumber: return "number";
        case P4_P4GenChar: return "char";
        case P4_P4GenLiteral: return "literal";
        case P4_P4GenRange: return "range";
        case P4_P4GenReference: return "reference";
        case P4_P4GenPositive: return "positive";
        case P4_P4GenNegative: return "negative";
        case P4_P4GenChoice: return "choice";
        case P4_P4GenSequence: return "sequence";
        case P4_P4GenRepeat: return "repeat";
        case P4_P4GenRepeatOnceOrMore: return "onceormore";
        case P4_P4GenRepeatZeroOrMore: return "zeroormore";
        case P4_P4GenRepeatZeroOrOnce: return "zerooronce";
        case P4_P4GenRepeatMin: return "repeatmin";
        case P4_P4GenRepeatMax: return "repeatmax";
        case P4_P4GenRepeatMinMax: return "repeatminmax";
        case P4_P4GenRepeatExact: return "repeatexact";
        case P4_P4GenRuleName: return "name";
        case P4_P4GenRuleDecorators: return "decorators";
        case P4_P4GenDecorator: return "decorator";
        case P4_P4GenRule: return "rule";
        case P4_P4GenGrammar: return "grammar";
        default: return "<unknown>";
    }
}

P4_Error P4_P4GenEvalFlag(P4_Token* token, P4_ExpressionFlag *flag) {
    P4_String token_str = token->text + token->slice.start.pos; /* XXX: need slice api. */
    size_t token_len = SLICE_LEN(&token->slice); /* XXX: need slice api. */

    if (memcmp("@squashed", token_str, token_len) == 0)
        *flag = P4_FLAG_SQUASHED;
    else if (memcmp("@scoped", token_str, token_len) == 0)
        *flag = P4_FLAG_SCOPED;
    else if (memcmp("@spaced", token_str, token_len) == 0)
        *flag = P4_FLAG_SPACED;
    else if (memcmp("@lifted", token_str, token_len) == 0)
        *flag = P4_FLAG_LIFTED;
    else if (memcmp("@tight", token_str, token_len) == 0)
        *flag = P4_FLAG_TIGHT;
    else if (memcmp("@nonterminal", token_str, token_len) == 0)
        *flag = P4_FLAG_NON_TERMINAL;
    else {
        *flag = 0; return P4_ValueError;
    }

    return P4_Ok;
}

P4_Error P4_P4GenEvalRuleFlags(P4_Token* token, P4_ExpressionFlag* flag) {
    P4_Token* child = NULL;
    P4_ExpressionFlag child_flag = 0;
    P4_Error err = P4_Ok;
    for (child = token->head; child != NULL; child = child->next) {
        if ((err = P4_P4GenEvalFlag(child, &child_flag)) != P4_Ok) {
            *flag = 0;
            return err;
        }
        *flag |= child_flag;
    }
    return P4_Ok;
}

P4_Error P4_P4GenEvalNumber(P4_Token* token, long* num) {
    P4_String s = P4_CopyTokenString(token);

    if (s == NULL)
        return P4_MemoryError;

    *num = atol(s);
    free(s);

    return P4_Ok;
}


P4_Error P4_P4GenEvalChar(P4_Token* token, P4_Rune* rune) {
    size_t len = SLICE_LEN(&token->slice);
    *rune = 0;
    switch(len) {
        case 0:
            return P4_NullError;
        case 1: /* single char */
            *rune = (P4_Rune) token->text[token->slice.start.pos];
            return P4_Ok;
        case 2: /* utf-8 rune, escaped char */
        {
            if (token->text[token->slice.start.pos] == '\\') {
                switch(token->text[token->slice.start.pos+1]) {
                    case 'b': *rune = 0x8; break;
                    case 't': *rune = 0x9; break;
                    case 'n': *rune = 0xa; break;
                    case 'f': *rune = 0xc; break;
                    case 'r': *rune = 0xd; break;
                    case '"': *rune = 0x22; break;
                    case '/': *rune = 0x2f; break;
                    case '\\': *rune = 0x5c; break;
                    default: return P4_ValueError;
                }
            } else {
                if (P4_ReadRune(token->text+token->slice.start.pos, rune) != 2)
                    return P4_ValueError;
            }
            return P4_Ok;
        }
        case 3: /* utf-8 rune, escaped char */
            if (P4_ReadRune(token->text+token->slice.start.pos, rune) != 3)
                return P4_ValueError;
            return P4_Ok;
        case 4: /* utf-8 rune, escaped char */
            if (P4_ReadRune(token->text+token->slice.start.pos, rune) != 4)
                return P4_ValueError;
            return P4_Ok;
        case 6:
        {
            if (token->text[token->slice.start.pos] != '\\') return P4_ValueError;
            if (token->text[token->slice.start.pos+1] != 'u') return P4_ValueError;
            char cp[5] = {0, 0, 0, 0, 0};
            memcpy(cp, token->text + token->slice.start.pos + 2, 4);
            *rune = strtoul(cp, NULL, 16);
            return P4_Ok;
        }
        default:
            return P4_ValueError;
    }
}

P4_Error P4_P4GenEval(P4_Token* token, void* result) {
    P4_Error err = P4_Ok;
    switch (token->rule_id) {
        case P4_P4GenDecorator:
            return P4_P4GenEvalFlag(token, result);
        case P4_P4GenRuleDecorators:
            return P4_P4GenEvalRuleFlags(token, result);
        case P4_P4GenNumber:
            return P4_P4GenEvalNumber(token, result);
        case P4_P4GenChar:
            return P4_P4GenEvalChar(token, result);
        default: return P4_ValueError;
    }
    return P4_Ok;
}

#ifdef __cplusplus
}
#endif

# endif
