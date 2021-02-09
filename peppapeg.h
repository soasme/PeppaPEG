/*
 * Peppa PEG -  Ultra lightweight PEG Parser in ANSI C.
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

#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

# define P4_PUBLIC(type) type
# define P4_PRIVATE(type) static type

# define P4_MAJOR_VERSION 1
# define P4_MINOR_VERSION 0
# define P4_PATCH_VERSION 0

# define P4_FLAG_NONE                   ((uint32_t)(0x0))

/*
 * Let the children tokens disappear!
 *
 * AST:
 *     a   ===>  a
 *    b c
 *   d e
 * */
# define P4_FLAG_SQUASHED               ((uint32_t)(0x1))

/*
 * Replace the token with the children tokens.
 *
 * AST:
 *     a   ===>   a
 *   (b) c ===> d e c
 *   d e
 * */
# define P4_FLAG_LIFTED                 ((uint32_t)(0x10))

/*
 * Apply implicit whitespaces rule.
 * Used by repetition and sequence expressions.
 */
# define P4_FLAG_TIGHT                ((uint32_t)(0x100))

/*
 * Apply the expression under a new scope, regardless
 * if the parent expression is lifted/tighted.
 */
# define P4_FLAG_SCOPED                 ((uint32_t)(0x1000))

/*
 * Use the space expression in expression of sequence/repeat kind
 * if they're not tight.
 */
# define P4_FLAG_SPACED                 ((uint32_t)(0x10000))

/* The flag of expression. */
typedef uint32_t        P4_ExpressionFlag;

/* A position in the text. */
typedef size_t          P4_Position;

/* A slice in the text. */
typedef struct P4_Slice {
    /* The start position of the slice. */
    P4_Position         i;
    /* The stop position of the slice. */
    P4_Position         j;
}                       P4_Slice;

/* An UTF8 rune */
typedef uint32_t        P4_Rune;

/* Shortcut for a string. */
typedef char*           P4_String;

/* The identifier of a rule expression. */
typedef uint64_t        P4_RuleID;

/* A range of two runes. */
typedef P4_Rune P4_RuneRange[2];

/* The expression kind. */
typedef enum {
    // P4_Numeric,
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
    /* No error is like a bless. */
    P4_Ok               = 0,
    /* When there is an internal error.
     * Please raise an issue: https://github.com/soasme/peppapeg/issues. */
    P4_InternalError    = 1,
    /* When no text is matched. */
    P4_MatchError       = 2,
    /* When no name is resolved. */
    P4_NameError        = 3,
    /* When the parse gets stuck forever or has reached the end. */
    P4_AdvanceError     = 4,
    /* When out of memory. */
    P4_MemoryError      = 5,
    /* When the given value is of unexpected type. */
    P4_ValueError       = 6,
    /* When the index is out of boundary. */
    P4_IndexError       = 7,
    /* When the id is out of the table. */
    P4_KeyError         = 8,
    /* When null is encountered. */
    P4_NullError        = 9,
} P4_Error;

typedef struct P4_Source {
    struct P4_Grammar*      grammar;
    P4_RuleID               rule_id;
    P4_String               content;
    P4_Position             pos;
    P4_Error                err;
    P4_String               errmsg;
    struct P4_Token*        root;
    struct P4_Expression**  frames;
    size_t                  frames_len;
    size_t                  frames_cap;
    bool                    verbose;
    bool                    whitespacing;

    // to be removed.
    struct P4_Expression*   implicit_whitespace;
    struct P4_Expression*   whitespace;
    struct P4_Expression*   comment;
} P4_Source;

typedef struct P4_Expression {
    // The name of expression (only for debugging).
    // P4_String            name;
    /* The id of expression. */
    P4_RuleID            id;
    /* The kind of expression. */
    P4_ExpressionKind     kind;
    /* The flag of expression. */
    P4_ExpressionFlag      flag;

    union {
        /* Used by P4_Numeric. */
        size_t                      num;

        /* Used by P4_Literal. */
        struct {
            P4_String               literal;
            bool                    sensitive;
        };

        /* Used by P4_Reference..P4_Negative. */
        struct {
            P4_String               reference;
            P4_RuleID               ref_id;
            struct P4_Expression*   ref_expr;
        };

        /* Used by P4_Range. */
        P4_RuneRange                range;

        /* Used by P4_Sequence..P4_Choice. */
        struct {
            struct P4_Expression**  members;
            size_t                  count;
        };

        /* Used by P4_ZeroOrOnce..P4_RepeatExact.
         * repeat the expr for n times, n >= min and n <= max. */
        struct {
            struct P4_Expression*   repeat_expr; // maybe we can merge it with ref_expr?
            size_t                  repeat_min;
            size_t                  repeat_max;
        };
    };
} P4_Expression;


typedef struct P4_Token {
    /* the full text. */
    P4_String               text;
    /* The matched substring.
     * slice[0] is the beginning (inclusive), and slice[1] is the end (exclusive).
     */
    P4_Slice                slice;

    /* The matched grammar expression. */
    struct P4_Expression*   expr;

    /* the sibling token. NULL if not exists. */
    struct P4_Token*        next;
    /* the first child of inner tokens. NULL if not exists. */
    struct P4_Token*        head;
    /* the last child of inner tokens. NULL if not exists. */
    struct P4_Token*        tail;
} P4_Token;


typedef struct P4_Grammar{
    /* the rules, e.g. the expressions with IDs. */
    struct P4_Expression**  rules;
    /* the total number of rules. */
    int                     count;
    /* the maximum number of rules. */
    int                     cap;
    /* the total number of spaced rules. */
    size_t                  spaced_count;
    /* the repetition rule for spaced rules. */
    struct P4_Expression*   spaced_rules;
} P4_Grammar;


