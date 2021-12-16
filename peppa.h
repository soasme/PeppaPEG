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
 * @file       peppa.h
 * @brief      Peppa PEG header file
 * @author     Ju Lin
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

# ifdef ENABLE_UNISTR
#include <unictype.h>
# else

/**
 * A single unicode character.
 * Used when libunistring is not installed.
 * */
typedef uint32_t        ucs4_t;
# endif

# ifndef P4_MALLOC
/**
 * The malloc function. By default, it's `malloc`.
 */
# define P4_MALLOC malloc
# endif

# ifndef P4_FREE
/**
 * The free function. By default, it's `free`.
 */
# define P4_FREE free
# endif

# ifndef P4_REALLOC
/**
 * The realloc function. By default, it's `realloc`.
 */
# define P4_REALLOC realloc
# endif

# ifndef P4_CALLOC
/**
 * The calloc function. By default, it's `calloc`.
 */
# define P4_CALLOC calloc
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
# define P4_MINOR_VERSION 16

/** Patch version number. */
# define P4_PATCH_VERSION 0

/** No flag. */
# define P4_FLAG_NONE                   ((uint32_t)(0x0))

/**
 * When the flag is set, the grammar rule will have squash all
 * children nodes.
 *
 * `node->head`, `node->tail` will be NULL.
 *
 * For example, rule b has P4_FLAG_SQUASHED. After parsing,
 * the children nodes d and e are gone:
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
 * node with its children nodes.
 *
 * For example, rule b has P4_FLAG_SQUASHED. After parsing,
 * the node (b) is gone, and its children d and e become
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
 * creating nodes and apply SPACED rules for sequences and repetitions.
 */
# define P4_FLAG_SCOPED                 ((uint32_t)(0x1000))

/**
 * When the flag is set, the expression will be inserted
 * between every node inside the sequences and repetitions
 *
 * If there are multiple SPACED expressions, Peppa PEG will
 * iterate through all SPACED expressions.
 *
 * This flag makes the grammar clean and tidy without inserting
 * whitespace rule everywhere.
 */
# define P4_FLAG_SPACED                 ((uint32_t)(0x10000))

/**
 * When the flag is set and a sequence/repetition node tree
 * has only one child, the child node will replace the parent.
 */
# define P4_FLAG_NON_TERMINAL           ((uint32_t)(0x100000))

/**
 * The default recursion limit.
 *
 * It can be adjusted via function P4_SetRecursionLimit.
 */
# define P4_DEFAULT_RECURSION_LIMIT     8192

/**
 * The maximum length of a rule name.
 */
# define P4_MAX_RULE_NAME_LEN           32

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
    /** Rule: Unicode Category. */
    P4_UnicodeCategory,
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
    /** Rule: Cut. */
    P4_Cut,
    /** Rule: Left Recursion. */
    P4_LeftRecursion
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
    /** When the given value is not valid peg grammar. */
    P4_PegError             = 11,
    /** When the failure occurs after a `@cut` operator. */
    P4_CutError             = 12
} P4_Error;

# define E_WRONG_LIT           10000
# define E_BACKREF_OUT_REACHED 10001
# define E_BACKREF_TO_SELF     10002
# define E_TEXT_TOO_SHORT      10003
# define E_OUT_RANGED          10004
# define E_INVALID_UNICODE_CHAR    10005
# define E_INVALID_UNICODE_PROPERTY 10006
# define E_INVALID_UNICODE_CATEGORY 10007
# define E_NO_SUCH_RULE        10008
# define E_NO_ALTERNATIVE       10009
# define E_NO_PROGRESSING       10010
# define E_INSUFFICIENT_REPEAT  10011
# define E_INSUFFICIENT_REPEAT2 10012
# define E_EXCESSIVE_REPEAT     10013
# define E_LEFT_RECUR_NO_LIFT   10014
# define E_VIOLATE_NEGATIVE     10015
# define E_MAX_RECURSION        10016
# define E_INVALID_MATCH_CALLBACK 10017
# define E_INVALID_ERROR_CALLBACK 10018
# define E_NO_EXPR              10019
# define E_WRONG_BACKREF        10020
# define E_RECURSIVE_BACKREF    10021

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

