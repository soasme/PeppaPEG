/**
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
 *
 * @file       peppapeg.h
 * @brief      Peppa PEG header file
 * @author     Ju
 * @copyright  MIT
 * @date       2021
 * @see        https://github.com/soasme/PeppaPEG
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

# ifndef P4_MALLOC
# define P4_MALLOC malloc
# endif

# ifndef P4_FREE
# define P4_FREE free
# endif

# ifndef P4_REALLOC
# define P4_REALLOC realloc
# endif

/*
 *
 * ███████╗██╗░░░░░░█████╗░░██████╗░░██████╗
 * ██╔════╝██║░░░░░██╔══██╗██╔════╝░██╔════╝
 * █████╗░░██║░░░░░███████║██║░░██╗░╚█████╗░
 * ██╔══╝░░██║░░░░░██╔══██║██║░░╚██╗░╚═══██╗
 * ██║░░░░░███████╗██║░░██║╚██████╔╝██████╔╝
 * ╚═╝░░░░░╚══════╝╚═╝░░╚═╝░╚═════╝░╚═════╝░
 */

/** Major version number. */
# define P4_MAJOR_VERSION 1

/** Minor version number. */
# define P4_MINOR_VERSION 12

/** Patch version number. */
# define P4_PATCH_VERSION 0

# define P4_FLAG_NONE                   ((uint32_t)(0x0))

/**
 * When the flag is set, the grammar rule will have squash all
 * children tokens.
 *
 * `token->head`, `token->tail` will be NULL.
 *
 * For example, rule b has P4_FLAG_SQUASHED. After parsing,
 * the children tokens d and e are gone:
 *
 *          a   ===>   a
 *        (b) c ===>  b c
 *        d e
 *
 * The flag will impact all of the descendant rules.
 **/
# define P4_FLAG_SQUASHED               ((uint32_t)(0x1))

/**
 * When the flag is set, the grammar rule will replace the
 * token with its children tokens.
 *
 * For example, rule b has P4_FLAG_SQUASHED. After parsing,
 * the token (b) is gone, and its children d and e become
 * the children of a:
 *
 *          a   ===>   a
 *        (b) c ===> d e c
 *        d e
 * */
# define P4_FLAG_LIFTED                 ((uint32_t)(0x10))

/**
 * When the flag is set, the grammar rule will insert
 * no P4_FLAG_SPACED rules inside the sequences and repetitions
 *
 * This flag only works for the repetition and sequence expressions.
 */
# define P4_FLAG_TIGHT                ((uint32_t)(0x100))

/**
 * When the flag is set, the effect of SQUASHED and TIGHT
 * are canceled.
 *
 * Regardless if the ancestor expression has SQUASHED or TIGHT
 * flag, starting from this expression, Peppa PEG will start
 * creating tokens and apply SPACED rules for sequences and repetitions.
 */
# define P4_FLAG_SCOPED                 ((uint32_t)(0x1000))

/**
 * When the flag is set, the expression will be inserted
 * between every token inside the sequences and repetitions
 *
 * If there are multiple SPACED expressions, Peppa PEG will
 * iterate through all SPACED expressions.
 *
 * This flag makes the grammar clean and tidy without inserting
 * whitespace rule everywhere.
 */
# define P4_FLAG_SPACED                 ((uint32_t)(0x10000))

/**
 * When the flag is set and a sequence/repetition token tree
 * has only one child, the child token will replace the parent.
 */
# define P4_FLAG_NON_TERMINAL           ((uint32_t)(0x100000))

/**
 * The default recursion limit.
 *
 * It can be adjusted via function P4_SetRecursionLimit.
 */
# define P4_DEFAULT_RECURSION_LIMIT     8192

/*
 *
 * ███████╗███╗░░██╗██╗░░░██╗███╗░░░███╗░██████╗
 * ██╔════╝████╗░██║██║░░░██║████╗░████║██╔════╝
 * █████╗░░██╔██╗██║██║░░░██║██╔████╔██║╚█████╗░
 * ██╔══╝░░██║╚████║██║░░░██║██║╚██╔╝██║░╚═══██╗
 * ███████╗██║░╚███║╚██████╔╝██║░╚═╝░██║██████╔╝
 * ╚══════╝╚═╝░░╚══╝░╚═════╝░╚═╝░░░░░╚═╝╚═════╝░
 */

/** The expression kind. */
typedef enum {
    /** Rule: Case-Sensitive Literal, Case-Insensitive Literal. */
    P4_Literal,
    /** Rule: Range. */
    P4_Range,
    /** Rule: Reference. */
    P4_Reference,
    /** Rule: Positive. */
    P4_Positive,
    /** Rule: Negative. */
    P4_Negative,
    /** Rule: Sequence. */
    P4_Sequence,
    /** Rule: Case-Sensitive BackReference, Case-Insensitive BackReference. */
    P4_BackReference,
    /** Rule: Choice. */
    P4_Choice,
    /**
     * Rule: RepeatMinMax, RepeatMin, RepeatMax, RepeatExact,
     * OnceOrMore, ZeroOrMore, ZeroOrOnce.
     */
    P4_Repeat,
} P4_ExpressionKind;

/**
 * The error code.
 */
typedef enum {
    /** No error is like a bless. */
    P4_Ok                   = 0,
    /** When there is an internal error.
     * Please raise an issue: https://github.com/soasme/peppapeg/issues.
     * */
    P4_InternalError        = 1,
    /** When no text is matched. */
    P4_MatchError           = 2,
    /** When no name is resolved. */
    P4_NameError            = 3,
    /** When the parse gets stuck forever or has reached the end. */
    P4_AdvanceError         = 4,
    /** When out of memory. */
    P4_MemoryError          = 5,
    /** When the given value is of unexpected type. */
    P4_ValueError           = 6,
    /** When the index is out of boundary. */
    P4_IndexError           = 7,
    /** When the id is out of the table. */
    P4_KeyError             = 8,
    /** When null is encountered. */
    P4_NullError            = 9,
    /** When recursion limit is reached. */
    P4_StackError           = 10,
} P4_Error;

/**
 * The RuleID for Peppa PEG grammar.
 */
typedef enum {
    P4_PegGrammar                   = 1,
    P4_PegRuleRule                  = 2,
    P4_PegRuleRuleDecorators        = 3,
    P4_PegRuleRuleName              = 4,
    P4_PegRuleExpression            = 5,
    P4_PegRulePrimary               = 6,
    P4_PegRuleDecorator             = 7,
    P4_PegRuleIdentifier            = 8,
    P4_PegRuleLiteral               = 9,
    P4_PegRuleInsensitiveLiteral    = 10,
    P4_PegRuleRange                 = 11,
    P4_PegRuleReference             = 12,
    P4_PegRulePositive              = 13,
    P4_PegRuleNegative              = 14,
    P4_PegRuleSequence              = 15,
    P4_PegRuleChoice                = 16,
    P4_PegRuleBackReference         = 17,
    P4_PegRuleRepeat                = 18,
    P4_PegRuleRepeatOnceOrMore      = 19,
    P4_PegRuleRepeatZeroOrMore      = 20,
    P4_PegRuleRepeatZeroOrOnce      = 21,
    P4_PegRuleRepeatMin             = 22,
    P4_PegRuleRepeatMax             = 23,
    P4_PegRuleRepeatExact           = 24,
    P4_PegRuleRepeatMinMax          = 25,
    P4_PegRuleNumber                = 26,
    P4_PegRuleChar                  = 27,
    P4_PegRuleDot                   = 28,
    P4_PegRuleWhitespace            = 29,
    P4_PegRuleRangeCategory         = 30,
    P4_PegRuleComment               = 31,
} P4_PegRuleID;

