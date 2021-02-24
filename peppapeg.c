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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "peppapeg.h"

# define                        IS_TIGHT(e) (((e)->flag & P4_FLAG_TIGHT) != 0)
# define                        IS_SCOPED(e) (((e)->flag & P4_FLAG_SCOPED) != 0)
# define                        IS_SPACED(e) (((e)->flag & P4_FLAG_SPACED) != 0)
# define                        IS_SQUASHED(e) (((e)->flag & P4_FLAG_SQUASHED) != 0)
# define                        IS_LIFTED(e) (((e)->flag & P4_FLAG_LIFTED) != 0)
# define                        IS_RULE(e) ((e)->id != 0)
# define                        NEED_SILENT(s) ((s)->frame_stack ? (s)->frame_stack->silent : false)
# define                        NEED_SPACE(s) ((s)->frame_stack ? (s)->frame_stack->space : false)
# define                        NO_ERROR(s) ((s)->err == P4_Ok)
# define                        NO_MATCH(s) ((s)->err == P4_MatchError)

# define                        autofree __attribute__ ((cleanup (cleanup_freep)))

P4_PRIVATE(void)
cleanup_freep (void *p)
{
  void **pp = (void **) p;
  if (*pp)
    free (*pp);
}

# define                        P4_AdoptToken(head, tail, list) do { \
                                    if ((list) != NULL) {\
                                        if ((head) == NULL) (head) = (list); \
                                        if ((tail) == NULL) (tail) = (list); \
                                        else (tail)->next = (list); \
                                        if ((tail) != NULL) {\
                                            while ((tail)->next != NULL) \
                                                (tail) = (tail)->next; \
                                        } \
                                    } \
                                } while (0)

# define                        P4_AddSomeGrammarRule(g, id, rule) do { \
                                    P4_Error err = P4_Ok;       \
                                    P4_Expression* expr = NULL; \
                                                                \
                                    if (grammar == NULL || id == 0) { \
                                        err = P4_NullError;     \
                                        goto end;               \
                                    }                           \
                                    if ((expr = (rule)) == NULL) { \
                                        err = P4_MemoryError;   \
                                        goto end;               \
                                    }                           \
                                                                \
                                    if ((err=P4_AddGrammarRule(grammar, id, expr))!=P4_Ok) {\
                                        goto end;               \
                                    }                           \
                                                                \
                                    break;                      \
                                    end:                        \
                                    if (expr != NULL) {         \
                                        P4_DeleteExpression(expr); \
                                    }                           \
                                    return err;                 \
} while (0)

P4_PRIVATE(size_t)       P4_ReadRune(P4_String s, P4_Rune* c);
P4_PRIVATE(int)          P4_CaseCmpInsensitive(P4_String, P4_String, size_t);

P4_PRIVATE(P4_Position)  P4_GetPosition(P4_Source*);
P4_PRIVATE(void)                P4_SetPosition(P4_Source*, P4_Position);

# define                        P4_MarkPosition(s, p) P4_Position (p) = P4_GetPosition(s);

P4_PRIVATE(P4_String)    P4_RemainingText(P4_Source*);

P4_PRIVATE(bool)         P4_NeedLift(P4_Source*, P4_Expression*);

P4_PRIVATE(void)         P4_RaiseError(P4_Source*, P4_Error, P4_String);
P4_PRIVATE(void)         P4_RescueError(P4_Source*);

P4_PRIVATE(P4_Error)            P4_PushFrame(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Error)            P4_PopFrame(P4_Source*, P4_Frame*);

P4_PRIVATE(P4_Expression*)      P4_GetReference(P4_Source*, P4_Expression*);

P4_PRIVATE(P4_String)           P4_CopySliceString(P4_String, P4_Slice*);

P4_PRIVATE(P4_Error)            P4_SetWhitespaces(P4_Grammar*);
P4_PRIVATE(P4_Expression*)      P4_GetWhitespaces(P4_Grammar*);

P4_PRIVATE(P4_Error)            P4_RefreshReference(P4_Expression*, P4_RuleID);

