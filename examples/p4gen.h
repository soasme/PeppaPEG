# ifndef P4_LANG_PEPPA_H
# define P4_LANG_PEPPA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

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

#ifdef __cplusplus
}
#endif

# endif