/*
 *
 * ████████╗██╗░░░██╗██████╗░███████╗██████╗░███████╗███████╗░██████╗
 * ╚══██╔══╝╚██╗░██╔╝██╔══██╗██╔════╝██╔══██╗██╔════╝██╔════╝██╔════╝
 * ░░░██║░░░░╚████╔╝░██████╔╝█████╗░░██║░░██║█████╗░░█████╗░░╚█████╗░
 * ░░░██║░░░░░╚██╔╝░░██╔═══╝░██╔══╝░░██║░░██║██╔══╝░░██╔══╝░░░╚═══██╗
 * ░░░██║░░░░░░██║░░░██║░░░░░███████╗██████╔╝███████╗██║░░░░░██████╔╝
 * ░░░╚═╝░░░░░░╚═╝░░░╚═╝░░░░░╚══════╝╚═════╝░╚══════╝╚═╝░░░░░╚═════╝░
 */

/**
 * @brief The flag of expression.
 **/
typedef uint32_t        P4_ExpressionFlag;

/** An UTF8 rune */
typedef uint32_t        P4_Rune;

/** A string, equivalent to char*. */
typedef char*           P4_String;

/**
 * The identifier of a rule expression.
 *
 * The rule id must be greater than zero.
 * */
typedef uint64_t        P4_RuleID;

/**
 * The reference of user data.
 */
typedef void*   P4_UserData;

/**
 * The function to free user data.
 */
typedef void    (*P4_UserDataFreeFunc)(P4_UserData);


/*
 *
 * ░██████╗████████╗██████╗░██╗░░░██╗░█████╗░████████╗░██████╗
 * ██╔════╝╚══██╔══╝██╔══██╗██║░░░██║██╔══██╗╚══██╔══╝██╔════╝
 * ╚█████╗░░░░██║░░░██████╔╝██║░░░██║██║░░╚═╝░░░██║░░░╚█████╗░
 * ░╚═══██╗░░░██║░░░██╔══██╗██║░░░██║██║░░██╗░░░██║░░░░╚═══██╗
 * ██████╔╝░░░██║░░░██║░░██║╚██████╔╝╚█████╔╝░░░██║░░░██████╔╝
 * ╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝░╚═════╝░░╚════╝░░░░╚═╝░░░╚═════╝░
 */

/**
 * The position.
 *
 * P4_Position does not hold a pointer to the string.
 *
 * Example:
 *
 *      P4_Position pos = { .pos=10, .lineno=1, .offset=2 };
 *      printf("%u..%u\n", pos.lineno, pos.offset);
 */
typedef struct P4_Position {
    /** The position in the string. */
    size_t              pos;
    /** The line number in the string. */
    size_t              lineno;
    /** The col offset in the line. */
    size_t              offset;
} P4_Position;

/**
 * P4_RuneRange specifies a range between two runes.
 *
 * Example:
 *
 *      P4_RuneRange range = { .lower='a', .upper='z', .stride=1 };
 */
typedef struct P4_RuneRange {
    /* The lower code point of the range (inclusive). */
    P4_Rune                 lower;
    /* The upper code point of the range (inclusive). */
    P4_Rune                 upper;
    /* The step to jump inside the range. */
    size_t                  stride;
} P4_RuneRange;

/**
 * The slice of a string.
 *
 * P4_Slice does not hold a pointer to the string.
 * It only has the start and stop position of the string.
 *
 * Example:
 *
 *      P4_Slice slice = {
 *          i=0,
 *          j=strlen("hello world")
 *      };
 *      printf("%u..%u\n", slice.i, slice.j);
 **/
typedef struct P4_Slice {
    /** The start position of the slice. */
    P4_Position         start;
    /** The stop position of the slice. */
    P4_Position         stop;
}                       P4_Slice;

/**
 * The stack frame.
 *
 * This data structure is used by Peppa PEG internally.
 * You generally would not use it.
 */
typedef struct P4_Frame {
    /** The current matching expression for the frame. */
    struct P4_Expression*   expr;
    /** Whether spacing is applicable to frame & frame dependents. */
    bool                    space;
    /** Whether silencing is applicable to frame & frame dependents. */
    bool                    silent;
    /** The next frame in the stack. */
    struct P4_Frame*        next;
} P4_Frame;

/**
 * The source.
 */
typedef struct P4_Source {
    /** The grammar used to parse the source. */
    struct P4_Grammar*      grammar;
    /** The ID of entry rule in the grammar used to parse the source. */
    P4_RuleID               rule_id;

    /** The content of the source. */
    P4_String               content;
    /** The length of the source. */
    P4_Slice                slice;

    /**
     * The position of the consumed input. Min: 0, Max: strlen(content).
     *
     * It's possible the pos is less then length of content when the Source
     * is successfully parsed. It's called a partial parse.
     *
     * To avoid that, the rule will need to be wrapped with an EOI and SOI.
     * An SOI is Positive(Range(1, 0x10ffff))
     * and An EOI is Negative(Range(1, 0x10ffff)). When the rule is wrapped,
     * the input is guaranteed to be parsed until all bits are consumed.
     * */
    size_t                  pos;
    /**
     * The line number of the unconsumed input. Min: 1, Max: countlines(content).
     */
    size_t                  lineno;
    /**
     * The bytes offset of the line in the unconsumed input.
     */
    size_t                  offset;

    /** The error code of the parse. */
    P4_Error                err;
    /** The error message of the parse. */
    P4_String               errmsg;

    /** The root of abstract syntax tree. */
    struct P4_Token*        root;

    /** Reserved: whether to enable DEBUG logs. */
    bool                    verbose;

    /** The flag for checking if the parse is matching SPACED rules.
     *
     * Since we're wrapping SPACED rules into a repetition rule internally,
     * it's important to prevent matching SPACED rules in P4_MatchRepeat.
     *
     * XXX: Maybe there are some better ways to prevent that?
     */
    bool                    whitespacing;

    /** The top frame in the stack. */
    struct P4_Frame*        frame_stack;
    /** The size of frame stack. */
    size_t                  frame_stack_size;
} P4_Source;

/**
 * The grammar rule.
 */
typedef struct P4_Expression {
    /* The name of expression. */
    P4_String               name;
    /** The id of expression. */
    P4_RuleID               id;
    /** The kind of expression. */
    P4_ExpressionKind       kind;
    /** The flag of expression. */
    P4_ExpressionFlag       flag;

    union {
        /** Used by P4_Numeric. */
        size_t                      num;

        /** Used by P4_Literal and P4_BackReference. */
        struct {
            P4_String               literal;
            bool                    sensitive;
            size_t                  backref_index;
        };

        /** Used by P4_Reference..P4_Negative. */
        struct {
            P4_String               reference;
            P4_RuleID               ref_id;
            struct P4_Expression*   ref_expr;
        };

        /** Used by P4_Range. */
        struct {
            size_t                  ranges_count;
            struct P4_RuneRange*    ranges;
        };

        /** Used by P4_Sequence..P4_Choice. */
        struct {
            struct P4_Expression**  members;
            size_t                  count;
        };

        /** Used by P4_ZeroOrOnce..P4_RepeatExact.
         * repeat the expr for n times, n >= min and n <= max. */
        struct {
            struct P4_Expression*   repeat_expr; /* maybe we can merge it with ref_expr? */
            size_t                  repeat_min;
            size_t                  repeat_max;
        };
    };
} P4_Expression;

