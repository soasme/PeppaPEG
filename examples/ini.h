# ifndef P4_LANG_INI_H
# define P4_LANG_INI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../peppapeg.h"

typedef enum {
    P4_INI_Entry = 1,
    P4_INI_Row,
    P4_INI_Section,
    P4_INI_Property,
    P4_INI_Newline,
    P4_INI_Name,
    P4_INI_Value,
    P4_INI_Char,
    P4_INI_Whitespace,
} P4_INIRuleID;

P4_PUBLIC(P4_Grammar*)  P4_CreateINIGrammar() {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        return NULL;
    }

    // Entry = POSITIVE([1-0x10ffff]) Row* NEGATIVE([1-0x10ffff])

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_INI_Entry, 3,
        P4_CreatePositive(P4_CreateRange(0x1, 0x10ffff)), // start of input
        P4_CreateZeroOrMore(P4_CreateReference(P4_INI_Row)),
        P4_CreateNegative(P4_CreateRange(0x1, 0x10ffff)) // end of input
    ))
        goto finalize;

    // Row {LIFTED,TIGHT} = (Section / Property)? Newline
    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_INI_Row, 2,
        P4_CreateZeroOrOnce(
            P4_CreateChoiceWithMembers(2,
                P4_CreateReference(P4_INI_Section),
                P4_CreateReference(P4_INI_Property)
            )
        ),
        P4_CreateReference(P4_INI_Newline)
    ))
        goto finalize;

    P4_SetExpressionFlag(P4_GetGrammarRule(grammar, P4_INI_Row), P4_FLAG_LIFTED | P4_FLAG_TIGHT);

    // Section = "[" Name "]"

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_INI_Section, 3,
        P4_CreateLiteral("[", true),
        P4_CreateReference(P4_INI_Name),
        P4_CreateLiteral("]", true)
    ))
        goto finalize;

    // Property = Name "=" Value

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_INI_Property, 3,
        P4_CreateReference(P4_INI_Name),
        P4_CreateLiteral("=", true),
        P4_CreateReference(P4_INI_Value)
    ))
        goto finalize;

    // Name = Char+

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_INI_Name, P4_CreateReference(P4_INI_Char)))
        goto finalize;

    // Value = Char*

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_INI_Value, P4_CreateReference(P4_INI_Char)))
        goto finalize;

    // Char = [a-z] / [A-Z] / [0-9] / " " / "." / ":" / "?" / "_" / "/"

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_INI_Char, 9,
        P4_CreateRange('a', 'z'),
        P4_CreateRange('A', 'Z'),
        P4_CreateRange('0', '9'),
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral(".", true),
        P4_CreateLiteral(":", true),
        P4_CreateLiteral("?", true),
        P4_CreateLiteral("_", true),
        P4_CreateLiteral("/", true)
    ))
        goto finalize;

    // Newline {LIFTED} = "\n"
    if (P4_Ok != P4_AddLiteral(grammar, P4_INI_Newline, "\n", true))
        goto finalize;

    P4_SetExpressionFlag(
        P4_GetGrammarRule(grammar, P4_INI_Newline),
        P4_FLAG_LIFTED
    );

    // Whitespace {LIFTED,SPACED} = " " / "\t" / "\n"
    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_INI_Whitespace, 3,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true),
        P4_CreateLiteral("\n", true)
    ))
        goto finalize;

    P4_SetExpressionFlag(
        P4_GetGrammarRule(grammar, P4_INI_Whitespace),
        P4_FLAG_LIFTED | P4_FLAG_SPACED
    );

    return grammar;

finalize:
    P4_DeleteGrammar(grammar);
    return NULL;
}

#ifdef __cplusplus
}
#endif

# endif

