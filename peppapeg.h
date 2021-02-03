# ifndef P4_H
# define P4_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

# define P4_PUBLIC(type) type

# define P4_MAJOR_VERSION 0
# define P4_MINOR_VERSION 1
# define P4_PATCH_VERSION 0

# define P4_FLAG_NONE                   0x0

/*
 * Let the children tokens disappear!
 *
 * AST:
 *     a   ===>  a
 *    b c
 *   d e
 * */
# define P4_FLAG_SQUASH_CHILDREN        0x1

/*
 * Replace the token with the children tokens.
 *
 * AST:
 *     a   ===>   a
 *   (b) c ===> d e c
 *   d e
 * */
# define P4_FLAG_LIFT_CHILDREN          0x10

/*
 * Apply implicit whitespaces rule.
 * Used by repetition and sequence expressions.
 */
# define P4_FLAG_TIGHT                  0x100

/* A position in the text. */
typedef uint64_t        P4_Position;

/* A slice of position 0 to position 1. */
typedef P4_Position     P4_Slice[2];

/* An UTF8 rune */
typedef uint32_t        P4_Rune;

/* Shortcut for a string. */
typedef char*           P4_String;

/* The identifier of a rule expression. */
typedef uint32_t        P4_RuleID;

typedef enum {
    P4_Numeric,
    P4_Literal,
    P4_Range,
    P4_Reference,
    P4_Positive,
    P4_Negative,
    P4_Sequence,
    P4_Choice,
    P4_ZeroOrOnce,
    P4_ZeroOrMore,
    P4_OnceOrMore,
    P4_RepeatMin,
    P4_RepeatMax,
    P4_RepeatMinMax,
    P4_RepeatExact,
} P4_ExpressionKind;

typedef struct P4_Expression {
    P4_RuleID                       id;
    P4_ExpressionKind               kind;
    uint8_t                         flags;
    struct P4_Expression*           next;
    union {
        uint64_t                    num;
        /* Used by P4_Literal. */
        struct {
            P4_String               literal;
            bool                    sensitive;
        };

        /* Used by P4_Reference..P4_Negative. */
        P4_RuleID                   refid;
        struct P4_Expression*       refexpr;

        /* Used by P4_Range. */
        struct {
            P4_Rune                 start;
            P4_Rune                 end;
        };

        /* Used by P4_Sequence..P4_Choice. */
        struct {
            struct P4_Expression*   members;
            uint32_t                count;
        };

        /* Used by P4_ZeroOrOnce..P4_RepeatExact . */
        struct {
            struct P4_Expression*   repeat;
            uint64_t                min;
            uint64_t                max;
        };
    };
} P4_Expression;

typedef struct P4_State {
    P4_String       text;
    P4_Position     pos;
    P4_Expression*  exprs;
} P4_State;

typedef struct P4_Token{
    /* The full text. */
    P4_String           text;
    /* The matched substring.
     * slice[0] is the beginning (inclusive), and slice[1] is the end (exclusive).
     */
    P4_Slice            slice;
    /* The matching grammar expression. */
    P4_Expression       expr;
    /* The sibling token. NULL if not exists. */
    struct P4_Token*    next;
    /* The first child of inner tokens. NULL if not exists. */
    struct P4_Token*    head;
    /* The last child of inner tokens. NULL if not exists. */
    struct P4_Token*    tail;
} P4_Token;

P4_PUBLIC(P4_String)  P4_Version(void);

P4_PUBLIC(P4_Expression*) P4_CreateNumeric(uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateLiteral(const P4_String, bool sensitive);
P4_PUBLIC(P4_Expression*) P4_CreateRange(P4_Rune, P4_Rune);
P4_PUBLIC(P4_Expression*) P4_CreateReference(P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_CreatePositive(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateNegative(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateSequence(void);
P4_PUBLIC(P4_Expression*) P4_CreateChoice(void);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrOnce(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrMore(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateOnceOrMore(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMin(P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMax(P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMinMax(P4_Expression*, uint64_t, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatExact(P4_Expression*, uint64_t);
P4_PUBLIC(void)           P4_AddMember(P4_Expression*, P4_Expression*);

P4_PUBLIC(void)           P4_Delete(P4_Expression*);

P4_PUBLIC(P4_String)      P4_Print(P4_Expression*);

#ifdef __cplusplus
}
#endif

# endif