P4_PUBLIC(P4_String)      P4_Version(void);


// P4_PUBLIC(P4_Expression*) P4_CreateNumeric(size_t);
P4_PUBLIC(P4_Expression*) P4_CreateLiteral(const P4_String, bool sensitive);
P4_PUBLIC(P4_Expression*) P4_CreateRange(P4_Rune, P4_Rune);
P4_PUBLIC(P4_Expression*) P4_CreateReference(P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_CreatePositive(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateNegative(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateSequence(size_t, ...);
P4_PUBLIC(P4_Expression*) P4_CreateChoice(size_t, ...);
P4_PUBLIC(P4_Expression*) P4_CreateRepeat(P4_Expression*, size_t, size_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMin(P4_Expression*, size_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatMax(P4_Expression*, size_t);
P4_PUBLIC(P4_Expression*) P4_CreateRepeatExact(P4_Expression*, size_t);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrOnce(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateZeroOrMore(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_CreateOnceOrMore(P4_Expression*);

P4_PUBLIC(P4_Error)       P4_SetMember(P4_Expression*, size_t, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_SetReferenceMember(P4_Expression*, size_t, P4_RuleID);
P4_PUBLIC(size_t)         P4_GetMembersCount(P4_Expression*);
P4_PUBLIC(P4_Expression*) P4_GetMember(P4_Expression*, size_t);

P4_PUBLIC(void)           P4_DeleteExpression(P4_Expression*);

P4_PUBLIC(P4_String)      P4_PrintExpression(P4_Expression*);

P4_PUBLIC(void)           P4_SetRuleID(P4_Expression*, P4_RuleID);
P4_PUBLIC(bool)           P4_IsRule(P4_Expression*);

P4_PUBLIC(bool)           P4_IsSquashed(P4_Expression*);
P4_PUBLIC(bool)           P4_IsLifted(P4_Expression*);
P4_PUBLIC(bool)           P4_IsTight(P4_Expression*);
P4_PUBLIC(bool)           P4_IsScoped(P4_Expression*);
P4_PUBLIC(bool)           P4_IsSpaced(P4_Expression*);
P4_PUBLIC(void)           P4_SetExpressionFlag(P4_Expression*, P4_ExpressionFlag);
P4_PUBLIC(void)           P4_SetSquashed(P4_Expression*);
P4_PUBLIC(void)           P4_SetLifted(P4_Expression*);
P4_PUBLIC(void)           P4_SetTight(P4_Expression*);
P4_PUBLIC(void)           P4_SetSpaced(P4_Expression*);
P4_PUBLIC(void)           P4_SetScoped(P4_Expression*);

P4_PUBLIC(P4_Grammar*)    P4_CreateGrammar(void);
P4_PUBLIC(void)           P4_DeleteGrammar(P4_Grammar*);

P4_PUBLIC(size_t)         P4_AddGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(void)           P4_AddNumeric(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC(P4_Error)       P4_AddLiteral(P4_Grammar*, P4_RuleID, const P4_String, bool sensitive);
P4_PUBLIC(P4_Error)       P4_AddRange(P4_Grammar*, P4_RuleID, P4_Rune, P4_Rune);
P4_PUBLIC(P4_Error)       P4_AddReference(P4_Grammar*, P4_RuleID, P4_RuleID);
P4_PUBLIC(P4_Error)       P4_AddPositive(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_AddNegative(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_AddSequence(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC(P4_Error)       P4_AddChoice(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC(P4_Error)       P4_AddZeroOrOnce(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_AddZeroOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_AddOnceOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC(P4_Error)       P4_AddRepeatMin(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_PUBLIC(P4_Error)       P4_AddRepeatMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_PUBLIC(P4_Error)       P4_AddRepeatMinMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t, size_t);
P4_PUBLIC(P4_Error)       P4_AddRepeatExact(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);

P4_PUBLIC(void)           P4_DeleteGrammarRule(P4_Grammar*, P4_RuleID);
P4_PUBLIC(P4_Expression*) P4_GetGrammarRule(P4_Grammar*, P4_RuleID);

P4_PUBLIC(bool)           P4_HasError(P4_Grammar*);
P4_PUBLIC(P4_String)      P4_PrintError(P4_Grammar*);
P4_PUBLIC(void)           P4_SetError(P4_Grammar*, P4_Error, P4_String);

P4_PUBLIC(P4_Source*)     P4_CreateSource(P4_String, P4_RuleID);
P4_PUBLIC(void)           P4_DeleteSource(P4_Source*);

P4_PUBLIC(P4_Token*)      P4_GetSourceAst(P4_Source*);
P4_PUBLIC(void)           P4_PrintSourceAst(P4_Token*, P4_String, size_t);

P4_PUBLIC(P4_Error)       P4_Parse(P4_Grammar*, P4_Source*);

P4_PUBLIC(P4_Error)       P4_SetWhitespaces(P4_Grammar*);
P4_PUBLIC(P4_Expression*) P4_GetWhitespaces(P4_Grammar*);

P4_PUBLIC(void)           P4_GrammarReset(P4_Grammar*);

P4_PUBLIC(P4_Token*)      P4_CreateToken(P4_String, size_t, size_t, P4_Expression*);
P4_PUBLIC(void)           P4_DeleteToken(P4_Token*);
P4_PUBLIC(void)           P4_AppendToken(P4_Token*, P4_Token*);
P4_PUBLIC(P4_Slice*)      P4_GetTokenSlice(P4_Token*);
P4_PUBLIC(P4_String)      P4_CopyTokenString(P4_Token*);

#ifdef __cplusplus
}
#endif

# endif