P4_PRIVATE(P4_Token*)           P4_Match(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchLiteral(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRange(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchReference(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchPositive(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchNegative(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchSequence(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchChoice(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeat(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchSpacedExpressions(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchBackReference(P4_Source*, P4_Expression*, P4_Slice*, size_t);

/**
 *
 * @brief   Reads a single UTF-8 code point from the string.
 * @param   s   The string in UTF-8 encoding.
 * @param   c   The pointer to the rune to read.
 * @return  The size of rune, could be 1,2,3,4. When error occurs, read nothing and returning size is 0.
 *
 * Example::
 *
 *     > uint32_t c = 0x0
 *     > P4_ReadRune("你好", &c)
 *     3
 *     > printf("%p %d\n", c, c)
 *     0x4f60 20320
 */
P4_PRIVATE(size_t)
P4_ReadRune(P4_String s, P4_Rune* c) {
    *c = 0;

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
P4_PRIVATE(int)
P4_CaseCmpInsensitive(P4_String src, P4_String dst, size_t len) {
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


/*
 * Determine if the corresponding token to `e` should be ignored.
 *
 * 1. Intermediate expr.
 * 2. Bareness expr.
 * 3. Hollowed expr.
 *
 */
P4_PRIVATE(bool)
P4_NeedLift(P4_Source* s, P4_Expression* e) {
    return !IS_RULE(e) || IS_LIFTED(e) || NEED_SILENT(s);
}


/*
 * Raise an error.
 *
 * Set err and errmsg to state.
 */
P4_PRIVATE(void)
P4_RaiseError(P4_Source* s, P4_Error err, P4_String errmsg) {
    s->err = err;

    if (s->errmsg != NULL)
        free(s->errmsg);

    s->errmsg = strdup(errmsg);
}


/*
 * Clear an error.
 *
 * It allows the parser to keep parsing the text.
 */
P4_PRIVATE(void)
P4_RescueError(P4_Source* s) {
    s->err = P4_Ok;
    if (s->errmsg != NULL) {
        free(s->errmsg);
        s->errmsg = NULL;
        s->errmsg = strdup("");
    }
}


/*
 * Initialize a token.
 */
P4_Token*
P4_CreateToken (const P4_String     str,
                size_t              start,
                size_t              stop,
                P4_Expression*      expr) {
    P4_Token* token;

    if ((token=malloc(sizeof(P4_Token))) == NULL)
        return NULL;

    token->text         = str;
    token->slice.i      = start;
    token->slice.j      = stop;
    token->expr         = expr;
    token->next         = NULL;
    token->head         = NULL;
    token->tail         = NULL;

    return token;
}


/*
 * Free the token.
 *
 * Danger: if free the token only without cleaning next & head & tail,
 * they're in risk of being dangled.
 */
P4_PRIVATE(void)
P4_DeleteTokenNode(P4_Token* token) {
    assert(token != NULL);
    if (token)
        free(token);
}


/*
 * Free all of the children tokens of the token.
 */
P4_PRIVATE(void)
P4_DeleteTokenChildren(P4_Token* token) {
    assert(token != NULL);
    P4_Token*   child   = token->head;
    P4_Token*   tmp     = NULL;

    while (child) {
        tmp = child->next;
        if (child->head)
            P4_DeleteTokenChildren(child);
        P4_DeleteTokenNode(child);
        child = tmp;
    }
}


/*
 * Free the token list and all the children nodes of each
 * node in the token list.
 */
P4_PUBLIC void
P4_DeleteToken(P4_Token* token) {
    P4_Token* tmp = NULL;
    while (token) {
        tmp     = token->next;
        P4_DeleteTokenChildren(token);
        P4_DeleteTokenNode(token);
        token   = tmp;
    }
}

/*
 * Push e into s->frame_stack.
 */
P4_PRIVATE(P4_Error)
P4_PushFrame(P4_Source* s, P4_Expression* e) {
    if ((s->frame_stack_size) >= (s->grammar->depth)) {
        return P4_StackError;
    }

    P4_Frame* frame = malloc(sizeof(P4_Frame));

    if (frame == NULL) {
        return P4_MemoryError;
    }

    P4_Frame* top = s->frame_stack;

    /* Set silent */
    frame->silent = false;

    if (!IS_SCOPED(e)) {
        if (
            (top && IS_SQUASHED(top->expr))
            || (top && top->silent)
        )
            frame->silent = true;
    }

    /* Set space */
    frame->space = false;

    if (P4_GetWhitespaces(s->grammar) != NULL
            && !s->whitespacing) {
        if (IS_SCOPED(e)) {
            frame->space = true;
        } else if (top) {
            if (!IS_TIGHT(e)) {
                frame->space = top->space;
            }
        } else if (!IS_TIGHT(e)) {
            frame->space = true;
        }
    }

    /* Set expr & next */
    frame->expr = e;
    frame->next = top;

    /* Push stack */
    s->frame_stack_size++;
    s->frame_stack = frame;

    return P4_Ok;
}


/*
 * Pop top from s->frames.
 */
P4_PRIVATE(P4_Error)
P4_PopFrame(P4_Source* s, P4_Frame* f) {
    if (s->frame_stack == NULL)
        return P4_MemoryError;

    P4_Frame* oldtop = s->frame_stack;
    s->frame_stack = s->frame_stack->next;
    if (oldtop) free(oldtop);
    s->frame_stack_size--;

    return P4_Ok;
}


P4_PRIVATE(P4_Token*)
P4_MatchLiteral(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s));

    P4_String str = P4_RemainingText(s);

# define EOT(s) (*(s) == 0x0)

    if (EOT(str)) {
        P4_RaiseError(s, P4_MatchError, "eof");
        return NULL;
    }

    size_t len = strlen(e->literal);

    P4_MarkPosition(s, startpos);
    if ((!e->sensitive && P4_CaseCmpInsensitive(e->literal, str, len) != 0)
            || (e->sensitive && memcmp(e->literal, str, len) != 0)) {
        P4_RaiseError(s, P4_MatchError, "expect literal");
        return NULL;
    }
    P4_SetPosition(s, startpos+len);
    P4_MarkPosition(s, endpos);

    if (P4_NeedLift(s, e))
        return NULL;

    P4_Token* result = NULL;
    if ((result=P4_CreateToken (s->content, startpos, endpos, e)) == NULL) {
        P4_RaiseError(s, P4_MemoryError, "");
        return NULL;
    }

    return result;
}

P4_PRIVATE(P4_Token*)
P4_MatchRange(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s));

    P4_String str = P4_RemainingText(s);
    if (*str == '\0') {
        P4_RaiseError(s, P4_MatchError, "eof");
        return NULL;
    }

    P4_MarkPosition(s, startpos);

    uint32_t rune = 0x0;
    size_t size = P4_ReadRune(str, &rune);

#define IN_RANGE(e, c) ((c)>=(e)->range[0] && (c)<=(e)->range[1])

    if (!IN_RANGE(e, rune)) {
        P4_RaiseError(s, P4_MatchError, "not in range");
        return NULL;
    }

    P4_SetPosition(s, startpos+size);
    P4_MarkPosition(s, endpos);

    if (P4_NeedLift(s, e))
        return NULL;

    P4_Token* result = NULL;
    if ((result=P4_CreateToken (s->content, startpos, endpos, e)) == NULL) {
        P4_RaiseError(s, P4_MemoryError, "oom");
        return NULL;
    }

    return result;
}

P4_PRIVATE(P4_Expression*)
P4_GetReference(P4_Source* s, P4_Expression* e) {
    if (e->ref_expr != NULL)
        return e->ref_expr;

    if (e->ref_id != 0) {
        e->ref_expr = P4_GetGrammarRule(s->grammar, e->ref_id);
    }

    return e->ref_expr;
}

P4_PRIVATE(P4_Token*)
P4_MatchReference(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s));

    if (e->ref_expr == NULL && e->ref_id != 0) {
        e->ref_expr = P4_GetGrammarRule(s->grammar, e->ref_id);
    }

    if (e->ref_expr == NULL) {
        P4_RaiseError(s, P4_NameError, "");
        return NULL;
    }

    P4_MarkPosition(s, startpos);
    P4_Token* reftok = P4_Match(s, e->ref_expr);
    P4_MarkPosition(s, endpos);

    // Ref matching is terminated when error occurred.
    if (!NO_ERROR(s))
        return NULL;

    // The referenced token is returned when silenced.
    if (P4_NeedLift(s, e))
        return reftok;

    // A single reference expr can be a rule: `e = { ref }`
    // In such a case, a token for `e` with single child `ref` is created.
    //
    P4_Token* result = NULL;

    if ((result=P4_CreateToken (s->content, startpos, endpos, e)) == NULL) {
        P4_RaiseError(s, P4_MemoryError, "oom");
        return NULL;
    }

    P4_AdoptToken(result->head, result->tail, reftok);
    return result;
}

P4_PRIVATE(P4_Token*)
P4_MatchSequence(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s));

    P4_Expression *member = NULL;
    P4_Token *head = NULL,
             *tail = NULL,
             *tok = NULL,
             *whitespace = NULL;

    autofree P4_Slice* backrefs = malloc(sizeof(P4_Slice) * e->count);
    if (backrefs == NULL) {
        P4_RaiseError(s, P4_MemoryError, "OOM");
        return NULL;
    }

    bool need_space = NEED_SPACE(s);

    P4_MarkPosition(s, startpos);

    for (size_t i = 0; i < e->count; i++) {
        member = e->members[i];

        // Optional `WHITESPACE` and `COMMENT` are inserted between every member.
        if (need_space && i > 0) {
            whitespace = P4_MatchSpacedExpressions(s, NULL);
            if (!NO_ERROR(s)) goto finalize;
            P4_AdoptToken(head, tail, whitespace);
        }

        P4_MarkPosition(s, member_startpos);

        if (member->kind == P4_BackReference) {
            tok = P4_MatchBackReference(s, e, backrefs, member->backref_index);
            if (!NO_ERROR(s)) goto finalize;
        } else {
            tok = P4_Match(s, member);
        }

        // If any of the sequence members fails, the entire sequence fails.
        // Puke the eaten text and free all created tokens.
        if (!NO_ERROR(s)) {
            goto finalize;
        }

        P4_AdoptToken(head, tail, tok);
        backrefs[i].i = member_startpos;
        backrefs[i].j = P4_GetPosition(s);
    }

    if (P4_NeedLift(s, e))
        return head;

    P4_Token* ret = P4_CreateToken (s->content, startpos, P4_GetPosition(s), e);
    if (ret == NULL) {
        P4_RaiseError(s, P4_MemoryError, "oom");
        return NULL;
    }

    ret->head = head;
    ret->tail = tail;
    return ret;

finalize:
    P4_SetPosition(s, startpos);
    P4_DeleteToken(head);
    return NULL;
}

P4_PRIVATE(P4_Token*)
P4_MatchChoice(P4_Source* s, P4_Expression* e) {
    P4_Token* tok = NULL;
    P4_Expression* member = NULL;

    // A member is attempted if previous yields no match.
    // The oneof match matches successfully immediately if any match passes.
    P4_MarkPosition(s, startpos);
    size_t i;
    for (i = 0; i < e->count; i++) {
        member = e->members[i];
        tok = P4_Match(s, member);
        if (NO_ERROR(s)) break;
        if (NO_MATCH(s)) {
            // retry until the last one.
            if (i < e->count-1) {
                P4_RescueError(s);
                P4_SetPosition(s, startpos);
            // fail when the last one is a no-match.
            } else {
                P4_RaiseError(s, P4_MatchError, "no match");
                goto finalize;
            }
        }
    }
    P4_MarkPosition(s, endpos);

    if (P4_NeedLift(s, e))
        return tok;

    P4_Token* oneof = P4_CreateToken (s->content, startpos, endpos, e);
    if (oneof == NULL) {
        P4_RaiseError(s, P4_MemoryError, "oom");
        goto finalize;
    }

    P4_AdoptToken(oneof->head, oneof->tail, tok);
    return oneof;

finalize:
    P4_SetPosition(s, startpos);
    free(tok);
    return NULL;
}

/*
 * Repetition matcher function.
 *
 * Returns a token in a greedy fashion.
 *
 * There are seven repetition mode: zeroormore, zerooronce,
 */
P4_PRIVATE(P4_Token*)
P4_MatchRepeat(P4_Source* s, P4_Expression* e) {
    size_t min = SIZE_MAX, max = SIZE_MAX, repeated = 0;

    assert(e->repeat_min != min || e->repeat_max != max); // need at least one of min/max.
    assert(e->repeat_expr != NULL); // need repeat expression.
    assert(NO_ERROR(s));

# define IS_REF(e) ((e)->kind == P4_Reference)
# define IS_PROGRESSING(k) ((k)==P4_Positive \
                            || (k)==P4_Negative)

    // when expression inside repetition is non-progressing, it repeats indefinitely.
    // we know negative/positive definitely not progressing,
    // and so does a reference to a negative/positive rule.
    // Question: we may never list all the cases in this way. How to deal with it better?
    if (IS_PROGRESSING(e->repeat_expr->kind) ||
            (IS_REF(e->repeat_expr)
             && IS_PROGRESSING(P4_GetReference(s, e->repeat_expr)->kind))) {
        P4_RaiseError(s, P4_AdvanceError, "no progressing in repetition");
        return NULL;
    }

    min = e->repeat_min;
    max = e->repeat_max;

    bool need_space = NEED_SPACE(s);
    P4_Position startpos = P4_GetPosition(s);
    P4_Token *head = NULL, *tail = NULL, *tok = NULL, *whitespace = NULL;

    while (*P4_RemainingText(s) != '\0') {
        P4_MarkPosition(s, before_implicit);

        // SPACED rule expressions are inserted between every repetition.
        if (need_space && repeated > 0 ) {
            whitespace = P4_MatchSpacedExpressions(s, NULL);
            if (!NO_ERROR(s)) goto finalize;
            P4_AdoptToken(head, tail, whitespace);
        }

        tok = P4_Match(s, e->repeat_expr);

        if (NO_MATCH(s)) {
            assert(tok == NULL);

            // considering the case: MATCH WHITESPACE MATCH WHITESPACE NO_MATCH
            if (need_space && repeated > 0)//               ^          ^ we are here
                P4_SetPosition(s, before_implicit);  //           ^ puke extra whitespace
                                               //           ^ now we are here

            if (min != SIZE_MAX && repeated < min) {
                P4_RaiseError(s, P4_MatchError, "insufficient repetitions");
                goto finalize;
            } else {                       // sufficient repetitions.
                P4_RescueError(s);
                break;
            }
        }

        if (!NO_ERROR(s))
            goto finalize;

        if (P4_GetPosition(s) == before_implicit) {
            P4_RaiseError(s, P4_AdvanceError, "Repeated expression consumes no input");
            goto finalize;
        }

        repeated++;
        P4_AdoptToken(head, tail, tok);

        if (max != SIZE_MAX && repeated == max) { // enough attempts
            P4_RescueError(s);
            break;
        }

    }

    // there should be no error when repetition is successful.
    assert(NO_ERROR(s));

    // fails when attempts are excessive, e.g. repeated > max.
    if (max != SIZE_MAX && repeated > max) {
        P4_RaiseError(s, P4_MatchError, "excessive repetitions");
        goto finalize;
    }

    if (min != SIZE_MAX && repeated < min) {
        P4_RaiseError(s, P4_MatchError, "insufficient repetitions");
        goto finalize;
    }

    if (P4_GetPosition(s) == startpos) // success but no token is produced.
        goto finalize;


    if (P4_NeedLift(s, e))
        return head;

    P4_Token* repetition = P4_CreateToken (s->content, startpos, P4_GetPosition(s), e);
    if (repetition == NULL) {
        P4_RaiseError(s, P4_MemoryError, "oom");
        return NULL;
    }

    P4_AdoptToken(repetition->head, repetition->tail, head);
    return repetition;

// cleanup before returning NULL.
// tokens between head..tail should be freed.
finalize:
    P4_SetPosition(s, startpos);
    P4_DeleteToken(head);
    return NULL;
}

P4_PRIVATE(P4_Token*)
P4_MatchPositive(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s) && e->ref_expr != NULL);

    P4_MarkPosition(s, startpos);

    P4_Token* token = P4_Match(s, e->ref_expr);
    if (token != NULL)
        P4_DeleteToken(token);

    P4_SetPosition(s, startpos);

    return NULL;
}

P4_PRIVATE(P4_Token*)
P4_MatchNegative(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s) && e->ref_expr != NULL);

    P4_MarkPosition(s, startpos);
    P4_Token* token = P4_Match(s, e->ref_expr);
    P4_SetPosition(s, startpos);

    if (NO_ERROR(s)) {
        P4_DeleteToken(token);
        P4_RaiseError(s, P4_MatchError, "should not appear");
    } else if (s->err == P4_MatchError) {
        P4_RescueError(s);
    }

    return NULL;
}

