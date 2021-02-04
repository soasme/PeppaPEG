/*
 * PeppaPeg -  A C-Implementation PEG Parser.
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
*/

# ifndef P4_H
# define P4_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

# define P4_PUBLIC(type) type
# define P4_PRIVATE(type) static type

# define P4_MAJOR_VERSION 0
# define P4_MINOR_VERSION 1
# define P4_PATCH_VERSION 0

# define P4_FLAG_NONE                   ((uint8_t)(0x0))

/*
 * Let the children tokens disappear!
 *
 * AST:
 *     a   ===>  a
 *    b c
 *   d e
 * */
# define P4_FLAG_SQUASHED               ((uint8_t)(0x1))

/*
 * Replace the token with the children tokens.
 *
 * AST:
 *     a   ===>   a
 *   (b) c ===> d e c
 *   d e
 * */
# define P4_FLAG_LIFTED                 ((uint8_t)(0x10))

/*
 * Apply implicit whitespaces rule.
 * Used by repetition and sequence expressions.
 */
# define P4_FLAG_TIGHTED                ((uint8_t)(0x100))

/* A position in the text. */
typedef size_t          P4_Position;

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
    P4_Repeat,
} P4_ExpressionKind;

typedef enum {
    /* When there is an internal error. Please raise an issue: https://github.com/soasme/peppapeg/issues. */
    P4_InternalError = 1,
    /* When no text is matched. */
    P4_MatchError,
    /* When no name is resolved. */
    P4_NameError,
    /* When the given value is of unexpected type. */
    P4_ValueError,
    /* When the index is out of boundary. */
    P4_IndexError,
    /* When the id is out of the table. */
    P4_KeyError,
    /* When the parse gets stuck forever or has reached the end. */
    P4_AdvanceError,
    /* When out of memory. */
    P4_MemoryError,
    /* When null is encountered. */
    P4_NullError,
} P4_Error;

