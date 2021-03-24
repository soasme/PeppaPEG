# ifndef P4_LANG_PEPPA_H
# define P4_LANG_PEPPA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "peppapeg.h"

typedef enum {
    P4_PeppaEntry           = 1,
    P4_PeppaRule            = 2,
    P4_PeppaRuleDecorators  = 3,
    P4_PeppaRuleName        = 4,
    P4_PeppaRuleDefinition  = 5
    P4_PeppaDecorator       = 6,
    P4_PeppaExpression,
    P4_PeppaIdentifier,
    P4_PeppaLiteral,
    P4_PeppaRange,
    P4_PeppaRangeLower,
    P4_PeppaRangeUpper,
    P4_PeppaReference,
    P4_PeppaPositive,
    P4_PeppaNegative,
    P4_PeppaSequence,
    P4_PeppaChoice,
    P4_PeppaBackReference,
    P4_PeppaZeroOrMore,
    P4_PeppaZeroOrOnce,
    P4_PeppaOnceOrMore,
    P4_PeppaRepeat,
    P4_PeppaNumber,
    P4_PeppaChar,
};

#ifdef __cplusplus
}
#endif

# endif