/**
 * The token object of abstract syntax tree.
 */
typedef struct P4_Token {
    /** the full text. */
    P4_String               text;
    /** The matched substring.
     * slice.start is the beginning (inclusive), and slice.stop is the end (exclusive).
     */
    P4_Slice                slice;

    /** The id for matched grammar rule. */
    P4_RuleID               rule_id;

    /** The user data. */
    P4_UserData             userdata;

    /** the sibling token. NULL if not exists. */
    struct P4_Token*        next;
    /** the first child of inner tokens. NULL if not exists. */
    struct P4_Token*        head;
    /** the last child of inner tokens. NULL if not exists. */
    struct P4_Token*        tail;
} P4_Token;

/**
 * The callback for a successful match.
 */
typedef P4_Error (*P4_MatchCallback)(struct P4_Grammar*, struct P4_Expression*, struct P4_Token*);

/**
 * The callback for a failure match.
 */
typedef P4_Error (*P4_ErrorCallback)(struct P4_Grammar*, struct P4_Expression*);

/**
 * The grammar object that holds all grammar rules.
 */
typedef struct P4_Grammar{
    /** The rules, e.g. the expressions with IDs. */
    struct P4_Expression**  rules;
    /** The total number of rules. */
    size_t                  count;
    /** The maximum number of rules. */
    int                     cap;
    /** The total number of spaced rules. */
    size_t                  spaced_count;
    /** The repetition rule for spaced rules. */
    struct P4_Expression*   spaced_rules;
    /** The recursion limit, or maximum allowed nested rules. */
    size_t                  depth;
    /** The callback after a match for an expression is successful. */
    P4_MatchCallback        on_match;
    /** The callback after a match for an expression is failed. */
    P4_ErrorCallback        on_error;
    /** The callback to free user data. */
    P4_UserDataFreeFunc     free_func;
} P4_Grammar;

typedef struct P4_EvalResult{
    P4_PegRuleID    rule;
    char            reason[64];
    union {
        P4_ExpressionFlag   flag;
        size_t              size;
        P4_Rune             rune;
        P4_String           str;
        P4_Expression**     expr;
        P4_Grammar*         grammar;
    };
} P4_EvalResult;


/*
 *
 * ███████╗██╗░░░██╗███╗░░██╗░█████╗░████████╗██╗░█████╗░███╗░░██╗░██████╗
 * ██╔════╝██║░░░██║████╗░██║██╔══██╗╚══██╔══╝██║██╔══██╗████╗░██║██╔════╝
 * █████╗░░██║░░░██║██╔██╗██║██║░░╚═╝░░░██║░░░██║██║░░██║██╔██╗██║╚█████╗░
 * ██╔══╝░░██║░░░██║██║╚████║██║░░██╗░░░██║░░░██║██║░░██║██║╚████║░╚═══██╗
 * ██║░░░░░╚██████╔╝██║░╚███║╚█████╔╝░░░██║░░░██║╚█████╔╝██║░╚███║██████╔╝
 * ╚═╝░░░░░░╚═════╝░╚═╝░░╚══╝░╚════╝░░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚══╝╚═════╝░
 */


/**
 * Provide the version string for the library.
 *
 * @return a string like "1.0.0".
 *
 * Example:
 *
 *      P4_String   version = P4_Version();
 *      printf("version=%s\n", version);
 */
P4_String      P4_Version(void);

/**
 * Read a single code point (rune) from an UTF-8 string.
 */
size_t         P4_ReadRune(P4_String s, P4_Rune* c);

/**
 * Read an escaped single code point (rune) from an UTF-8 string.
 *
 * @param       text        The text.
 * @param       rune        The rune.
 * @return      The size of rune.
 *
 * For example:
 *
 *      P4_Rune rune;
 *      P4_ReadEscapedRune("\\u000D", rune); // 0xd
 */
size_t         P4_ReadEscapedRune(char* text, P4_Rune* rune);

/**
 * Append a rune to str (in-place).
 *
 * @param       str     The string to be appended.
 * @param       chr     The rune.
 * @param       n       The size of rune. Should be 1, 2, 3, 4.
 * @return      The string starting from appended rune.
 */
void*          P4_ConcatRune(void* str, P4_Rune chr, size_t n);

/*
 * ██╗░░░░░██╗████████╗███████╗██████╗░░█████╗░██╗░░░░░
 * ██║░░░░░██║╚══██╔══╝██╔════╝██╔══██╗██╔══██╗██║░░░░░
 * ██║░░░░░██║░░░██║░░░█████╗░░██████╔╝███████║██║░░░░░
 * ██║░░░░░██║░░░██║░░░██╔══╝░░██╔══██╗██╔══██║██║░░░░░
 * ███████╗██║░░░██║░░░███████╗██║░░██║██║░░██║███████╗
 * ╚══════╝╚═╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝
 */

/**
 * Create a P4_Literal expression.
 *
 * @param   literal     The exact string to match.
 * @param   sensitive   Whether the string is case-sensitive.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match "let", "Let", "LET", etc.
 *      P4_Expression* expr = P4_CreateLiteral("let", false);
 *
 *      // It can only match "let".
 *      P4_Expression* expr = P4_CreateLiteral("let", true);
 *
 * The object holds a full copy of the literal.
 *
 *
 */
P4_Expression* P4_CreateLiteral(const P4_String, bool sensitive);

/**
 * Add a literal expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   literal     The exact string to match.
 * @param   sensitive   Whether the string is case-sensitive.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddLiteral(grammar, 1, "let", true);
 */
P4_Error       P4_AddLiteral(P4_Grammar*, P4_RuleID, const P4_String, bool sensitive);

/*
 * ██████╗░░█████╗░███╗░░██╗░██████╗░███████╗
 * ██╔══██╗██╔══██╗████╗░██║██╔════╝░██╔════╝
 * ██████╔╝███████║██╔██╗██║██║░░██╗░█████╗░░
 * ██╔══██╗██╔══██║██║╚████║██║░░╚██╗██╔══╝░░
 * ██║░░██║██║░░██║██║░╚███║╚██████╔╝███████╗
 * ╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░╚══╝░╚═════╝░╚══════╝
 */

/**
 * Create a P4_Range expression.
 *
 * @param   lower       The lower bound of UTF-8 rule to match (inclusive).
 * @param   upper       The upper bound of UTF-8 rule to match (inclusive).
 * @param   stride      The stride when iterating the characters in range.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match 0, 1, 2, 3, 4, 5, 6, 7, 8, 9.
 *      P4_Expression* expr = P4_CreateRange('0', '9', 1);
 *
 *      // It can match any code points starting from U+4E00 to U+9FCC (CJK unified ideographs block).
 *      P4_Expression* expr = P4_CreateRange(0x4E00, 0x9FFF, 1);
 *
 *
 */
P4_Expression* P4_CreateRange(P4_Rune, P4_Rune, size_t);

/**
 * Create a P4_Range expression that holds multiple ranges.
 * @param   count       The total number of ranges.
 * @param   ranges      A list of P4_RuneRange.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_RuneRange alphadigits[] = {{'a', 'Z', 1}, {'0', '9', 1}};
 *      P4_Expression* range = P4_CreateRanges(
 *          sizeof(alphadigits) / sizeof(P4_RuneRange),
 *          alphadigits
 *      );
 */
P4_Expression* P4_CreateRanges(size_t count, P4_RuneRange* ranges);

