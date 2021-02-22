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
 * @version    1.4.0
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

# define P4_PUBLIC
# define P4_PRIVATE(type) static type

# define P4_MAJOR_VERSION 1
# define P4_MINOR_VERSION 3
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

/*
 * The default recursion limit.
 *
 * It can be adjusted through function `P4_SetRecursionLimit`.
 */
# define P4_DEFAULT_RECURSION_LIMIT     8192

/* The flag of expression. */
typedef uint32_t        P4_ExpressionFlag;

/**
 * @brief The position of a string.
 **/
typedef size_t          P4_Position;

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

/* The expression kinds. */
typedef enum {
    P4_Literal,
    P4_Range,
    P4_Reference,
    P4_Positive,
    P4_Negative,
    P4_Sequence,
    P4_Choice,
    P4_Repeat,
    P4_BackReference,
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

typedef struct P4_Frame {
    /* The current matching expression for the frame. */
    struct P4_Expression*   expr;
    /* Whether spacing is applicable to frame & frame dependents. */
    bool                    space;
    /* Whether silencing is applicable to frame & frame dependents. */
    bool                    silent;
    /* The next frame in the stack. */
    struct P4_Frame*        next;
} P4_Frame;

typedef struct P4_Source {
    /* The grammar used to parse the source. */
    struct P4_Grammar*      grammar;
    /* The ID of entry rule in the grammar used to parse the source. */
    P4_RuleID               rule_id;
    /* The content of the source. */
    P4_String               content;
    /* The position of the consumed input. Min: 0, Max: strlen(content).
     *
     * It's possible the pos is less then length of content when the Source
     * is successfully parsed. It's called a partial parse.
     *
     * To avoid that, the rule will need to be wrapped with an EOI and SOI.
     * An SOI is Positive(Range(1, 0x10ffff))
     * and An EOI is Negative(Range(1, 0x10ffff)). When the rule is wrapped,
     * the input is guaranteed to be parsed until all bits are consumed.
     * */
    P4_Position             pos;
    /* The error code of the parse. */
    P4_Error                err;
    /* The error message of the parse. */
    P4_String               errmsg;
    /* The root of abstract syntax tree. */
    struct P4_Token*        root;
    /* Reserved: whether to enable DEBUG logs. */
    bool                    verbose;
    /* The flag for checking if the parse is matching SPACED rules.
     *
     * Since we're wrapping SPACED rules into a repetition rule internally,
     * it's important to prevent matching SPACED rules in P4_MatchRepeat.
     *
     * XXX: Maybe there are some better ways to prevent that?
     */
    bool                    whitespacing;
    /* The top frame in the stack. */
    struct P4_Frame*        frame_stack;
    /* The size of frame stack. */
    size_t                  frame_stack_size;
} P4_Source;

typedef struct P4_Expression {
    /* The name of expression (only for debugging). */
    /* P4_String            name; */
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
            struct P4_Expression*   repeat_expr; /* maybe we can merge it with ref_expr? */
            size_t                  repeat_min;
            size_t                  repeat_max;
        };

        /* Used by P4_BackReference. */
        size_t                      backref_index;
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
    /* The rules, e.g. the expressions with IDs. */
    struct P4_Expression**  rules;
    /* The total number of rules. */
    size_t                  count;
    /* The maximum number of rules. */
    int                     cap;
    /* The total number of spaced rules. */
    size_t                  spaced_count;
    /* The repetition rule for spaced rules. */
    struct P4_Expression*   spaced_rules;
    /* The recursion limit, or maximum allowed nested rules. */
    size_t                  depth;
} P4_Grammar;


/**
 * Provide the version string for the library.
 *
 * @return a string like "1.0.0".
 */
P4_PUBLIC P4_String      P4_Version(void);


