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

typedef struct P4_State {
    P4_String       text;
    P4_Position     pos;
} P4_State;

typedef struct P4_Expression {
    P4_RuleID           id;
    P4_ExpressionKind   kind;
    uint8_t             flags;
    union {
        /* Used by P4_Literal. */
        struct {
            P4_String               literal;
            bool                    sensitive;
        };

        /* Used by P4_Reference..P4_Negative. */
        P4_RuleID                   ref;

        /* Used by P4_Range. */
        struct {
            P4_Position             start;
            P4_Position             end;
        };

        /* Used by P4_Sequence..P4_Choice. */
        struct {
            struct P4_Expression*   members;
            uint32_t                count;
        };

        /* Used by P4_ZeroOrOnce..P4_RepeatExact . */
        struct {
            struct P4_Expression*   repeat;
            P4_Position             min;
            P4_Position             max;
        };
    };
} P4_Expression;

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

#ifdef __cplusplus
}
#endif

# endif