/**
 * Add a range expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   lower       The lower bound of UTF-8 rule to match (inclusive).
 * @param   upper       The upper bound of UTF-8 rule to match (inclusive).
 * @param   stride      The stride when iterating the characters in range.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddRange(grammar, 1, '0', '9', 1);
 *
 *      P4_AddRange(grammar, 1, 0x4E00, 0x9FFF, 1);
 */
P4_Error       P4_AddRange(P4_Grammar*, P4_RuleID, P4_Rune, P4_Rune, size_t);

/**
 * Add sub-ranges expression as grammar rule.
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   count       The total number of ranges.
 * @param   ranges      A list of P4_RuneRange.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_RuneRange alphadigits[] = {{'a', 'Z', 1}, {'0', '9', 1}};
 *      P4_Error err = P4_AddRanges(
 *          grammar, 1,
 *          sizeof(alphadigits) / sizeof(P4_RuneRange),
 *          alphadigits
 *      );
 */
P4_Error       P4_AddRanges(P4_Grammar*, P4_RuleID, size_t count, P4_RuneRange* ranges);

/*
 * ██████╗░███████╗███████╗███████╗██████╗░███████╗███╗░░██╗░█████╗░███████╗
 * ██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝████╗░██║██╔══██╗██╔════╝
 * ██████╔╝█████╗░░█████╗░░█████╗░░██████╔╝█████╗░░██╔██╗██║██║░░╚═╝█████╗░░
 * ██╔══██╗██╔══╝░░██╔══╝░░██╔══╝░░██╔══██╗██╔══╝░░██║╚████║██║░░██╗██╔══╝░░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║███████╗██║░╚███║╚█████╔╝███████╗
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚══════╝╚═╝░░╚══╝░╚════╝░╚══════╝
 */

/**
 * Create a P4_Reference expression.
 *
 * @param   ref_id      The ID of referenced grammar rule.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateReference(1);
 *
 *
 */
P4_Expression* P4_CreateReference(P4_RuleID);

/**
 * Add a reference expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   ref_id      The ID of referenced grammar rule.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddReference(grammar, 1, 2);
 */
P4_Error       P4_AddReference(P4_Grammar*, P4_RuleID, P4_RuleID);

/*
 * ██████╗░░█████╗░░██████╗██╗████████╗██╗██╗░░░██╗███████╗
 * ██╔══██╗██╔══██╗██╔════╝██║╚══██╔══╝██║██║░░░██║██╔════╝
 * ██████╔╝██║░░██║╚█████╗░██║░░░██║░░░██║╚██╗░██╔╝█████╗░░
 * ██╔═══╝░██║░░██║░╚═══██╗██║░░░██║░░░██║░╚████╔╝░██╔══╝░░
 * ██║░░░░░╚█████╔╝██████╔╝██║░░░██║░░░██║░░╚██╔╝░░███████╗
 * ╚═╝░░░░░░╚════╝░╚═════╝░╚═╝░░░╚═╝░░░╚═╝░░░╚═╝░░░╚══════╝
 */

/**
 * Create a P4_Positive expression.
 *
 * @param   refexpr     The positive pattern to check.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // If the following text includes "let", the match is successful.
 *      P4_Expression* expr = P4_CreatePositive(P4_CreateLiteral("let", true));
 *
 *
 */
P4_Expression* P4_CreatePositive(P4_Expression*);

/**
 * Add a positive expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   refexpr     The positive pattern to check.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddPositive(grammar, 1, P4_CreateLiteral("let", true));
 */
P4_Error       P4_AddPositive(P4_Grammar*, P4_RuleID, P4_Expression*);

/*
 * ███╗░░██╗███████╗░██████╗░░█████╗░████████╗██╗██╗░░░██╗███████╗
 * ████╗░██║██╔════╝██╔════╝░██╔══██╗╚══██╔══╝██║██║░░░██║██╔════╝
 * ██╔██╗██║█████╗░░██║░░██╗░███████║░░░██║░░░██║╚██╗░██╔╝█████╗░░
 * ██║╚████║██╔══╝░░██║░░╚██╗██╔══██║░░░██║░░░██║░╚████╔╝░██╔══╝░░
 * ██║░╚███║███████╗╚██████╔╝██║░░██║░░░██║░░░██║░░╚██╔╝░░███████╗
 * ╚═╝░░╚══╝╚══════╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░░╚═╝░░░╚══════╝
 */

/**
 * Create a P4_Negative expression.
 *
 * @param   refexpr     The negative pattern to check.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // If the following text does not start with "let", the match is successful.
 *      P4_Expression* expr = P4_CreateNegative(P4_CreateLiteral("let", true));
 *
 */
P4_Expression* P4_CreateNegative(P4_Expression*);

/**
 * Add a negative expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   refexpr     The negative pattern to check.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddNegative(grammar, 1, P4_CreateLiteral("let", true));
 */
P4_Error       P4_AddNegative(P4_Grammar*, P4_RuleID, P4_Expression*);

/*
 * ░██████╗███████╗░██████╗░██╗░░░██╗███████╗███╗░░██╗░█████╗░███████╗
 * ██╔════╝██╔════╝██╔═══██╗██║░░░██║██╔════╝████╗░██║██╔══██╗██╔════╝
 * ╚█████╗░█████╗░░██║██╗██║██║░░░██║█████╗░░██╔██╗██║██║░░╚═╝█████╗░░
 * ░╚═══██╗██╔══╝░░╚██████╔╝██║░░░██║██╔══╝░░██║╚████║██║░░██╗██╔══╝░░
 * ██████╔╝███████╗░╚═██╔═╝░╚██████╔╝███████╗██║░╚███║╚█████╔╝███████╗
 * ╚═════╝░╚══════╝░░░╚═╝░░░░╚═════╝░╚══════╝╚═╝░░╚══╝░╚════╝░╚══════╝
 */

/**
 * Create a P4_Sequence expression.
 *
 * @param   count       The number of sequence members.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateSequence(3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 *
 *
 */
P4_Expression* P4_CreateSequence(size_t);

/**
 * Create a P4_Sequence expression.
 *
 * @param   count       The number of sequence members.
 * @param   ...         The vararg of every sequence member.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match { BODY }.
 *      P4_Expression* expr = P4_CreateSequenceWithMembers(3,
 *          P4_CreateLiteral("{"),
 *          P4_CreateReference(BODY),
 *          P4_CreateLiteral("}")
 *      );
 */
P4_Expression* P4_CreateSequenceWithMembers(size_t, ...);

/**
 * Add a sequence expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   count       The number of sequence members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddSequence(grammar, 1, 3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 */
P4_Error       P4_AddSequence(P4_Grammar*, P4_RuleID, size_t);

/**
 * Add a sequence expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   count       The number of sequence members.
 * @param   ...         The members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddSequenceWithMembers(grammar, 1, 3,
 *          P4_CreateLiteral("{"),
 *          P4_CreateReference(BODY),
 *          P4_CreateLiteral("}")
 *      );
 */
P4_Error       P4_AddSequenceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);

/*
 * ░█████╗░██╗░░██╗░█████╗░██╗░█████╗░███████╗
 * ██╔══██╗██║░░██║██╔══██╗██║██╔══██╗██╔════╝
 * ██║░░╚═╝███████║██║░░██║██║██║░░╚═╝█████╗░░
 * ██║░░██╗██╔══██║██║░░██║██║██║░░██╗██╔══╝░░
 * ╚█████╔╝██║░░██║╚█████╔╝██║╚█████╔╝███████╗
 * ░╚════╝░╚═╝░░╚═╝░╚════╝░╚═╝░╚════╝░╚══════╝
 */