P4_Token*
P4_Expression_dispatch(P4_Source* s, P4_Expression* e) {
    P4_Token* result = NULL;

    switch (e->kind) {
        case P4_Literal:
            result = P4_MatchLiteral(s, e);
            break;
        case P4_Range:
            result = P4_MatchRange(s, e);
            break;
        case P4_Reference:
            result = P4_MatchReference(s, e);
            break;
        case P4_Sequence:
            result = P4_MatchSequence(s, e);
            break;
        case P4_Choice:
            result = P4_MatchChoice(s, e);
            break;
        case P4_Positive:
            result = P4_MatchPositive(s, e);
            break;
        case P4_Negative:
            result = P4_MatchNegative(s, e);
            break;
        case P4_Repeat:
            result = P4_MatchRepeat(s, e);
            break;
        case P4_BackReference:
            P4_RaiseError(s, P4_ValueError, "BackReference only works in Sequence.");
            result = NULL;
            break;
        default:
            P4_RaiseError(s, P4_ValueError, "no such kind");
            result = NULL;
            break;
    }

    return result;
}

/*
 * The match function updates the state given an expression.
 *
 * It returns a token linked list, NULL if no token is generated.
 * State pos will advance if needed.
 * Not-advancing pos / NULL returning token list do not indicate a failed match.
 * It fails when state err/errmsg are set.
 * It propagate the failed match up to the top level.
 */