typedef struct P4_Expression {
    P4_RuleID                       id;
    P4_ExpressionKind               kind;
    uint8_t                         flags;
    struct P4_Expression*           prev;
    struct P4_Expression*           next;
    union {
        size_t                      num;
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
            P4_Rune                 lower;
            P4_Rune                 upper;
        };

        /* Used by P4_Sequence..P4_Choice. */
        struct {
            struct P4_Expression**  members;
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

typedef struct P4_Grammar {
    /* A P4_Choice expression that includes all rules. */
    P4_Expression*      rules;
    /* The error code. */
    P4_Error            err;
    /* The error message. */
    P4_String           errmsg;
    /* The implicit whitespaces rule expression. */
    P4_Expression*      whitespaces;
} P4_Grammar;

typedef struct P4_Token{
    /* The full text. */
    P4_String           text;
    /* The matched substring.
     * slice[0] is the beginning (inclusive), and slice[1] is the end (exclusive).
     */
    P4_Slice            slice;
    /* The matching grammar expression. */
    P4_Expression*      expr;
    /* The sibling token. NULL if not exists. */
    struct P4_Token*    next;
    /* The first child of inner tokens. NULL if not exists. */
    struct P4_Token*    head;
    /* The last child of inner tokens. NULL if not exists. */
    struct P4_Token*    tail;
} P4_Token;

P4_PUBLIC(P4_String)      P4_Version(void);

P4_PUBLIC(P4_Expression*) P4_CreateNumeric(size_t);
P4_PUBLIC(P4_Expression*) P4_CreateLiteral(const P4_String, bool sensitive);
P4_PUBLIC(P4_Expression*) P4_CreateRange(P4_Rune, P4_Rune);
P4_PUBLIC(P4_Expression*) P4_CreateReference(P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_CreatePositive(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateNegative(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateSequence(size_t, ...);
P4_PUBLIC(P4_Expression*) P4_CreateChoice(size_t, ...);
P4_PUBLIC(P4_Expression*) P4_CreateRepeat(P4_Expression*, uint64_t, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMin(P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMax(P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatExact(P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrOnce(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrMore(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateOnceOrMore(P4_Expression*);

P4_PUBLIC(P4_Expression**) P4_AddMember(P4_Expression*, P4_Expression*);
P4_PUBLIC(size_t)         P4_GetMembersCount(P4_Expression*);

P4_PUBLIC(void)           P4_DeleteExpression(P4_Expression*);

P4_PUBLIC(P4_String)      P4_PrintExpression(P4_Expression*);

P4_PUBLIC(void)           P4_SetRuleID(P4_Expression*, P4_RuleID);
P4_PUBLIC(bool)           P4_IsRule(P4_Expression*);

P4_PUBLIC(bool)           P4_IsSquashed(P4_Expression*);
P4_PUBLIC(bool)           P4_IsLifted(P4_Expression*);
P4_PUBLIC(bool)           P4_IsTighted(P4_Expression*);
P4_PUBLIC(void)           P4_SetExpressionFlag(P4_Expression*, uint8_t);
P4_PUBLIC(void)           P4_SetSquashed(P4_Expression*);
P4_PUBLIC(void)           P4_SetLifted(P4_Expression*);
P4_PUBLIC(void)           P4_SetTighted(P4_Expression*);
P4_PUBLIC(void)           P4_UnsetExpressionFlag(P4_Expression*, uint8_t);
P4_PUBLIC(void)           P4_UnsetSquashed(P4_Expression*);
P4_PUBLIC(void)           P4_UnsetLifted(P4_Expression*);
P4_PUBLIC(void)           P4_UnsetTighted(P4_Expression*);

P4_PUBLIC(P4_Grammar*)    P4_CreateGrammar(void);
P4_PUBLIC(void)           P4_DeleteGrammar(P4_Grammar*);

P4_PUBLIC(void)           P4_AddGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(void)           P4_AddNumeric(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC(P4_Expression*) P4_AddLiteral(P4_Grammar*, P4_RuleID, const P4_String, bool sensitive);
P4_PUBLIC(P4_Expression*) P4_AddRange(P4_Grammar*, P4_RuleID, P4_Rune, P4_Rune);
P4_PUBLIC(P4_Expression*) P4_AddReference(P4_Grammar*, P4_RuleID, P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_AddPositive(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_AddNegative(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_AddSequence(P4_Grammar*, P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_AddChoice(P4_Grammar*, P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_AddZeroOrOnce(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_AddZeroOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_AddOnceOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_AddRepeatMin(P4_Grammar*, P4_RuleID, P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_AddRepeatMax(P4_Grammar*, P4_RuleID, P4_Expression*, uint64_t);
P4_PUBLIC(P4_Expression*) P4_AddRepeatMinMax(P4_Grammar*, P4_RuleID, P4_Expression*, uint64_t, uint64_t);
P4_PUBLIC(P4_Expression*) P4_AddRepeatExact(P4_Grammar*, P4_RuleID, P4_Expression*, uint64_t);

P4_PUBLIC(void)           P4_DeleteGrammarRule(P4_Grammar*, P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_GetGrammarRule(P4_Grammar*, P4_RuleID);

P4_PUBLIC(bool)           P4_HasError(P4_Grammar*);
P4_PUBLIC(P4_String)      P4_PrintError(P4_Grammar*);
P4_PUBLIC(void)           P4_SetError(P4_Grammar*, P4_Error, P4_String);

P4_PUBLIC(P4_Token*)      P4_Parse(P4_Grammar*, P4_RuleID, P4_String input);
P4_PUBLIC(P4_Token*)      P4_ParseWithLength(P4_Grammar*, P4_RuleID, P4_String input, P4_Position offset);

P4_PUBLIC(void)           P4_SetWhitespaces(P4_Grammar*, P4_Expression* space, P4_Expression* comment);
P4_PUBLIC(P4_Expression*) P4_GetWhitespaces(P4_Grammar*);

P4_PUBLIC(void)           P4_GrammarReset(P4_Grammar*);

P4_PUBLIC(P4_Token*)      P4_CreateToken(P4_String, P4_Slice, P4_Expression*);
P4_PUBLIC(void)           P4_DeleteToken(P4_Token*);
P4_PUBLIC(void)           P4_AppendToken(P4_Token*, P4_Token*);
P4_PUBLIC(void)           P4_AdoptToken(P4_Token*, P4_Token*);

P4_PUBLIC(P4_String)      P4_PrintToken(P4_Token*);

#ifdef __cplusplus
}
#endif

# endif