/**
 * Create a P4_Choice expression.
 *
 * @param   count       The number of choice members.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateChoice(3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 *
 *
 */
P4_Expression* P4_CreateChoice(size_t);

/**
 * Create a P4_Choice expression.
 *
 * @param   count       The number of choice members.
 * @param   ...         The vararg of every choice member.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match whitespace, tab and newline.
 *      P4_Expression* expr = P4_CreateChoiceWithMembers(3,
 *          P4_CreateLiteral(" "),
 *          P4_CreateReference(\t),
 *          P4_CreateLiteral("\n")
 *      );
 */
P4_Expression* P4_CreateChoiceWithMembers(size_t, ...);

/**
 * Add a choice expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   count       The number of choice members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddChoice(grammar, 1, 3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 */
P4_Error       P4_AddChoice(P4_Grammar*, P4_RuleID, size_t);

/**
 * Add a choice expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   count       The number of choice members.
 * @param   ...         The members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddChoiceWithMembers(grammar, 1, 3,
 *          P4_CreateLiteral(" "),
 *          P4_CreateReference(\t),
 *          P4_CreateLiteral("\n")
 *      );
 */
P4_Error       P4_AddChoiceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░ █▀ █▀▄▀█ █ █▄░█ ░ █▀▄▀█ ▄▀█ ▀▄▀ ▀█
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░ █▄ █░▀░█ █ █░▀█ █ █░▀░█ █▀█ █░█ ▄█
 */

/**
 * Create a P4_Repeat expression minimal min times and maximal max times.
 *
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @param   max         The maximum repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "a", "aa", or "aaa".
 *      P4_Expression* expr = P4_CreateRepeatMinMax(
 *          P4_CreateLiteral("a", true),
 *          1, 3
 *      );
 */
P4_Expression* P4_CreateRepeatMinMax(P4_Expression*, size_t, size_t);

/**
 * Create a P4_Repeat expression minimal min times and maximal max times.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @param   max         The maximum repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", or "aaa".
 *      P4_AddRepeatMinMax(
 *          P4_CreateLiteral("a", true),
 *          1, 3
 *      );
 */
P4_Error       P4_AddRepeatMinMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t, size_t);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░ █▀ █▀▄▀█ █ █▄░█ ░ ▀█
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░ █▄ █░▀░█ █ █░▀█ █ ▄█
 */

/**
 * Create a P4_Repeat expression minimal min times and maximal SIZE_MAX times.
 *
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_Expression* expr = P4_CreateRepeatMin(P4_CreateLiteral("a", true), 1);
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, min, SIZE_MAX);
 *
 */
P4_Expression* P4_CreateRepeatMin(P4_Expression*, size_t);

/**
 * Create a RepeatMin expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_AddRepeatMin(grammar, 1, P4_CreateLiteral("a", true), 1);
 */
P4_Error       P4_AddRepeatMin(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░ █▀ ░ █▀▄▀█ ▄▀█ ▀▄▀ ▀█
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░ █▄ █ █░▀░█ █▀█ █░█ ▄█
 */

/**
 * Create a P4_Repeat expression maximal max times.
 *
 * @param   repeat_expr The repeated expression.
 * @param   max         The maximum repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "", "a", "aa", "aaa".
 *      P4_Expression* expr = P4_CreateRepeatMax(P4_CreateLiteral("a", true), 3);
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 0, max);
 *
 */
P4_Expression* P4_CreateRepeatMax(P4_Expression*, size_t);

/**
 * Add a RepeatMax expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @param   max         The maximum repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "", "a", "aa", "aaa".
 *      P4_AddRepeatMax(grammar, 1, P4_CreateLiteral("a", true), 3);
 */
P4_Error       P4_AddRepeatMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░ █▀ █▄░█ ▀█
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░ █▄ █░▀█ ▄█
 */

/**
 * Create a P4_Repeat expression exact N times.
 *
 * @param   repeat_expr The repeated expression.
 * @param   N           The repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "aaa".
 *      P4_Expression* expr = P4_CreateRepeatExact(P4_CreateLiteral("a", true), 3);
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, N, N);
 *
 *
 */
P4_Expression* P4_CreateRepeatExact(P4_Expression*, size_t);

/**
 * Add a RepeatExact expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @param   N           The repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "aaa".
 *      P4_AddRepeatExact(P4_CreateLiteral("a", true), 3);
 */
P4_Error       P4_AddRepeatExact(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗░█████╗░
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝██╔══██╗
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░╚═╝███╔╝
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░░░░╚══╝░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░░░░██╗░░
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░░░░╚═╝░░
 */

/**
 * Create a P4_Repeat expression zero or once.
 *
 * @param   repeat_expr The repeated expression.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_Expression* expr = P4_CreateZeroOrOnce(P4_CreateLiteral("a", true));
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 0, 1);
 *
 *
 */
P4_Expression* P4_CreateZeroOrOnce(P4_Expression*);

/**
 * Add a ZeroOrOnce expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_AddZeroOrOnce(grammar, 1, P4_CreateLiteral("a", true));
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 0, 1);
 */
P4_Error       P4_AddZeroOrOnce(P4_Grammar*, P4_RuleID, P4_Expression*);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗██╗░░██╗
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝╚██╗██╔╝
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░█████████
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░░██╔██╗░
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░██╔╝╚██╗
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝
 */

/**
 * Create a P4_Repeat expression zero or more times.
 *
 * @param   repeat_expr The repeated expression.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_Expression* expr = P4_CreateZeroOrMore(P4_CreateLiteral("a", true));
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 0, SIZE_MAX);
 *
 *
 */
P4_Expression* P4_CreateZeroOrMore(P4_Expression*);

/**
 * Add a ZeroOrMore expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_AddZeroOrMore(grammar, 1, P4_CreateLiteral("a", true));
 */
P4_Error       P4_AddZeroOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);

/*
 * ██████╗░███████╗██████╗░███████╗░█████╗░████████╗░░░░░░░
 * ██╔══██╗██╔════╝██╔══██╗██╔════╝██╔══██╗╚══██╔══╝░░██╗░░
 * ██████╔╝█████╗░░██████╔╝█████╗░░███████║░░░██║░░░██████╗
 * ██╔══██╗██╔══╝░░██╔═══╝░██╔══╝░░██╔══██║░░░██║░░░╚═██╔═╝
 * ██║░░██║███████╗██║░░░░░███████╗██║░░██║░░░██║░░░░░╚═╝░░
 * ╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░░░░░░░░
 */

/**
 * Create a P4_Repeat expression once or more times.
 *
 * @param   repeat_expr The repeated expression.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_Expression* expr = P4_CreateOnceOrMore(P4_CreateLiteral("a", true));
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 1, SIZE_MAX);
 *
 *
 */
P4_Expression* P4_CreateOnceOrMore(P4_Expression*);

/**
 * Add an OnceOrMore expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_AddOnceOrMore(grammar, 1, P4_CreateLiteral("a", true));
 */
P4_Error       P4_AddOnceOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);