P4_Token*
P4_Match(P4_Source* s, P4_Expression* e) {
    assert(e != NULL);

    if (s->err != P4_Ok) {
        return NULL;
    }

    P4_Error     err = P4_Ok;
    P4_Token* result = NULL;

    if (IS_RULE(e) && (err = P4_PushFrame(s, e)) != P4_Ok) {
        P4_RaiseError(s, err, "failed to push frame");
        return NULL;
    }

    result = P4_Expression_dispatch(s, e);

    if (IS_RULE(e) && (err = P4_PopFrame(s, NULL)) != P4_Ok) {
        P4_RaiseError(s, err, "failed to pop frame");
        P4_DeleteToken(result);
        return NULL;
    }

    if (s->err != P4_Ok) {
        P4_DeleteToken(result);
        return NULL;
    }

    if (s->grammar->callback && (err = (s->grammar->callback)(s->grammar, e, result)) != P4_Ok) {
        P4_RaiseError(s, err, "failed to run callback.");
        P4_DeleteToken(result);
        return NULL;
    }

    return result;
}

P4_Token*
P4_Match_any(P4_Source* s, P4_Expression* e) {
    P4_String str = P4_RemainingText(s);

# define UTF8_CHARLEN(b) (( 0xe5000000 >> (( (b) >> 3 ) & 0x1e )) & 3 ) + 1

    P4_MarkPosition(s, startpos);
    size_t   len = UTF8_CHARLEN(*str);
    P4_SetPosition(s, startpos+len);

    return NULL;
}