/**
 * The C string type in locale encoding, by default utf-8.
 **/
typedef char*           P4_String;
typedef const char*     P4_ConstString;

/**
 * The utf-8 string type.
 */
typedef uint8_t*        P4_Utf8;

/**
 * The size type.
 */
#ifdef P4_SIZE_T_UINT
typedef unsigned int        P4_size_t;
#define P4_SIZE_T_FMT "%u"
#define P4_SIZE_T_XFMT "%ux"
#define P4_SIZE_MAX UINT_MAX
#else
typedef size_t        P4_size_t;
#define P4_SIZE_T_FMT "%zu"
#define P4_SIZE_T_XFMT "%zx"
#define P4_SIZE_MAX SIZE_MAX
#endif

/**
 * The reference of user data.
 */
typedef void*   P4_UserData;

/**
 * The function to free user data.
 */
typedef void    (*P4_UserDataFreeFunc)(P4_UserData);

/**
 * The grammar object that holds all grammar rules.
 */
typedef struct P4_Grammar P4_Grammar;

/**
 * The grammar rule expression.
 */
typedef struct P4_Expression P4_Expression;

/**
 * The stack frame.
 */
typedef struct P4_Frame P4_Frame;

/**
 * The node object of abstract syntax tree.
 */
typedef struct P4_Node P4_Node;

/**
 * The source object that holds text to parse.
 */
typedef struct P4_Source P4_Source;

/**
 * The slice of a string.
 */
typedef struct P4_Slice P4_Slice;

/**
 * The callback for a successful match.
 */
typedef P4_Error (*P4_MatchCallback)(P4_Grammar*, P4_Expression*, P4_Node*);

/**
 * The callback for a failure match.
 */
typedef P4_Error (*P4_ErrorCallback)(P4_Grammar*, P4_Expression*);

/**
 * A formatter function for the node.
 */
typedef void (*P4_Formatter)(FILE* stream, P4_Node* node);

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
    P4_size_t              pos;
    /** The line number in the string. */
    P4_size_t              lineno;
    /** The col offset in the line. */
    P4_size_t              offset;
} P4_Position;

/**
 * P4_RuneRange specifies a range between two runes.
 *
 * Example:
 *
 *      P4_RuneRange range = { .lower='a', .upper='z', .stride=1 };
 */
typedef struct P4_RuneRange {
    /** The lower code point of the range (inclusive). */
    ucs4_t                 lower;
    /** The upper code point of the range (inclusive). */
    ucs4_t                 upper;
    /** The step to jump inside the range. */
    P4_size_t                  stride;
} P4_RuneRange;

/**
 *
 * P4_Slice does not hold the pointer to the string.
 * It only stores the start and stop position of the string.
 *
 * Example:
 *
 *      P4_Slice slice = {
 *          i=0,
 *          j=strlen("hello world")
 *      };
 *      printf("%u..%u\n", slice.i, slice.j);
 **/
struct P4_Slice {
    /** The start position of the slice. */
    P4_Position         start;
    /** The stop position of the slice. */
    P4_Position         stop;
};

/**
 * AST Node.
 */
struct P4_Node {
    /** the full text. */
    P4_ConstString               text;
    /** The matched substring.
     * slice.start is the beginning (inclusive), and slice.stop is the end (exclusive).
     */
    P4_Slice                slice;

    /** The matched grammar rule name. */
    P4_String               rule_name;

    /** The user data. */
    P4_UserData             userdata;

    /** the sibling node. NULL if not exists. */
    struct P4_Node*        next;
    /** the first child of inner nodes. NULL if not exists. */
    struct P4_Node*        head;
    /** the last child of inner nodes. NULL if not exists. */
    struct P4_Node*        tail;
};

/**
 * The result object that holds either value or errors.
 */
typedef struct P4_Result {
    union {
        P4_String               str;
        ucs4_t                  rune;
        P4_size_t               num;
        P4_ExpressionFlag       flag;
        /* The expression result. */
        P4_Expression*          expr;
        /* The grammar result. */
        P4_Grammar*             grammar;
    };
    /** The error message. */
    char                        errmsg[256];
}                               P4_Result;

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
P4_size_t         P4_ReadRune(P4_String s, ucs4_t* c);