/* P4_PUBLIC P4_Expression* P4_CreateNumeric(size_t); */
P4_PUBLIC P4_Expression* P4_CreateLiteral(const P4_String, bool sensitive);
P4_PUBLIC P4_Expression* P4_CreateRange(P4_Rune, P4_Rune);
P4_PUBLIC P4_Expression* P4_CreateReference(P4_RuleID);
P4_PUBLIC P4_Expression* P4_CreatePositive(P4_Expression*);
P4_PUBLIC P4_Expression* P4_CreateNegative(P4_Expression*);
P4_PUBLIC P4_Expression* P4_CreateSequence(size_t);
P4_PUBLIC P4_Expression* P4_CreateChoice(size_t);
P4_PUBLIC P4_Expression* P4_CreateSequenceWithMembers(size_t, ...);
P4_PUBLIC P4_Expression* P4_CreateChoiceWithMembers(size_t, ...);
P4_PUBLIC P4_Expression* P4_CreateRepeatMinMax(P4_Expression*, size_t, size_t);
P4_PUBLIC P4_Expression* P4_CreateRepeatMin(P4_Expression*, size_t);
P4_PUBLIC P4_Expression* P4_CreateRepeatMax(P4_Expression*, size_t);
P4_PUBLIC P4_Expression* P4_CreateRepeatExact(P4_Expression*, size_t);
P4_PUBLIC P4_Expression* P4_CreateZeroOrOnce(P4_Expression*);
P4_PUBLIC P4_Expression* P4_CreateZeroOrMore(P4_Expression*);
P4_PUBLIC P4_Expression* P4_CreateOnceOrMore(P4_Expression*);
P4_PUBLIC P4_Expression* P4_CreateBackReference(size_t);

P4_PUBLIC P4_Error       P4_SetMember(P4_Expression*, size_t, P4_Expression*);
P4_PUBLIC P4_Error       P4_SetReferenceMember(P4_Expression*, size_t, P4_RuleID);
P4_PUBLIC size_t         P4_GetMembersCount(P4_Expression*);
P4_PUBLIC P4_Expression* P4_GetMember(P4_Expression*, size_t);

P4_PUBLIC void           P4_DeleteExpression(P4_Expression*);

P4_PUBLIC P4_String      P4_PrintExpression(P4_Expression*);

P4_PUBLIC P4_Error       P4_SetRuleID(P4_Expression*, P4_RuleID);
P4_PUBLIC bool           P4_IsRule(P4_Expression*);

P4_PUBLIC bool           P4_IsSquashed(P4_Expression*);
P4_PUBLIC bool           P4_IsLifted(P4_Expression*);
P4_PUBLIC bool           P4_IsTight(P4_Expression*);
P4_PUBLIC bool           P4_IsScoped(P4_Expression*);
P4_PUBLIC bool           P4_IsSpaced(P4_Expression*);
P4_PUBLIC void           P4_SetExpressionFlag(P4_Expression*, P4_ExpressionFlag);
P4_PUBLIC void           P4_SetSquashed(P4_Expression*);
P4_PUBLIC void           P4_SetLifted(P4_Expression*);
P4_PUBLIC void           P4_SetTight(P4_Expression*);
P4_PUBLIC void           P4_SetSpaced(P4_Expression*);
P4_PUBLIC void           P4_SetScoped(P4_Expression*);

P4_PUBLIC P4_Grammar*    P4_CreateGrammar(void);
P4_PUBLIC void           P4_DeleteGrammar(P4_Grammar*);