P4_PRIVATE(P4_Token*)
P4_MatchSpacedExpressions(P4_Source* s, P4_Expression* e) {
    // implicit whitespace is guaranteed to be an unnamed rule.
    // state flag is guaranteed to be none.
    assert(NO_ERROR(s));

    if (s->grammar == NULL)
        return NULL;

    P4_Expression* implicit_whitespace = P4_GetWhitespaces(s->grammar);
    assert(implicit_whitespace != NULL);

    // (1) Temporarily set implicit whitespace to empty.
    s->whitespacing = true;

    // (2) Perform implicit whitespace checks.
    //     We won't do implicit whitespace inside an implicit whitespace expr.
    P4_Token* result = P4_Match(s, implicit_whitespace);
    if (NO_MATCH(s))
        P4_RescueError(s);

    // (3) Set implicit whitespace back.
    s->whitespacing = false;

    return result;
}

P4_PRIVATE(P4_Token*)
P4_MatchBackReference(P4_Source* s, P4_Expression* e, P4_Slice* backrefs, size_t index) {
    if (backrefs == NULL) {
        P4_RaiseError(s, P4_NullError, "");
        return NULL;
    }

    if (index > e->count) {
        P4_RaiseError(s, P4_IndexError, "BackReference Index OutOfBound");
        return NULL;
    }

    P4_Slice* slice = &(backrefs[index]);
    if (slice == NULL) {
        P4_RaiseError(s, P4_IndexError, "BackReference Index OutOfBound");
        return NULL;
    }

    autofree P4_String litstr = P4_CopySliceString(s->content, slice);

    if (litstr == NULL) {
        P4_RaiseError(s, P4_MemoryError, "OOM");
        return NULL;
    }

    P4_Expression* backref_expr = e->members[index];

    if (backref_expr == NULL) {
        P4_RaiseError(s, P4_NullError, "Member NULL");
        return NULL;
    }

    P4_Expression* litexpr = P4_CreateLiteral(litstr, true);

    if (litexpr == NULL) {
        P4_RaiseError(s, P4_MemoryError, "OOM");
        return NULL;
    }

    if (backref_expr->kind == P4_Reference)
        litexpr->id = backref_expr->ref_expr->id;
    else
        litexpr->id = backref_expr->id;

    P4_Token* tok = P4_MatchLiteral(s, litexpr);

    if (tok != NULL) {
        if (backref_expr->kind == P4_Reference) { /* TODO: other cases? */
            tok->expr = backref_expr->ref_expr;
        } else {
            tok->expr = backref_expr;
        }
    }

    P4_DeleteExpression(litexpr);

    return tok;
}

// Poor performance, refactor me.
P4_PUBLIC void
P4_PrintSourceAst(P4_Token* token, P4_String buf, size_t indent) {
    P4_Token* current = token;
    char idstr[6];
    idstr[0] = 'R';
    while (current != NULL) {
        for (size_t i = 0; i < indent; i++) strcat(buf, " ");
        strcat(buf, "- ");
        sprintf(idstr+1, "%llu", current->expr->id);
        strcat(buf, idstr);
        if (current->head == NULL) {
            strcat(buf, ": \"");
            size_t substr_len = (current->slice.j - current->slice.i);
            autofree P4_String substr = malloc( sizeof(char) * (substr_len+1) );
            memcpy(substr, current->text+current->slice.i, substr_len);
            substr[substr_len] = '\0';
            strcat(buf, substr);
            strcat(buf, "\"\n");
        } else {
            strcat(buf, "\n");
            P4_PrintSourceAst(current->head, buf, indent+2);
        }
        current = current->next;
    }
}

/*
 * Get version string.
 */
P4_PUBLIC P4_String
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

/*
P4_PUBLIC P4_Expression*
P4_CreateNumeric(size_t num) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Numeric;
    expr->num = num;
    return expr;
}
*/

