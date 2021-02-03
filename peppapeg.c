#include <stdio.h>
#include "peppapeg.h"

typedef struct P4_State {
    /* The input text. */
    P4_String       text;
    /* The size of input text. Shortcut for strlen(s->text). */
    size_t          size;
    /* The depth of nested AST. */
    size_t          depth;
    /* The parsed position of the input text. */
    P4_Position     pos;
    /* The root expr to parse. */
    P4_Expression*  expr;
    /* The error message when error occurs. */
    P4_String       errmsg;
} P4_State;

P4_PRIVATE(P4_Token*)   P4_Match(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchNumeric(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchLiteral(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchRange(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchReference(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchPositive(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchNegative(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchSequence(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchChoice(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchZeroOrOnce(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchZeroOrMore(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchOnceOrMore(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchRepeatMin(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchRepeatMax(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchRepeatMinMax(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)   P4_MatchRepeatExact(P4_State*, P4_Expression*);

P4_PUBLIC(P4_String)
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