/*
 * ██████╗░░█████╗░░█████╗░██╗░░██╗██████╗░███████╗███████╗███████╗██████╗░███████╗███╗░░██╗░█████╗░███████╗
 * ██╔══██╗██╔══██╗██╔══██╗██║░██╔╝██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝████╗░██║██╔══██╗██╔════╝
 * ██████╦╝███████║██║░░╚═╝█████═╝░██████╔╝█████╗░░█████╗░░█████╗░░██████╔╝█████╗░░██╔██╗██║██║░░╚═╝█████╗░░
 * ██╔══██╗██╔══██║██║░░██╗██╔═██╗░██╔══██╗██╔══╝░░██╔══╝░░██╔══╝░░██╔══██╗██╔══╝░░██║╚████║██║░░██╗██╔══╝░░
 * ██████╦╝██║░░██║╚█████╔╝██║░╚██╗██║░░██║███████╗██║░░░░░███████╗██║░░██║███████╗██║░╚███║╚█████╔╝███████╗
 * ╚═════╝░╚═╝░░╚═╝░╚════╝░╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚══════╝╚═╝░░╚══╝░╚════╝░╚══════╝
 */

/**
 * Create a P4_BackReference expression.
 *
 * @param   backref_index   The index of backref member in the sequence.
 * @param   sensitive       Whether the backref matching is case sensitive.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "EOF MULTILINE EOF" or "eof MULTILINE eof", but not "EOF MULTILINE eof".
 *      P4_Expression* expr = P4_CreateSequenceWithMembers(4,
 *          P4_CreateLiteral("EOF", false),
 *          P4_CreateReference(MULTILINE),
 *          P4_CreateBackReference(0, true)
 *      );
 */
P4_Expression* P4_CreateBackReference(size_t, bool);

/**
 * Set the member of Sequence/Choice at a given index.
 *
 * @param   expr        The sequence/choice expression.
 * @param   index       The index of member.
 * @param   member      The member expression.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_Error       err;
 *      P4_Expression* expr = P4_CreateSequence(2);
 *      if ((err = P4_SetMember(expr, 0, P4_CreateLiteral("a", true))) != P4_Ok) {
 *          // handle error.
 *      }
 *      if ((err = P4_SetMember(expr, 1, P4_CreateLiteral("b", true))) != P4_Ok) {
 *          // handle error.
 *      }
 */
P4_Error       P4_SetMember(P4_Expression*, size_t, P4_Expression*);

/**
 * Set the referenced member of Sequence/Choice at a given index.
 *
 * @param   expr                The sequence/choice expression.
 * @param   index               The index of member.
 * @param   member_rule_id      The rule id of member.
 * @return  The error code.
 *
 * It's equivalent to:
 *
 *      P4_SetMember(expr, index, P4_CreateReference(member_rule_id));
 */
P4_Error       P4_SetReferenceMember(P4_Expression*, size_t, P4_RuleID);

/**
 * Get the total number members for sequence/choice.
 *
 * @param   expr        The sequence/choice expression.
 * @return  The number. If something goes wrong, it returns 0.
 *
 *      P4_Expression* expr = P4_CreateSequence(3);
 *      size_t  count = P4_GetMembers(expr); // 3
 */
size_t         P4_GetMembersCount(P4_Expression*);

/**
 * Get the member of sequence/choice at a given index.
 *
 * @param   expr        The sequence/choice expression.
 * @param   index       The index of a member.
 * @return  The P4_Expression object of a member.
 *
 * Example:
 *
 *      P4_Expression* member = P4_GetMember(expr, 0);
 */
P4_Expression* P4_GetMember(P4_Expression*, size_t);

/**
 * Create an Start-Of-Input expression.
 *
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateStartOfInput();
 *
 * Start-Of-Input can be used to insert whitespace before the actual content.
 *
 * Example:
 *
 *      P4_AddSequenceWithMembers(grammar, 1, 2,
 *          P4_CreateStartOfInput(),
 *          P4_CreateLiteral("HELLO", true)
 *      );
 *
 * Start-Of-Input is equivalent to ``P4_CreatePositive(P4_CreateRange(1, 0x10ffff, 1))`.
 */
P4_Expression* P4_CreateStartOfInput();

/**
 * Create an End-Of-Input expression.
 *
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateEndOfInput();
 *
 * End-Of-Input can be used to insert whitespace after the actual content.
 *
 * Example:
 *
 *      P4_AddSequenceWithMembers(grammar, 1, 2,
 *          P4_CreateLiteral("HELLO", true),
 *          P4_CreateEndOfInput()
 *      );
 *
 * Start-Of-Input is equivalent to ``P4_CreateNegative(P4_CreateRange(1, 0x10ffff, 1))`.
 */
P4_Expression* P4_CreateEndOfInput();

/**
 * Add a Join expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   joiner  A joiner literal.
 * @param   pattern The repeated pattern rule id.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_Expression* row = P4_AddJoin(grammar, 1, ",", Element);
 */
P4_Error P4_AddJoin(P4_Grammar* grammar, P4_RuleID id, const P4_String joiner, P4_RuleID rule_id);

/**
 * Create a Join expression.
 *
 * @param   joiner  A joiner literal.
 * @param   pattern The repeated pattern rule id.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* row = P4_CreateJoin(",", Element);
 *
 * Join is a syntax sugar to the below structure:
 *
 *      Sequence(pattern, ZeroOrMore(Sequence(Literal(joiner), pattern)))
 */
P4_Expression* P4_CreateJoin(const P4_String joiner, P4_RuleID rule_id);


/**
 * Free an expression.
 *
 * @param   expr        The expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateLiteral("a", true);
 *      P4_DeleteExpression(expr);
 *
 * The members of a sequence/choice expression will be deleted.
 * The ref_expr of a positive/negative expression will be deleted.
 *
 * P4_Reference only hold a reference so the referenced expression won't be freed.
 */
void           P4_DeleteExpression(P4_Expression*);

/**
 * Set the rule id for the expression.
 *
 * @param   expr        The expression.
 * @param   rule_id     The rule id to set.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateLiteral("a", true);
 *      P4_SetRuleID(expr, 1);
 *
 * Note that `P4_SetRuleID` only changes the `expr->id` field.
 * It does not add expr to the P4_Grammar object.
 */
P4_Error       P4_SetRuleID(P4_Expression*, P4_RuleID);

/**
 * Check if an expression is a grammar rule.
 *
 * @param   expr        The expression.
 * @return  if an expression is a grammar rule.
 *
 *      P4_AddLiteral(grammar, 1, "a", true);
 *      P4_IsRule(P4_GetGrammarRule(grammar, 1)); // true
 */
bool           P4_IsRule(P4_Expression*);

/**
 * Check if an expression has P4_FLAG_SQUASHED flag.
 */
bool           P4_IsSquashed(P4_Expression*);

/**
 * Check if an expression has P4_FLAG_LIFTED flag.
 */
bool           P4_IsLifted(P4_Expression*);

/**
 * Check if an expression has P4_FLAG_TIGHT flag.
 */
bool           P4_IsTight(P4_Expression*);

/**
 * Check if an expression has P4_FLAG_SCOPED flag.
 */
bool           P4_IsScoped(P4_Expression*);

/**
 * Check if an expression has P4_FLAG_SPACED flag.
 */
bool           P4_IsSpaced(P4_Expression*);

/**
 * Set the flag for an expression.
 *
 * Example:
 *
 *      P4_SetExpressionFlag(expr, P4_FLAG_SQUASHED);
 *      P4_SetExpressionFlag(expr, P4_FLAG_TIGHT | P4_FLAG_SQUASHED);
 */
void           P4_SetExpressionFlag(P4_Expression*, P4_ExpressionFlag);

