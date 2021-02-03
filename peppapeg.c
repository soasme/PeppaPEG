#include <ctype.h>
#include <stdio.h>
#include <string.h>
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

/*
 * Get version string.
 */
P4_PUBLIC(P4_String)
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

/*
 * Reads a single UTF-8 code point from the string.
 * Returns the number of bytes of this code point.
 *
 * Example::
 *
 *     > uint32_t c = 0x0
 *     > read_codepoint("你好", &c)
 *     3
 *     > printf("%p %d\n", c, c)
 *     0x4f60 20320
 */
P4_PRIVATE(inline size_t)
P4_ReadRune(P4_String s, P4_Rune* c) {
    if ((s[0] & 0b10000000) == 0) { // 1 byte code point, ASCII
        *c = (s[0] & 0b01111111);
        return 1;
    } else if ((s[0] & 0b11100000) == 0b11000000) { // 2 byte code point
        *c = (s[0] & 0b00011111) << 6 | (s[1] & 0b00111111);
        return 2;
    } else if ((s[0] & 0b11110000) == 0b11100000) { // 3 byte code point
        *c = (s[0] & 0b00001111) << 12 | (s[1] & 0b00111111) << 6 | (s[2] & 0b00111111);
        return 3;
    } else if ((s[0] & 0b11111000) == 0b11110000) { // 4 byte code point
        *c = (s[0] & 0b00000111) << 18 | (s[1] & 0b00111111) << 12 | (s[2] & 0b00111111) << 6 | (s[3] & 0b00111111);
        return 4;
    } else {
        *c = 0x0;
        return 0;
    }
}

/*
 * Compare case-insensitive string src v/s dest.
 *
 * Like strcmp, but works for a case insensitive UTF-8 string.
 */
P4_PRIVATE(inline int)
caseicmp(P4_String src, P4_String dst, size_t len) {
    uint32_t srcch = 0x0, dstch = 0x0;
    size_t srcsz = 0, dstsz = 0;
    int cmp = 0, remaining = len;
    if (strlen(dst) < len) return -1;
    while (*src != 0x0 && *dst != 0x0) {
        srcsz = P4_ReadRune(src, &srcch);
        dstsz = P4_ReadRune(dst, &dstch);
        if (srcsz < dstsz) return -1;
        if (srcsz > dstsz) return 1;
        if (srcsz != 1 && srcch != dstch) return srcch > dstch ? 1 : -1;
        cmp = tolower(srcch) - tolower(dstch);
        if (srcsz == 1 && cmp != 0) return cmp;
        src = src + srcsz;
        dst = dst + dstsz;
        remaining--;
        if (remaining == 0) return 0;
    }
    return 0;
}