/**
 * Read an escaped single code point (rune) from an UTF-8 string.
 *
 * @param       text        The text.
 * @param       rune        The rune.
 * @return      The size of rune.
 *
 * For example:
 *
 *      ucs4_t rune;
 *      P4_ReadEscapedRune("\\u000D", rune); // 0xd
 */
P4_size_t         P4_ReadEscapedRune(char* text, ucs4_t* rune);

/**
 * Append a rune to str (in-place).
 *
 * @param       str     The string to be appended.
 * @param       chr     The rune.
 * @param       n       The size of rune. Should be 1, 2, 3, 4.
 * @return      The string starting from appended rune.
 */
void*          P4_ConcatRune(void* str, ucs4_t chr, P4_size_t n);

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
P4_Expression* P4_CreateLiteral(const P4_String literal, bool sensitive);

/**
 * Add a literal expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   literal     The exact string to match.
 * @param   sensitive   Whether the string is case-sensitive.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddLiteral(grammar, "R1", "let", true);
 */
P4_Error       P4_AddLiteral(P4_Grammar* grammar, P4_String name, const P4_String literal, bool sensitive);

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
P4_Expression* P4_CreateRange(ucs4_t lower, ucs4_t upper, P4_size_t stride);

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
P4_Expression* P4_CreateRanges(P4_size_t count, P4_RuneRange* ranges);

/**
 * Add a range expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   lower       The lower bound of UTF-8 rule to match (inclusive).
 * @param   upper       The upper bound of UTF-8 rule to match (inclusive).
 * @param   stride      The stride when iterating the characters in range.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddRange(grammar, "R1", '0', '9', 1);
 *
 *      P4_AddRange(grammar, "R1", 0x4E00, 0x9FFF, 1);
 */
P4_Error       P4_AddRange(P4_Grammar* grammar, P4_String name, ucs4_t lower, ucs4_t upper, P4_size_t stride);

/**
 * Add sub-ranges expression as grammar rule.
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   count       The total number of ranges.
 * @param   ranges      A list of P4_RuneRange.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_RuneRange alphadigits[] = {{'a', 'Z', 1}, {'0', '9', 1}};
 *      P4_Error err = P4_AddRanges(
 *          grammar, "R1",
 *          sizeof(alphadigits) / sizeof(P4_RuneRange),
 *          alphadigits
 *      );
 */
P4_Error       P4_AddRanges(P4_Grammar* grammar, P4_String name, P4_size_t count, P4_RuneRange* ranges);

/**
 * Create a P4_Reference expression.
 *
 * @param   reference   The grammar rule name.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* expr = P4_CreateReference("entry");
 *
 *
 */
P4_Expression* P4_CreateReference(P4_String reference);

/**
 * Add a reference expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   ref_name    The name of referenced grammar rule.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddReference(grammar, "R1", 2);
 */
P4_Error       P4_AddReference(P4_Grammar* grammar, P4_String name, P4_String ref_name);

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
P4_Expression* P4_CreatePositive(P4_Expression* refexpr);

/**
 * Add a positive expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   refexpr     The positive pattern to check.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddPositive(grammar, "R1", P4_CreateLiteral("let", true));
 */
P4_Error       P4_AddPositive(P4_Grammar* grammar, P4_String name, P4_Expression* refexpr);

/**
 * Create a P4_Negative expression.
 *
 * @param   expr        The negative pattern to check.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // If the following text does not start with "let", the match is successful.
 *      P4_Expression* expr = P4_CreateNegative(P4_CreateLiteral("let", true));
 *
 */
P4_Expression* P4_CreateNegative(P4_Expression* expr);

/**
 * Add a negative expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   refexpr     The negative pattern to check.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddNegative(grammar, "R1", P4_CreateLiteral("let", true));
 */
P4_Error       P4_AddNegative(P4_Grammar* grammar, P4_String name, P4_Expression* refexpr);

/**
 * Create a P4_Cut expression.
 *
 * @return  A P4_Expression.
 *
 */