/**
 * @brief       Create a \ref P4_Grammar object.
 * @return      A \ref P4_Grammar object.
 *
 * Example:
 *
 *      P4_Grammar*     grammar = P4_CreateGrammar();
 */
P4_Grammar*    P4_CreateGrammar(void);

/**
 * @brief       Delete the grammar object.
 * @param       grammar     The grammar.
 *
 * It will also free all of the expression rules.
 */
void           P4_DeleteGrammar(P4_Grammar*);

/**
 * Add a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   expr        The grammar rule expression.
 * @return  The error code.
 */
P4_Error       P4_AddGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);

/**
 * Set the name for a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @param   name        The grammar rule name.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_SetRuleName(grammar, ENTRY, "entry");
 */
P4_Error       P4_SetGrammarRuleName(P4_Grammar* grammar, P4_RuleID id, P4_String name);

/**
 * Get the name for a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @return  The grammar rule name.
 *
 * Example:
 *
 *      P4_String name = P4_GetRuleName(grammar, ENTRY);
 *      printf("%s\n", name);
 */
P4_String      P4_GetGrammarRuleName(P4_Grammar* grammar, P4_RuleID id);

/**
 * Delete a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 *
 * WARNING: NOT IMPLEMENTED.
 */
void           P4_DeleteGrammarRule(P4_Grammar*, P4_RuleID);

/**
 * Get a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   id          The grammar rule id.
 * @return  The grammar rule expression. Returns NULL if not found.
 *
 * Example:
 *
 *      P4_AddLiteral(grammar, 1, "a", true);
 *      P4_Expression* expr = P4_GetGrammarRule(grammar, 1); // The literal expression.
 */
P4_Expression* P4_GetGrammarRule(P4_Grammar*, P4_RuleID);

/**
 * Get a grammar rule by its name.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @return  The grammar rule expression. Returns NULL if not found.
 *
 *      P4_AddLiteral(grammar, 1, "a", true);
 *      P4_SetGrammarRuleName(grammar, 1, "rule_a");
 *      P4_Expression* expr = P4_GetGrammarRule(grammar, "rule_a"); // The literal expression.
 */
P4_Expression* P4_GetGrammarRuleByName(P4_Grammar* grammar, P4_String name);

/**
 * @brief       Set the flag of a grammar rule.
 * @param       grammar     The grammar.
 * @param       id          The ID.
 * @param       flag        The bits of \ref P4_ExpressionFlag.
 * @return      The error code. If successful, return \ref P4_Ok.
 *
 * Example:
 *
 *      P4_Error err = P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_SQUASHED | P4_FLAG_LIFTED | P4_FLAG_TIGHT);
 *      if (err != P4_Ok) {
 *          printf("err=%u\n", err);
 *          exit(1);
 *      }
 */
P4_Error       P4_SetGrammarRuleFlag(P4_Grammar*, P4_RuleID, P4_ExpressionFlag);

/**
 * @brief       Set the maximum allowed recursion calls.
 * @param       grammar     The grammar.
 * @param       limit       The number of maximum recursion calls.
 * @return      An error. If successful, return \ref P4_Ok.
 *
 * Example:
 *
 *      // on a machine with small memory.
 *      P4_SetRecursionLimit(grammar, 256);
 *
 *      // on a machine with large memory.
 *      P4_SetRecursionLimit(grammar, 1024*20);
 */
P4_Error       P4_SetRecursionLimit(P4_Grammar*, size_t limit);

/**
 * @brief       Set free function for the user data.
 * @param       grammar     The grammar.
 * @param       free_func   The free function.
 * @return      The error code.
 */
P4_Error       P4_SetUserDataFreeFunc(P4_Grammar* grammar, P4_UserDataFreeFunc free_func);

/**
 * @brief       Get the maximum allowed recursion calls.
 * @param       grammar     The grammar.
 * @return      The maximum allowed recursion calls. If something goes wrong, return 0.
 *
 * Example:
 *
 *      size_t  limit = P4_GetRecursionLimit(grammar);
 *      printf("limit=%u\n", limit);
 */
size_t         P4_GetRecursionLimit(P4_Grammar*);

/**
 * @brief       Create a \ref P4_Source* object.
 * @param       content     The content of input.
 * @param       rule_id     The id of entry grammar rule.
 * @return      A source object.
 *
 * Example:
 *
 *      char*       content = ... // read from some files.
 *      P4_RuleID   rule_id = My_EntryRule;
 *
 *      P4_Source*  source  = P4_CreateSource(content, rule_id);
 *
 *      // do something...
 *
 *      P4_DeleteSource(source);
 */
P4_Source*     P4_CreateSource(P4_String, P4_RuleID);

/**
 * @brief       Free the allocated memory of a source.
 *              If the source has been parsed and has an AST, the entire AST will also be free-ed.
 * @param       source  The source.
 *
 * Example:
 *
 *      P4_DeleteSource(source);
 */
void           P4_DeleteSource(P4_Source* source);

/**
 * @brief       Reset the source so it can be called with P4_Parse again.
 *              All of its internal states are cleared.
 * @param       source  The source.
 *
 * Example:
 *
 *      // parse full text.
 *      P4_Parse(grammar, source);
 *
 *      P4_ResetSource(source);
 *      P4_SetSourceSlice(source, 1, 10);
 *
 *      // parse text[1:10].
 *      P4_Parse(grammar, source);
 */
void           P4_ResetSource(P4_Source* source);

/**
 * @brief       Set the buf size of the source content.
 *              If not set, strlen(source->content) is used.
 * @param       source  The source.
 * @param       start   The start position, inclusive.
 * @param       stop    The stop position, exclusive.
 * @return      The error code.
 *
 * Example:
 *
 *      P4_String  input = "(a)"
 *      P4_Source* source = P4_CreateSource(input, RuleA);
 *      if (P4_Ok != P4_SetSourceSlice(source, 1, 2)) // only parse "a"
 *          printf("set buf size error\n");
 */
P4_Error       P4_SetSourceSlice(P4_Source* source, size_t start, size_t stop);

/**
 * @brief       Get the root token of abstract syntax tree of the source.
 * @param       source  The source.
 * @return      The root token. If the parse is failed or the root token is lifted, return NULL.
 */
P4_Token*      P4_GetSourceAst(P4_Source*);


/**
 * @brief       Transfer the ownership of source ast to the caller.
 * @param       source  The source.
 * @return      The root token. If the parse is failed or the root token is lifted, return NULL.
 *
 * You're on your own now to manage the de-allocation of the source ast.
 *
 * The source is implicitly reset after the source ast is acquired by the caller.
 *
 * Example:
 *
 *      P4_Token* root = P4_AcquireSourceAst(source);
 *      // ...
 *      P4_DeleteToken(root);
 */
P4_Token*      P4_AcquireSourceAst(P4_Source* source);

/**
 * @brief       Get the last position in the input after a parse.
 * @param       source  The source.
 * @return      The position in the input.
 */
size_t          P4_GetSourcePosition(P4_Source*);

/**
 * @brief       Print the token tree.
 * @param       grammar     The grammar.
 * @param       stream      The output stream.
 * @param       token       The root node of source ast.
 *
 * Example:
 *
 *      P4_Token* root = P4_GetSourceAst(source);
 *      P4_JsonifySourceAst(grammar, stdout, root);
 */
void           P4_JsonifySourceAst(P4_Grammar* grammar, FILE* stream, P4_Token* token);