P4_PUBLIC P4_Expression*
P4_CreateLiteral(const P4_String literal, bool sensitive) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_Literal;
    expr->flag = 0;
    expr->literal = strdup(literal);
    expr->sensitive = sensitive;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateRange(P4_Rune lower, P4_Rune upper) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_Range;
    expr->flag = 0;
    expr->range[0] = lower;
    expr->range[1] = upper;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateReference(P4_RuleID id) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_Reference;
    expr->flag = 0;
    expr->ref_id = id;
    expr->ref_expr = NULL;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreatePositive(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_Positive;
    expr->flag = 0;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateNegative(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_Negative;
    expr->flag = 0;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PRIVATE(P4_Expression*)
P4_CreateContainer(size_t count) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->flag = 0;
    expr->count = count;
    expr->members = malloc(sizeof(P4_Expression*) * count);
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateSequence(size_t count) {
    P4_Expression* expr = P4_CreateContainer(count);

    if (expr == NULL)
        return NULL;

    expr->kind = P4_Sequence;

    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateChoice(size_t count) {
    P4_Expression* expr = P4_CreateContainer(count);

    if (expr == NULL)
        return NULL;

    expr->kind = P4_Choice;

    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateSequenceWithMembers(size_t count, ...) {
    P4_Expression* expr = P4_CreateSequence(count);

    if (expr == NULL)
        return NULL;

    va_list members;
    va_start (members, count);

    for (size_t i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return expr;

finalize:
    P4_DeleteExpression(expr);
    return NULL;
}

P4_PUBLIC P4_Expression*
P4_CreateChoiceWithMembers(size_t count, ...) {
    P4_Expression* expr = P4_CreateChoice(count);

    if (expr == NULL)
        return NULL;

    va_list members;
    va_start (members, count);

    for (size_t i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return expr;

finalize:
    P4_DeleteExpression(expr);
    return NULL;
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMinMax(P4_Expression* repeat, size_t min, size_t max) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->flag = 0;
    expr->kind = P4_Repeat;
    expr->repeat_expr = repeat;
    expr->repeat_min = min;
    expr->repeat_max = max;
    return expr;
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMin(P4_Expression* repeat, size_t min) {
    return P4_CreateRepeatMinMax(repeat, min, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatMax(P4_Expression* repeat, size_t max) {
    return P4_CreateRepeatMinMax(repeat, SIZE_MAX, max);
}

P4_PUBLIC P4_Expression*
P4_CreateRepeatExact(P4_Expression* repeat, size_t minmax) {
    return P4_CreateRepeatMinMax(repeat, minmax, minmax);
}

P4_PUBLIC P4_Expression*
P4_CreateZeroOrOnce(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, 1);
}

P4_PUBLIC P4_Expression*
P4_CreateZeroOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateOnceOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 1, SIZE_MAX);
}

P4_PUBLIC P4_Expression*
P4_CreateBackReference(size_t index) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->id = 0;
    expr->kind = P4_BackReference;
    expr->flag = 0;
    expr->backref_index = index;
    return expr;
}

P4_PUBLIC P4_Error
P4_SetRuleID(P4_Expression* e, P4_RuleID id) {
    if (e == NULL)
        return P4_NullError;

    if (e->id != 0 || id == 0)
        return P4_ValueError;

    e->id = id;
    return P4_Ok;
}

P4_PUBLIC bool
P4_IsRule(P4_Expression* e) {
    if (e == NULL)
        return false;

    return e->id != 0;
}

P4_PUBLIC P4_Grammar*    P4_CreateGrammar(void) {
    P4_Grammar* grammar = malloc(sizeof(P4_Grammar));
    grammar->rules = NULL;
    grammar->count = 0;
    grammar->cap = 0;
    grammar->spaced_count = SIZE_MAX;
    grammar->spaced_rules = NULL;
    grammar->depth = P4_DEFAULT_RECURSION_LIMIT;
    grammar->callback = NULL;
    return grammar;
}

P4_PUBLIC void
P4_DeleteGrammar(P4_Grammar* grammar) {
    if (grammar) {
        for (size_t i = 0; i < grammar->count; i++) {
            if (grammar->rules[i])
                P4_DeleteExpression(grammar->rules[i]);
            grammar->rules[i] = NULL;
        }
        if (grammar->spaced_rules)
            P4_DeleteExpression(grammar->spaced_rules);
        free(grammar->rules);
        free(grammar);
    }
}

P4_PUBLIC P4_Expression*
P4_GetGrammarRule(P4_Grammar* grammar, P4_RuleID id) {
    P4_Expression* rule = NULL;
    for (size_t i = 0; i < grammar->count; i++) {
        rule = grammar->rules[i];
        if (rule && rule->id == id)
            return rule;
    }
    return NULL;
}

P4_PUBLIC P4_Error
P4_SetGrammarRuleFlag(P4_Grammar* grammar, P4_RuleID id, P4_ExpressionFlag flag) {
    P4_Expression* expr = P4_GetGrammarRule(grammar, id);
    if (expr == NULL)
        return P4_NameError;

    P4_SetExpressionFlag(expr, flag);

    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_SetRecursionLimit(P4_Grammar* grammar, size_t limit) {
    if (grammar == NULL)
        return P4_NullError;

    grammar->depth = limit;

    return P4_Ok;
}

P4_PUBLIC size_t
P4_GetRecursionLimit(P4_Grammar* grammar) {
    return grammar == NULL ? 0 : grammar->depth;
}

P4_PUBLIC P4_Error
P4_AddGrammarRule(P4_Grammar* grammar, P4_RuleID id, P4_Expression* expr) {
    size_t          cap   = grammar->cap;
    P4_Expression** rules = grammar->rules;

    if (grammar == NULL || id == 0 || expr == NULL)
        return P4_NullError;

    if (cap == 0) {
        cap = 32;
        rules = malloc(sizeof(P4_Expression*) * cap);
    } else if (grammar->count >= cap) {
        cap <<= 1;
        rules = realloc(rules, sizeof(P4_Expression*) * cap);
    }

    if (rules == NULL)
        return P4_MemoryError;

    expr->id = id;

    grammar->cap = cap;
    grammar->rules = rules;
    grammar->rules[grammar->count++] = expr;

    return P4_Ok;
}

P4_PUBLIC P4_Source*
P4_CreateSource(P4_String content, P4_RuleID rule_id) {
    P4_Source* source = malloc(sizeof(P4_Source));
    source->content = content;
    source->rule_id = rule_id;
    source->pos = 0;
    source->err = P4_Ok;
    source->errmsg = NULL;
    source->root = NULL;
    source->frame_stack = NULL;
    source->frame_stack_size = 0;
    source->whitespacing = false;
    return source;
}

P4_PUBLIC void
P4_DeleteSource(P4_Source* source) {
    if (source == NULL)
        return;

    P4_Frame* tmp = source->frame_stack;
    while(source->frame_stack) {
        tmp = source->frame_stack->next;
        free(source->frame_stack);
        source->frame_stack = tmp;
    }

    if (source->errmsg)
        free(source->errmsg);

    if (source->root)
        P4_DeleteToken(source->root);

    free(source);
}

P4_PUBLIC P4_Token*
P4_GetSourceAst(P4_Source* source) {
    return source == NULL ? NULL : source->root;
}

P4_PUBLIC P4_Position
P4_GetSourcePosition(P4_Source* source) {
    return source == NULL ? 0 : source->pos;
}

P4_PUBLIC P4_Error
P4_Parse(P4_Grammar* grammar, P4_Source* source) {
    if (source->err != P4_Ok)
        return source->err;

    if (source->root != NULL)
        return P4_Ok;

    source->grammar = grammar;

    P4_Expression* expr     = P4_GetGrammarRule(grammar, source->rule_id);
    P4_Token*      tok      = P4_Match(source, expr);

    source->root            = tok;

    return source->err;
}


P4_PUBLIC bool
P4_HasError(P4_Source* src) {
    if (src == NULL)
        return false;

    return src->err != P4_Ok;
}

P4_PUBLIC P4_Error
P4_GetError(P4_Source* source) {
    if (source == NULL)
        return P4_NullError;

    return source->err;
}

P4_PUBLIC P4_String
P4_GetErrorMessage(P4_Source* source) {
    if (source == NULL || source->errmsg == NULL)
        return NULL;

    return strdup(source->errmsg);
}

P4_PRIVATE(P4_Error)
P4_SetWhitespaces(P4_Grammar* grammar) {
    size_t          count = 0;
    P4_RuleID       ids[2] = {0};
    P4_Expression*  rules[2] = {0};
    P4_Expression*  repeat = NULL;
    P4_Expression*  rule = NULL;

    for (size_t i = 0; i < grammar->count; i++) {
        rule = grammar->rules[i];

        if (IS_SPACED(rule)) {
            ids[count] = rule->id;
            rules[count] = P4_CreateReference(rule->id);

            if (rules[count] == NULL)
                goto end;

            rules[count]->ref_expr = rule;

            count++;
        }

        if (count > 1)
            break;
    }

    if (count == 0) {
        return P4_Ok;

    } else if (count == 1) {
        repeat = rules[0];

    } else if (count == 2) {
        repeat = P4_CreateChoice(2);
        if (repeat == NULL)
            goto end;
        if (P4_SetMember(repeat, 0, rules[0]) != P4_Ok)
            goto end;
        if (P4_SetMember(repeat, 1, rules[1]) != P4_Ok)
            goto end;
    }

    if ((grammar->spaced_rules = P4_CreateZeroOrMore(repeat))== NULL)
        goto end;

    grammar->spaced_count = count;

    return P4_Ok;

end:
    if (rules[0] != NULL)
        P4_DeleteExpression(rules[0]);

    if (rules[1] != NULL)
        P4_DeleteExpression(rules[1]);

    if (grammar->spaced_rules != NULL) {
        P4_DeleteExpression(grammar->spaced_rules);
        grammar->spaced_rules = NULL;
    }

    grammar->spaced_count = SIZE_MAX;

    return P4_MemoryError;
}

P4_PRIVATE(P4_Expression*)
P4_GetWhitespaces(P4_Grammar* g) {
    if (g == NULL)
        return NULL;

    if (g->spaced_count == SIZE_MAX)
        P4_SetWhitespaces(g);

    return g->spaced_rules;
}

P4_PUBLIC void
P4_SetExpressionFlag(P4_Expression* e, P4_ExpressionFlag f) {
    assert(e != NULL);
    e->flag |= f;
}

P4_PRIVATE(P4_Position)
P4_GetPosition(P4_Source* s) {
    return s->pos;
}

P4_PRIVATE(void)
P4_SetPosition(P4_Source* s, P4_Position pos) {
    s->pos = pos;
}

/*
 * Get the remaining text.
 */
P4_PRIVATE(P4_String)
P4_RemainingText(P4_Source* s) {
    return s->content + s->pos;
}

P4_PUBLIC P4_Error
P4_AddLiteral(P4_Grammar* grammar, P4_RuleID id, const P4_String literal, bool sensitive) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateLiteral(literal, sensitive));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRange(P4_Grammar* grammar, P4_RuleID id, P4_Rune lower, P4_Rune upper) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateRange(lower, upper));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddPositive(P4_Grammar* grammar, P4_RuleID id, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, id, P4_CreatePositive(ref_expr));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddNegative(P4_Grammar* grammar, P4_RuleID id, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, id, P4_CreateNegative(ref_expr));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddSequence(P4_Grammar* grammar, P4_RuleID id, size_t size) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, id)->kind = P4_Sequence;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddSequenceWithMembers(P4_Grammar* grammar, P4_RuleID id, size_t count, ...) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateSequence(count));
    P4_Expression* expr = P4_GetGrammarRule(grammar, id);

    va_list members;
    va_start (members, count);

    for (size_t i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return P4_Ok;

finalize:
    P4_DeleteExpression(expr);

    return P4_MemoryError;
}

P4_PUBLIC P4_Error
P4_AddChoice(P4_Grammar* grammar, P4_RuleID id, size_t size) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, id)->kind = P4_Choice;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddChoiceWithMembers(P4_Grammar* grammar, P4_RuleID id, size_t count, ...) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateChoice(count));
    P4_Expression* expr = P4_GetGrammarRule(grammar, id);

    va_list members;
    va_start (members, count);

    for (size_t i = 0; i < count; i++) {
        expr->members[i] = va_arg(members, P4_Expression*);

        if (expr->members[i] == NULL) {
            goto finalize;
        }
    }

    va_end (members);

    return P4_Ok;