P4_Expression* P4_CreateCut();

/**
 * Create a P4_LeftRecursion expression.
 *
 * @param   lhs     Left-hand side of left recursion.
 * @param   rhs     Right-hand side of left recursion.
 * @return  A P4_Expression.
 */
P4_Expression* P4_CreateLeftRecursion(P4_Expression* lhs, P4_Expression* rhs);

/**
 * Add a P4_LeftRecursion expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   lhs     Left-hand side of left recursion.
 * @param   rhs     Right-hand side of left recursion.
 * @return  The error code.
 */
P4_Error       P4_AddLeftRecursion(P4_Grammar* grammar, P4_String name, P4_Expression* lhs, P4_Expression* rhs);

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
P4_Expression* P4_CreateSequence(P4_size_t count);

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
P4_Expression* P4_CreateSequenceWithMembers(P4_size_t count, ...);

/**
 * Add a sequence expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   count       The number of sequence members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddSequence(grammar, "R1", 3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 */
P4_Error       P4_AddSequence(P4_Grammar* grammar, P4_String name, P4_size_t count);

/**
 * Add a sequence expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   count       The number of sequence members.
 * @param   ...         The members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddSequenceWithMembers(grammar, "R1", 3,
 *          P4_CreateLiteral("{"),
 *          P4_CreateReference(BODY),
 *          P4_CreateLiteral("}")
 *      );
 */
P4_Error       P4_AddSequenceWithMembers(P4_Grammar* grammar, P4_String name, P4_size_t count, ...);

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
P4_Expression* P4_CreateChoice(P4_size_t count);

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
P4_Expression* P4_CreateChoiceWithMembers(P4_size_t count, ...);

/**
 * Add a choice expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   count       The number of choice members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddChoice(grammar, "R1", 3);
 *
 * Note that such an expression is useless as its members are all empty.
 * Please set members using P4_SetMembers.
 *
 * This function can be useful if you need to add members dynamically.
 */
P4_Error       P4_AddChoice(P4_Grammar* grammar, P4_String name, P4_size_t count);

/**
 * Add a choice expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   count       The number of choice members.
 * @param   ...         The members.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_AddChoiceWithMembers(grammar, "R1", 3,
 *          P4_CreateLiteral(" "),
 *          P4_CreateReference(\t),
 *          P4_CreateLiteral("\n")
 *      );
 */
P4_Error       P4_AddChoiceWithMembers(P4_Grammar* grammar, P4_String name, P4_size_t count, ...);

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
P4_Expression* P4_CreateRepeatMinMax(P4_Expression* repeat_expr, P4_size_t min, P4_size_t max);

/**
 * Create a P4_Repeat expression minimal min times and maximal max times.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @param   max         The maximum repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", or "aaa".
 *      P4_AddRepeatMinMax(grammar, "R1",
 *          P4_CreateLiteral("a", true),
 *          1, 3
 *      );
 */
P4_Error       P4_AddRepeatMinMax(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr, P4_size_t min, P4_size_t max);

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
P4_Expression* P4_CreateRepeatMin(P4_Expression* repeat_expr, P4_size_t min);

/**
 * Create a RepeatMin expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @param   min         The minimum repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_AddRepeatMin(grammar, "R1", P4_CreateLiteral("a", true), 1);
 */
P4_Error       P4_AddRepeatMin(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr, P4_size_t min);

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
P4_Expression* P4_CreateRepeatMax(P4_Expression* repeat_expr, P4_size_t max);

/**
 * Add a RepeatMax expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @param   max         The maximum repeat times.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      // It can match string "", "a", "aa", "aaa".
 *      P4_AddRepeatMax(grammar, "name", P4_CreateLiteral("a", true), 3);
 */
P4_Error       P4_AddRepeatMax(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr, P4_size_t max);

/**
 * Create a P4_Repeat expression exact N times.
 *
 * @param   repeat_expr The repeated expression.
 * @param   times       The repeat times.
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
P4_Expression* P4_CreateRepeatExact(P4_Expression* repeat_expr, P4_size_t times);

/**
 * Add a RepeatExact expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @param   times       The repeat times.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "aaa".
 *      P4_AddRepeatExact(grammar, "R1", P4_CreateLiteral("a", true), 3);
 */