/**
 * @brief       Inspect the token tree.
 * @param       token       The root node of source ast.
 * @param       userdata    Any additional information you want to pass in.
 * @param       inspector   The inspecting function. It should return P4_Ok for a successful inspection.
 *
 * Example:
 *
 *      void MyInspector(P4_Token* token, void* userdata) {
 *          printf("%lu\t%lu\t%p\n", token->slice.start.pos, token->rule_id, userdata);
 *          return P4_Ok;
 *      }
 *
 *      P4_Error err = P4_InspectSourceAst(root, NULL, MyInspector);
 */
P4_Error       P4_InspectSourceAst(P4_Token* token, void* userdata, P4_Error (*inspector)(P4_Token*, void*));

/**
 * @brief       Parse the source given a grammar.
 * @param       grammar     The grammar.
 * @param       source      The source.
 * @return      The error code. If successful, return \ref P4_Ok.
 *
 * Example:
 *
 *      if ((err = P4_Parse(grammar, source)) != P4_Ok) {
 *          // error handling ...
 *      }
 */
P4_Error       P4_Parse(P4_Grammar*, P4_Source*);

/**
 * @brief       Determine whether the parse is failed.
 * @param       source      The source.
 * @return      Whether the parse is failed.
 *
 * Example:
 *
 *      if (P4_HasError(source)) {
 *          printf("err=%u\n", P4_GetError(source));
 *          printf("msg=%s\n", P4_GetErrorMessage(source));
 *      }
 */
bool           P4_HasError(P4_Source*);

/**
 * @brief       Get the error code if failed to parse the source.
 * @param       source      The source.
 * @return      The error code.
 *
 * Example:
 *
 *      if (P4_Ok != P4_Parse(grammar, source)) {
 *          printf("err=%u\n", P4_GetError(source));
 *      }
 */
P4_Error       P4_GetError(P4_Source*);

/**
 * @brief       Get the error string given an error code.
 * @param       err     The error code.
 * @return      The error string.
 *
 * Example:
 *
 *      P4_String errstr = P4_GetErrorString(P4_MatchError);
 *      printf("%s\n", errstr); // "MatchError"
 */
P4_String      P4_GetErrorString(P4_Error err);

/**
 * @brief       Get the error message if failed to parse the source.
 * @param       source      The source.
 * @return      The error message.
 *
 * Example:
 *
 *      if (P4_Ok != P4_Parse(grammar, source)) {
 *          printf("msg=%s\n", P4_GetErrorMessage(source));
 *      }
 *
 * The returned value is a reference to the internal string. Don't free it after use.
 */
P4_String      P4_GetErrorMessage(P4_Source*);

/**
 * @brief       Create a token.
 * @param       str     The text.
 * @param       start   The starting position of the text.
 * @param       stop    The stopping position of the text.
 * @param       expr    The expression that matches to the slice of the text.
 * @return      The token.
 *
 * Example:
 *
 *      P4_String       str     = "Hello world";
 *      size_t          start   = 0;
 *      size_t          stop    = 11;
 *      P4_Expression*  expr    = P4_GetGrammarRule(grammar, ENTRY);
 *
 *      P4_Token* token = P4_CreateToken(text, start, stop, expr);
 *
 *      // do something.
 *
 *      P4_DeleteToken(token);
 */
P4_Token*      P4_CreateToken(P4_String, P4_Position*, P4_Position*, P4_RuleID);

/**
 * @brief       Delete the token.
 *              This will free the occupied memory for token.
 *              The str of the token won't be free-ed since the token only owns not the string but the slice of a string.
 * @param       token   The token.
 *
 * Example:
 *
 *      P4_DeleteToken(token);
 */
void           P4_DeleteToken(P4_Token*);

/**
 * @brief       Get the slice that the token covers.
 *              The slice is owned by the token so don't free it.
 * @param       token   The token.
 * @return      The slice.
 *
 * Example:
 *
 *      P4_Slice* slice = P4_GetTokenSlice(token);
 *      printf("token slice=[%u..%u]\n", slice->i, slice->j);
 */
P4_Slice*      P4_GetTokenSlice(P4_Token*);


/**
 * @brief       Get the total number of token children.
 * @param       token   The token.
 * @return      The total number of token children.
 *
 * Example:
 *
 *      P4_Token* root = P4_GetSourceAst(source);
 *      size_t count = P4_GetTokenChildrenCount(root);
 *      printf("There are %lu children for root node\n", count);
 */
size_t         P4_GetTokenChildrenCount(P4_Token* token);

/**
 * @brief       Copy the string that the token covers.
 *              The caller is responsible for freeing the string.
 *
 * @param       token   The token.
 * @return      The string.
 *
 * Example:
 *
 *      P4_String* str = P4_CopyTokenString(token);
 *      printf("token str=%s\n", str);
 *      free(str);
 */
P4_String      P4_CopyTokenString(P4_Token*);


/**
 * @brief       Set callback function.
 * @param       grammar     The grammar.
 * @param       matchcb     The callback on a successful match.
 * @param       errcb       The callback on a failure match.
 * @return      The error code.
 */
P4_Error       P4_SetGrammarCallback(P4_Grammar*, P4_MatchCallback, P4_ErrorCallback);

/**
 * @brief       Replace an existing grammar rule.
 * @param       grammar     The grammar.
 * @param       id          The rule id.
 * @param       expr        The rule expression to replace.
 * @return      The error code.
 *
 * The original grammar rule will be deleted.
 */
P4_Error       P4_ReplaceGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);

/**
 * @brief       Create a grammar that can parse other grammars written in PEG syntax.
 * @return      The grammar object.
 *
 * Example:
 *
 *      P4_Grammar* peg = P4_CreatePegGrammar();
 *      P4_DeleteGrammar(peg);
 */
P4_Grammar*    P4_CreatePegGrammar ();

/**
 * @brief       Evaluate the token ast parsed by P4_CreatePegGrammar.
 * @param       token   The token tree.
 * @param       result  Can be a reference several value kinds, such as size_t, P4_Grammar, P4_Expression, etc.
 * @return      The error code.
 *
 * Example:
 *
 *      P4_Grammar* peg     = P4_CreatePegGrammar();
 *      P4_Source*  peggram = P4_CreateSource("my_grammar = \"abc\";", P4_PegGrammar);
 *      P4_Parse(peg, peggram);
 *
 *      P4_Grammar* my_grammar = NULL;
 *
 *      if (P4_Ok != P4_PegEval(P4_GetSourceAst(peggram), &my_grammar))
 *          printf("invalid token tree.\n");
 *
 *      P4_DeleteGrammar(my_grammar);
 *      P4_DeleteSource(peggram);
 *      P4_DeleteGrammar(peg);
 */
P4_Error       P4_PegEval(P4_Token* token, void* result);

/**
 * @brief       Get the corresponding rule name for a peg grammar rule id.
 * @param       id      A P4_PegRuleID.
 * @return      The name.
 *
 *      printf("%s\n", P4_StringifyPegGrammarRuleID(P4_PegRuleRule));
 */
P4_String      P4_StringifyPegGrammarRuleID(P4_RuleID id);


/**
 * @brief       Load peg grammar from a string.
 * @param       rules   The rules string.
 * @return      The grammar object.
 *
 * Example:
 *
 *      P4_Grammar* grammar = P4_LoadGrammar(
 *          "entry = one one;\n"
 *          "one   = \"1\";\n"
 *      );
 *      P4_Source* source = P4_CreateSource("11", 1);
 *      P4_Parse(grammar, source);
 */
P4_Grammar*    P4_LoadGrammar(P4_String rules);


#ifdef __cplusplus
}
#endif

# endif