P4_PUBLIC P4_Error       P4_AddGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddNumeric(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC P4_Error       P4_AddLiteral(P4_Grammar*, P4_RuleID, const P4_String, bool sensitive);
P4_PUBLIC P4_Error       P4_AddRange(P4_Grammar*, P4_RuleID, P4_Rune, P4_Rune);
P4_PUBLIC P4_Error       P4_AddReference(P4_Grammar*, P4_RuleID, P4_RuleID);
P4_PUBLIC P4_Error       P4_AddPositive(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddNegative(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddSequence(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC P4_Error       P4_AddSequenceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);
P4_PUBLIC P4_Error       P4_AddChoice(P4_Grammar*, P4_RuleID, size_t);
P4_PUBLIC P4_Error       P4_AddChoiceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);
P4_PUBLIC P4_Error       P4_AddZeroOrOnce(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddZeroOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddOnceOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_PUBLIC P4_Error       P4_AddRepeatMin(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_PUBLIC P4_Error       P4_AddRepeatMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_PUBLIC P4_Error       P4_AddRepeatMinMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t, size_t);
P4_PUBLIC P4_Error       P4_AddRepeatExact(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);

P4_PUBLIC void           P4_DeleteGrammarRule(P4_Grammar*, P4_RuleID);
P4_PUBLIC P4_Expression* P4_GetGrammarRule(P4_Grammar*, P4_RuleID);
P4_PUBLIC P4_Error       P4_SetGrammarRuleFlag(P4_Grammar*, P4_RuleID, P4_ExpressionFlag);

/**
 * @brief       Set the maximum allowed recursion calls.
 * @param       grammar     The grammar.
 * @param       limit       The number of maximum recursion calls.
 * @return      An error. If successful, return \ref P4_Ok; otherwise, return other \ref P4_Error.
 *
 * Example:
 *
 *      // on a machine with small memory.
 *      P4_SetRecursionLimit(grammar, 256);
 *
 *      // on a machine with large memory.
 *      P4_SetRecursionLimit(grammar, 1024*20);
 */
P4_PUBLIC P4_Error       P4_SetRecursionLimit(P4_Grammar*, size_t limit);

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
P4_PUBLIC size_t         P4_GetRecursionLimit(P4_Grammar*);

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
P4_PUBLIC P4_Source*     P4_CreateSource(P4_String, P4_RuleID);

/**
 * @brief       Free the allocated memory of a source.
 *              If the source has been parsed and has an AST, the entire AST will also be free-ed.
 * @param       source  The source.
 *
 * Example:
 *
 *      P4_DeleteSource(source);
 */
P4_PUBLIC void           P4_DeleteSource(P4_Source*);

/**
 * @brief       Get the root token of abstract syntax tree of the source.
 * @param       source  The source.
 * @return      The root token. If the parse is failed or the root token is lifted, return NULL.
 */
P4_PUBLIC P4_Token*      P4_GetSourceAst(P4_Source*);
/**
 * @brief       Get the last position in the input after a parse.
 * @param       source  The source.
 * @return      The position in the input.
 */
P4_PUBLIC P4_Position    P4_GetSourcePosition(P4_Source*);

/**
 * @brief       Print the token tree.
 * @param       token   The token.
 * @param       buf     The buffer of output. It should be big enough to hold all of the outputs.
 * @param       indent  The indentation. Generally, set it to 0.
 */
P4_PUBLIC void           P4_PrintSourceAst(P4_Token*, P4_String, size_t);

/**
 * @brief       Parse the source given a grammar.
 * @param       grammar     The grammar.
 * @param       source      The source.
 * @return      The error code.
 *              When the parse is successful, return \ref P4_Ok.
 *              Otherwise, return the other \ref P4_Error, such as \ref P4_MatchError.
 *
 * Example:
 *
 *      if ((err = P4_Parse(grammar, source)) != P4_Ok) {
 *          // error handling ...
 *      }
 */
P4_PUBLIC P4_Error       P4_Parse(P4_Grammar*, P4_Source*);

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
P4_PUBLIC bool           P4_HasError(P4_Source*);

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
P4_PUBLIC P4_Error       P4_GetError(P4_Source*);

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
 */
P4_PUBLIC P4_String      P4_GetErrorMessage(P4_Source*);

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
P4_PUBLIC P4_Token*      P4_CreateToken(P4_String, size_t, size_t, P4_Expression*);

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
P4_PUBLIC void           P4_DeleteToken(P4_Token*);

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
P4_PUBLIC P4_Slice*      P4_GetTokenSlice(P4_Token*);

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
P4_PUBLIC P4_String      P4_CopyTokenString(P4_Token*);

#ifdef __cplusplus
}
#endif

# endif