finalize:
    P4_DeleteExpression(expr);

    return P4_MemoryError;
}

P4_PUBLIC P4_Error
P4_SetMember(P4_Expression* expr, size_t offset, P4_Expression* member) {
    if (expr == NULL
            || member == NULL
            || expr->members == NULL
            || expr->count == 0) {
        return P4_NullError;
    }

    if (expr->kind != P4_Sequence
            && expr->kind != P4_Choice) {
        return P4_ValueError;
    }

    if (offset < 0
            || offset >= expr->count) {
        return P4_IndexError;
    }

    expr->members[offset] = member;
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_SetReferenceMember(P4_Expression* expr, size_t offset, P4_RuleID ref) {
    P4_Expression* ref_expr = P4_CreateReference(ref);
    if (ref_expr == NULL) {
        return P4_MemoryError;
    }

    P4_Error error = P4_SetMember(expr, offset, ref_expr);
    if (error != P4_Ok) {
        P4_DeleteExpression(ref_expr);
        return error;
    }

    return P4_Ok;
}

P4_PUBLIC size_t
P4_GetMembersCount(P4_Expression* expr) {
    if (expr == NULL) {
        return 0;
    }

    return expr->count;
}


P4_PUBLIC P4_Expression*
P4_GetMember(P4_Expression* expr, size_t offset) {
    if (expr == NULL
            || expr->members == NULL
            || expr->count == 0) {
        return NULL;
    }

    if ((expr->kind != P4_Sequence) && (expr->kind != P4_Choice)) {
        return NULL;
    }

    if (offset < 0
            || offset >= expr->count) {
        return NULL;
    }

    return expr->members[offset];
}

P4_PUBLIC void
P4_DeleteExpression(P4_Expression* expr) {
    if (expr == NULL)
        return;

    switch (expr->kind) {
        case P4_Literal:
            if (expr->literal)
                free(expr->literal);
            break;
        // case P4_Reference is quite special - it is not the owner of ref_expr.
        case P4_Positive:
        case P4_Negative:
            if (expr->ref_expr)
                P4_DeleteExpression(expr->ref_expr);
            break;
        case P4_Sequence:
        case P4_Choice:
            for (size_t i = 0; i < expr->count; i++) {
                if (expr->members[i])
                    P4_DeleteExpression(expr->members[i]);
                expr->members[i] = NULL;
            }
            free(expr->members);
            expr->members = NULL;
            break;
        case P4_Repeat:
            if (expr->repeat_expr)
                P4_DeleteExpression(expr->repeat_expr);
            break;
        default:
            break;
    }

    free(expr);
}

P4_PUBLIC P4_Error
P4_AddReference(P4_Grammar* grammar, P4_RuleID id, P4_RuleID ref) {
    if (ref == 0) {
        return P4_NullError;
    }

    P4_AddSomeGrammarRule(grammar, id, P4_CreateReference(ref));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddZeroOrOnce(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateZeroOrOnce(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddZeroOrMore(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateZeroOrMore(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddOnceOrMore(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateOnceOrMore(repeat));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMin(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat, size_t min) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateRepeatMin(repeat, min));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMax(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat, size_t max) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateRepeatMax(repeat, max));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatMinMax(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat, size_t min, size_t max) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateRepeatMinMax(repeat, min, max));
    return P4_Ok;
}

P4_PUBLIC P4_Error
P4_AddRepeatExact(P4_Grammar* grammar, P4_RuleID id, P4_Expression* repeat, size_t num) {
    if (repeat == NULL)
        return P4_NullError;

    P4_AddSomeGrammarRule(grammar, id, P4_CreateRepeatExact(repeat, num));
    return P4_Ok;
}

P4_PUBLIC P4_Slice*
P4_GetTokenSlice(P4_Token* token) {
    if (token == NULL)
        return NULL;

    return &(token->slice);
}

P4_PRIVATE(P4_String)
P4_CopySliceString(P4_String s, P4_Slice* slice) {
    size_t    len = slice->j - slice->i;
    assert(len >= 0);

    P4_String str = malloc(len+1);
    strncpy(str, s + slice->i, len);
    str[len] = '\0';

    return str;
}

P4_PUBLIC P4_String
P4_CopyTokenString(P4_Token* token) {
    if (token == NULL)
        return NULL;

    return P4_CopySliceString(token->text, &(token->slice));
}

/*
 * Determine if expression has flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC bool
P4_IsSquashed(P4_Expression* e) {
    return (e->flag & P4_FLAG_SQUASHED) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_LIFTED.
 */
P4_PUBLIC bool
P4_IsLifted(P4_Expression* e) {
    return (e->flag & P4_FLAG_LIFTED) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_TIGHT.
 */
P4_PUBLIC bool
P4_IsTight(P4_Expression* e) {
    return (e->flag & P4_FLAG_TIGHT) != 0;
}

/*
 * Determine if expression has flag P4_FLAG_SPACED.
 */
P4_PUBLIC bool
P4_IsSpaced(P4_Expression* e) {
    return (e->flag & P4_FLAG_SPACED) != 0;
}

/*
 * Set expression flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC bool
P4_IsScoped(P4_Expression* e) {
    return (e->flag & P4_FLAG_SCOPED) != 0;
}

/*
 * Set expression flag P4_FLAG_SQUASHED.
 */
P4_PUBLIC void
P4_SetSquashed(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SQUASHED);
}

/*
 * Set expression flag P4_FLAG_LIFTED.
 */
P4_PUBLIC void
P4_SetLifted(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_LIFTED);
}

/*
 * Set expression flag P4_FLAG_TIGHT.
 */
P4_PUBLIC void
P4_SetTight(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_TIGHT);
}