P4_Error       P4_AddRepeatExact(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr, P4_size_t times);

/**
 * Create a P4_Repeat expression zero or once.
 *
 * @param   expr        The repeated expression.
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
P4_Expression* P4_CreateZeroOrOnce(P4_Expression* expr);

/**
 * Add a ZeroOrOnce expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_AddZeroOrOnce(grammar, "R1", P4_CreateLiteral("a", true));
 *
 * It's equivalent to P4_CreateRepeatMinMax(expr, 0, 1);
 */
P4_Error       P4_AddZeroOrOnce(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr);

/**
 * Create a P4_Repeat expression zero or more times.
 *
 * @param   expr        The repeated expression.
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
P4_Expression* P4_CreateZeroOrMore(P4_Expression* expr);

/**
 * Add a ZeroOrMore expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "" or "a".
 *      P4_AddZeroOrMore(grammar, "R1", P4_CreateLiteral("a", true));
 */
P4_Error       P4_AddZeroOrMore(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr);

/**
 * Create a P4_Repeat expression once or more times.
 *
 * @param   expr        The repeated expression.
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
P4_Expression* P4_CreateOnceOrMore(P4_Expression* expr);

/**
 * Add an OnceOrMore expression as grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   repeat_expr The repeated expression.
 * @return  The error code.
 *
 * Example:
 *
 *      // It can match string "a", "aa", "aaa", ....
 *      P4_AddOnceOrMore(grammar, "R1", P4_CreateLiteral("a", true));
 */
P4_Error       P4_AddOnceOrMore(P4_Grammar* grammar, P4_String name, P4_Expression* repeat_expr);

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
P4_Expression* P4_CreateBackReference(P4_size_t backref_index, bool sensitive);

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
P4_Error       P4_SetMember(P4_Expression* expr, P4_size_t index, P4_Expression* member);

/**
 * Set the referenced member of Sequence/Choice at a given index.
 *
 * @param   expr                The sequence/choice expression.
 * @param   index               The index of member.
 * @param   name                The reference name of grammar rule for member.
 * @return  The error code.
 *
 * It's equivalent to:
 *
 *      P4_SetMember(expr, index, P4_CreateReference("member_n"));
 */
P4_Error       P4_SetReferenceMember(P4_Expression* expr, P4_size_t index, P4_String name);

/**
 * Get the total number members for sequence/choice.
 *
 * @param   expr        The sequence/choice expression.
 * @return  The number. If something goes wrong, it returns 0.
 *
 *      P4_Expression* expr = P4_CreateSequence(3);
 *      P4_size_t  count = P4_GetMembers(expr); // 3
 */
