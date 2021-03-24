# ifndef P4_LANG_PEPPA_H
# define P4_LANG_PEPPA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "peppapeg.h"

typedef enum {
    P4_P4GenEntry           = 1,
    P4_P4GenRule            = 2,
    P4_P4GenRuleDecorators  = 3,
    P4_P4GenRuleName        = 4,
    P4_P4GenRuleDefinition  = 5,
    P4_P4GenDecorator       = 6,
    P4_P4GenExpression,
    P4_P4GenIdentifier,
    P4_P4GenLiteral,
    P4_P4GenRange,
    P4_P4GenRangeLower,
    P4_P4GenRangeUpper,
    P4_P4GenReference,
    P4_P4GenPositive,
    P4_P4GenNegative,
    P4_P4GenSequence,
    P4_P4GenChoice,
    P4_P4GenBackReference,
    P4_P4GenZeroOrMore,
    P4_P4GenZeroOrOnce,
    P4_P4GenOnceOrMore,
    P4_P4GenRepeat,
    P4_P4GenNumber,
    P4_P4GenChar,
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
        default: return "<unknown>";
    }
}

#ifdef __cplusplus
}
#endif

# endif