/*
 * Set expression flag P4_FLAG_SPACED.
 */
P4_PUBLIC void
P4_SetSpaced(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SPACED);
}

/*
 * Set expression flag P4_FLAG_SCOPED.
 */
P4_PUBLIC void
P4_SetScoped(P4_Expression* e) {
    return P4_SetExpressionFlag(e, P4_FLAG_SCOPED);
}

P4_PUBLIC P4_Error
P4_SetGrammarCallback(P4_Grammar* grammar, P4_Callback callback) {
    if (grammar == NULL || callback == NULL)
        return P4_NullError;

    grammar->callback = callback;
    return P4_Ok;
}

P4_PRIVATE(P4_Error)
P4_RefreshReference(P4_Expression* expr, P4_RuleID id) {
    if (expr == NULL || id == 0)
        return P4_NullError;

    P4_Error err = P4_Ok;

    switch(expr->kind) {
        case P4_Reference:
            if (expr->ref_id == id)
                expr->ref_expr = NULL;
            break;
        case P4_Positive:
        case P4_Negative:
            err = P4_RefreshReference(expr->ref_expr, id);
            break;
        case P4_Sequence:
        case P4_Choice:
            for (size_t i = 0; i < expr->count; i++) {
                err = P4_RefreshReference(expr->members[i], id);
                if (err != P4_Ok)
                    break;
            }
            break;
        case P4_Repeat:
            err = P4_RefreshReference(expr->repeat_expr, id);
            break;
        default:
            break;
    }

    return err;
}

P4_PUBLIC P4_Error
P4_ReplaceGrammarRule(P4_Grammar* grammar, P4_RuleID id, P4_Expression* expr) {
    if (grammar == NULL || id == 0 || expr == NULL)
        return P4_NullError;

    P4_Expression* oldexpr = P4_GetGrammarRule(grammar, id);
    if (oldexpr == NULL)
        return P4_NameError;

    P4_Error err = P4_Ok;

    for (size_t i = 0; i < grammar->count; i++) {
        if (grammar->rules[i]->id == id) {
            P4_DeleteExpression(oldexpr);

            grammar->rules[i] = expr;
            expr->id = id;

            break;
        }
    }

    for (size_t i = 0; i < grammar->count; i++) {
        if (grammar->rules[i]->id != id) {
            err = P4_RefreshReference(grammar->rules[i], id);

            if (err != P4_Ok)
                return err;
        }
    }

    return P4_Ok;
}