P4_size_t         P4_GetMembersCount(P4_Expression* expr);

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
P4_Expression* P4_GetMember(P4_Expression* expr, P4_size_t index);

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
 *      P4_AddSequenceWithMembers(grammar, "R1", 2,
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
 *      P4_AddSequenceWithMembers(grammar, "R1", 2,
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
 * @param   name        The grammar rule name.
 * @param   joiner      A joiner literal.
 * @param   reference   The repeated pattern rule name.
 * @return  The error code.
 *
 * Example:
 *
 *      P4_Expression* row = P4_AddJoin(grammar, "R1", ",", "element");
 */
P4_Error P4_AddJoin(P4_Grammar* grammar, P4_String name, const P4_String joiner, P4_String reference);

/**
 * Create a Join expression.
 *
 * @param   joiner  A joiner literal.
 * @param   rule_name The repeated pattern rule name.
 * @return  A P4_Expression.
 *
 * Example:
 *
 *      P4_Expression* row = P4_CreateJoin(",", "element");
 *
 * Join is a syntax sugar to the below structure:
 *
 *      Sequence(pattern, ZeroOrMore(Sequence(Literal(joiner), pattern)))
 */
P4_Expression* P4_CreateJoin(const P4_String joiner, P4_String rule_name);


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
void           P4_DeleteExpression(P4_Expression* expr);

/**
 * Check if an expression is a grammar rule.
 *
 * @param   expr        The expression.
 * @return  if an expression is a grammar rule.
 *
 *      P4_AddLiteral(grammar, "R1", "a", true);
 *      P4_IsRule(P4_GetGrammarRule(grammar, 1)); // true
 */
bool           P4_IsRule(P4_Expression* expr);

/**
 * Check if an expression has P4_FLAG_SQUASHED flag.
 */
bool           P4_IsSquashed(P4_Expression* expr);

/**
 * Check if an expression has P4_FLAG_LIFTED flag.
 */
bool           P4_IsLifted(P4_Expression* expr);

/**
 * Check if an expression has P4_FLAG_TIGHT flag.
 */
bool           P4_IsTight(P4_Expression* expr);

/**
 * Check if an expression has P4_FLAG_SCOPED flag.
 */
bool           P4_IsScoped(P4_Expression* expr);

/**
 * Check if an expression has P4_FLAG_SPACED flag.
 */
bool           P4_IsSpaced(P4_Expression* expr);

/**
 * @brief       Set the flag for an expression.
 * @param       expr    The expression.
 * @param       flag    The flag to set.
 *
 * Example:
 *
 *      P4_SetExpressionFlag(expr, P4_FLAG_SQUASHED);
 *      P4_SetExpressionFlag(expr, P4_FLAG_TIGHT | P4_FLAG_SQUASHED);
 */
void           P4_SetExpressionFlag(P4_Expression* expr, P4_ExpressionFlag flag);

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
void           P4_DeleteGrammar(P4_Grammar* grammar);

/**
 * Add a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @param   expr        The grammar rule expression.
 * @return  The error code.
 */
P4_Error       P4_AddGrammarRule(P4_Grammar* grammar, P4_String name, P4_Expression* expr);

/**
 * Delete a grammar rule.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 *
 * WARNING: NOT IMPLEMENTED.
 */
void           P4_DeleteGrammarRule(P4_Grammar* grammar, const P4_String name);

/**
 * Get a grammar rule by its name.
 *
 * @param   grammar     The grammar.
 * @param   name        The grammar rule name.
 * @return  The grammar rule expression. Returns NULL if not found.
 *
 *      P4_AddLiteral(grammar, "a", true);
 *      P4_Expression* expr = P4_GetGrammarRule(grammar, "a"); // The literal expression.
 */
P4_Expression* P4_GetGrammarRule(P4_Grammar* grammar, P4_String name);

/**
 * @brief       Set the flag of a grammar rule.
 * @param       grammar     The grammar.
 * @param       name        The grammar rule name.
 * @param       flag        The bits of \ref P4_ExpressionFlag.
 * @return      The error code. If successful, return \ref P4_Ok.
 *
 * Example:
 *
 *      P4_Error err = P4_SetGrammarRuleFlag(grammar, "entry", P4_FLAG_SQUASHED | P4_FLAG_LIFTED | P4_FLAG_TIGHT);
 *      if (err != P4_Ok) {
 *          printf("err=%u\n", err);
 *          exit(1);
 *      }
 */
P4_Error       P4_SetGrammarRuleFlag(P4_Grammar* grammar, P4_String name, P4_ExpressionFlag flag);

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
P4_Error       P4_SetRecursionLimit(P4_Grammar* grammar, P4_size_t limit);

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
 *      P4_size_t  limit = P4_GetRecursionLimit(grammar);
 *      printf("limit=%u\n", limit);
 */
P4_size_t         P4_GetRecursionLimit(P4_Grammar* grammar);

/**
 * @brief       Create a \ref P4_Source* object.
 * @param       content     The content of input.
 * @param       entry_name  The entry grammar rule name.
 * @return      A source object.
 *
 * Example:
 *
 *      char*       content = ... // read from some files.
 *
 *      P4_Source*  source  = P4_CreateSource(content, "entry");
 *
 *      // do something...
 *
 *      P4_DeleteSource(source);
 */
P4_Source*     P4_CreateSource(P4_ConstString content, P4_ConstString entry_name);
void           P4_SetDebugSource(P4_Source *src, bool debug, P4_size_t line_start, P4_size_t line_end);

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
 *      P4_Source* source = P4_CreateSource(input, "a");
 *      if (P4_Ok != P4_SetSourceSlice(source, 1, 2)) // only parse "a"
 *          printf("set buf size error\n");
 */
P4_Error       P4_SetSourceSlice(P4_Source* source, P4_size_t start, P4_size_t stop);

/**
 * @brief       Get the root node of abstract syntax tree of the source.
 * @param       source  The source.
 * @return      The root node. If the parse is failed or the root node is lifted, return NULL.
 */
P4_Node*      P4_GetSourceAst(P4_Source* source);


/**
 * @brief       Transfer the ownership of source ast to the caller.
 * @param       source  The source.
 * @return      The root node. If the parse is failed or the root node is lifted, return NULL.
 *
 * You're on your own now to manage the de-allocation of the source ast.
 *
 * The source is implicitly reset after the source ast is acquired by the caller.
 *
 * Example:
 *
 *      P4_Node* root = P4_AcquireSourceAst(source);
 *      // ...
 *      P4_DeleteNode(grammar, root);
 */
P4_Node*      P4_AcquireSourceAst(P4_Source* source);

/**
 * @brief       Get the last position in the input after a parse.
 * @param       source  The source.
 * @return      The position in the input.
 */
P4_size_t          P4_GetSourcePosition(P4_Source* source);

/**
 * @brief       Print the node tree.
 * @param       stream     The output stream.
 * @param       node       The root node of source ast.
 * *param       formatter  A callback function to format node.
 *
 * Example:
 *
 *      P4_Node* root = P4_GetSourceAst(source);
 *      P4_JsonifySourceAst(stdout, root);
 */
void    P4_JsonifySourceAst(FILE* stream, P4_Node* node, P4_Formatter formatter);
void    P4_Jsonify2SourceAst(FILE* stream, P4_Node* node, P4_Formatter formatter);
void    P4_TxtSourceAst(FILE* stream, P4_Node* node, int depth);
void    P4_NakedSourceAst(FILE* stream, P4_Node* node, int depth, const char *sep);

/**
 * @brief       Inspect the node tree.
 * @param       node       The root node of source ast.
 * @param       userdata    Any additional information you want to pass in.
 * @param       inspector   The inspecting function. It should return P4_Ok for a successful inspection.
 *
 * Example:
 *
 *      void MyInspector(P4_Node* node, void* userdata) {
 *          printf("%lu\t%lu\t%p\n", node->slice.start.pos, P4_GetRuleName(node), userdata);
 *          return P4_Ok;
 *      }
 *
 *      P4_Error err = P4_InspectSourceAst(root, NULL, MyInspector);
 */
P4_Error       P4_InspectSourceAst(P4_Node* node, void* userdata, P4_Error (*inspector)(P4_Node*, void*));

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
P4_Error       P4_Parse(P4_Grammar* grammar, P4_Source* source);

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
bool           P4_HasError(P4_Source* source);

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
P4_Error       P4_GetError(P4_Source* source);

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
P4_String      P4_GetErrorMessage(P4_Source* source);

/**
 * @brief       Create a node.
 * @param       text    The source text.
 * @param       start   The starting position of the text.
 * @param       stop    The stopping position of the text.
 * @param       rule    The name of rule expression that matches to the slice of the text.
 * @return      The node.
 *
 * Example:
 *
 *      P4_String       str     = "Hello world";
 *      P4_size_t       start   = 0;
 *      P4_size_t       stop    = 11;
 *      P4_String       rule    = "entry"
 *
 *      P4_Node* node = P4_CreateNode(text, start, stop, rule);
 *
 *      // do something.
 *
 *      P4_DeleteNode(grammar, node);
 */
P4_Node*      P4_CreateNode(P4_ConstString text, P4_Position* start, P4_Position* stop, P4_String rule);

/**
 * @brief       Delete the node.
 *              This will free the occupied memory for node.
 *              The str of the node won't be free-ed since the node only owns not the string but the slice of a string.
 * @param       grammar The grammar.
 * @param       node    The node.
 *
 * Example:
 *
 *      P4_DeleteNode(grammar, node);
 */
void           P4_DeleteNode(P4_Grammar* grammar, P4_Node* node);

/**
 * @brief       Delete the node children.
 *              This will free the occupied memory for all node children.
 * @param       grammar The grammar.
 * @param       node    The node.
 *
 * Example:
 *
 *      P4_DeleteNodeChildren(grammar, node);
 */
void P4_DeleteNodeChildren(P4_Grammar* grammar, P4_Node* node);

/**
 * @brief       Get the slice that the node covers.
 *              The slice is owned by the node so don't free it.
 * @param       node   The node.
 * @return      The slice.
 *
 * Example:
 *
 *      P4_Slice* slice = P4_GetNodeSlice(node);
 *      printf("node slice=[%u..%u]\n", slice->i, slice->j);
 */
P4_Slice*      P4_GetNodeSlice(P4_Node* node);


/**
 * @brief       Get the total number of node children.
 * @param       node   The node.
 * @return      The total number of node children.
 *
 * Example:
 *
 *      P4_Node* root = P4_GetSourceAst(source);
 *      P4_size_t count = P4_GetNodeChildrenCount(root);
 *      printf("There are %lu children for root node\n", count);
 */
P4_size_t         P4_GetNodeChildrenCount(P4_Node* node);

/**
 * @brief       Copy the string that the node covers.
 *              The caller is responsible for freeing the string.
 *
 * @param       node   The node.
 * @return      The string.
 *
 * Example:
 *
 *      P4_String* str = P4_CopyNodeString(node);
 *      printf("node str=%s\n", str);
 *      free(str);
 */
P4_String      P4_CopyNodeString(P4_Node* node);


/**
 * @brief       Set callback function.
 * @param       grammar     The grammar.
 * @param       matchcb     The callback on a successful match.
 * @param       errcb       The callback on a failure match.
 * @return      The error code.
 */
P4_Error       P4_SetGrammarCallback(P4_Grammar* grammar, P4_MatchCallback matchcb, P4_ErrorCallback errcb);

/**
 * @brief       Replace an existing grammar rule.
 * @param       grammar     The grammar.
 * @param       name        The rule name.
 * @param       expr        The rule expression to replace.
 * @return      The error code.
 *
 * The original grammar rule will be deleted.
 */
P4_Error       P4_ReplaceGrammarRule(P4_Grammar* grammar, P4_String name, P4_Expression* expr);

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
 * @brief       Load peg grammar result from a string.
 * @param       rules   The rules string.
 * @param       result  The P4_Result object.
 * @return      The error code.
 *
 * To get the result grammar, if return P4_Ok, use result->grammar.
 *
 * Example:
 *
 *      P4_Result result = {0};
 *
 *      if (P4_Ok == P4_LoadGrammarResult(RULES, &result)) {
 *          P4_Grammar* grammar = result.grammar;
 *      } else {
 *          printf("%s\n", result.errmsg);
 *      }
 */
P4_Error P4_LoadGrammarResult(P4_ConstString rules, P4_Result* result);

/**
 * @brief       Load PEG grammar written in string.
 * @param       rules   The grammar rules string.
 * @return      The grammar object.
 *
 * Example:
 *
 *      P4_Grammar* grammar = P4_LoadGrammar(
 *          "entry = one one;\n"
 *          "one   = \"1\";\n"
 *      );
 *      P4_Source* source1 = P4_CreateSource("11", "entry");
 *      P4_Parse(grammar, source1);
 *
 *      P4_Source* source2 = P4_CreateSource("1", "one");
 *      P4_Parse(grammar, source2);
 *
 * This function exits the program when an error occurs.
 *
 */
P4_Grammar*     P4_LoadGrammar(P4_ConstString rules);

/**
 * @brief       Get the rule name.
 * @param       expr    The rule expression.
 * @return      The rule name.
 *
 * Example:
 *
 *      const P4_String name = P4_GetRuleName(expr);
 */
const P4_String P4_GetRuleName(P4_Expression* expr);

#ifdef __cplusplus
}
#endif

# endif
