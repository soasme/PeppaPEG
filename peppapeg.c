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

# define                        EACH_INDEX() (p - pstart)
# define                        EACH(item, array, length) (\
        typeof(*(array)) *p = (array), *pstart = (array), (item) = *p;\
        p < &((array)[length]); \
        p++, (item) = *p\
)

# define                        NO_ERROR(s) ((s)->err == P4_Ok)
# define                        NO_MATCH(s) ((s)->err == P4_MatchError)

#define                         autofree __attribute__ ((cleanup (cleanup_freep)))

P4_PRIVATE(inline void)
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

// TODO: DeleteExpression when failed.
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
                                    size_t size = grammar->count + 1; \
                                    if (P4_AddGrammarRule(grammar, id, expr)!=size) {\
                                        err = P4_MemoryError;   \
                                        goto end;               \
                                    }                           \
                                                                \
                                    break;                      \
                                    end:                        \
                                    if (expr != NULL) {         \
                                    }                           \
                                    return err;                 \
} while (0)

P4_PRIVATE(inline P4_Position)  P4_GetPosition(P4_Source*);
P4_PRIVATE(void)                P4_SetPosition(P4_Source*, P4_Position);

# define                        P4_MarkPosition(s, p) P4_Position (p) = P4_GetPosition(s);

P4_PRIVATE(inline P4_String)    P4_RemainingText(P4_Source*);

P4_PRIVATE(inline bool)         P4_NeedLoosen(P4_Source*, P4_Expression*);
P4_PRIVATE(inline bool)         P4_NeedSquash(P4_Source*, P4_Expression*);
P4_PRIVATE(inline bool)         P4_NeedLift(P4_Source*, P4_Expression*);

P4_PRIVATE(inline void)         P4_RaiseError(P4_Source*, P4_Error, P4_String);
P4_PRIVATE(inline void)         P4_RescueError(P4_Source*);

P4_PRIVATE(P4_Error)            P4_PushFrame(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Error)            P4_PopFrame(P4_Source*, P4_Expression**);

P4_PRIVATE(P4_Expression*)      P4_GetReference(P4_Source*, P4_Expression*);

P4_PRIVATE(P4_Token*)           P4_Match(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchNumeric(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchLiteral(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRange(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchReference(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchPositive(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchNegative(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchSequence(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchChoice(P4_Source*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeat(P4_Source*, P4_Expression*);

/*
 * Determine if e is a tightness expr.
 */
static inline bool
is_tight(P4_Expression* e) {
    return (e->flag & P4_FLAG_TIGHT) != 0;
}


/*
 * Determine if e is a hollowness expr.
 */
static inline bool
is_squashed(P4_Expression* e) {
    return (e->flag & P4_FLAG_SQUASHED) != 0;
}


/*
 * Determine if e is a bareness expr.
 */
static inline bool
is_lifted(P4_Expression* e) {
    return (e->flag & P4_FLAG_LIFTED) != 0;
}


/*
 * Determine if e is a continuance expr.
 */
static inline bool
is_scoped(P4_Expression* e) {
    return (e->flag & P4_FLAG_SCOPED) != 0;
}


/*
 * Determine if e is an intermediate expr,
 * e.g, the expr has no left-hand symbol as its name.
 *
 * For example:
 * a = { "a" }, `"a"` is a literal non-intermediate expr.
 * b = { a ~ "b" }, `"b"` is a literal intermediate expr.
 */
static inline bool
is_intermediate(P4_Expression* e) {
    // TODO: remove e->name condition.
    return e->name == NULL && e->id == 0;
}



/*
 * Determine if the implicit whitespace should be applied.
 */
P4_PRIVATE(inline bool)
P4_NeedLoosen(P4_Source* s, P4_Expression* e) {
    assert(s != NULL && e != NULL && s->frames_len >= 0);

    // Insert no whitespace

    // (1) when there is no implicit whitespace rule.
    if (P4_GetWhitespaces(s->grammar) == NULL)
        return false;

    // (2) when we're already matching whitespace.
    if (s->whitespacing)
        return false;

    // Traverse the frame stack from the highest frame,
    for (int i = s->frames_len-1; i >=0; i--) {
        // (3) when e is inside a continuance expression and no tightness at all.
        if (!is_tight(s->frames[i]) && is_scoped(s->frames[i]))
            return true;
    }

    // traversing the frame stack since the inner most,
    for (int i = 0; i < s->frames_len; i++) {
        // (3) when e is a tighted expression.
        if (is_tight(s->frames[i]))
            return false;
    }

    return true;
}

/*
 * Determine if inner tokens should be generated.
 */
P4_PRIVATE(inline bool)
P4_NeedSquash(P4_Source* s, P4_Expression* e) {
    // A continuance expr forces no hollowness.
    if (is_scoped(e))
        return false;

    // Start from expr's parent.
    for (int i = s->frames_len-2; i>=0; i--) {
        // Any of expr's ancestor being continuance forces no hollowness.
        if (is_scoped(s->frames[i]))
            return false;

        // Otherwise, being the descendant of hollowness token should be hollowed.
        // printf("frame %d: id=%lu, %d\n", i, s->frames[i]->id, s->frames[i]->flag);
        if (is_squashed(s->frames[i]))
            return true;
    }

    return false;
}

/*
 * Determine if the corresponding token to `e` should be ignored.
 *
 * 1. Intermediate expr.
 * 2. Bareness expr.
 * 3. Hollowed expr.
 */
P4_PRIVATE(inline bool)
P4_NeedLift(P4_Source* s, P4_Expression* e) {
    // printf("need lift: id=%lu, %d %d %d\n", e->id, is_intermediate(e),  is_lifted(e) , P4_NeedSquash(s, e));
    return is_intermediate(e) || is_lifted(e) || P4_NeedSquash(s, e);
}


/*
 * Raise an error.
 *
 * Set err and errmsg to state.
 */
P4_PRIVATE(inline void)
P4_RaiseError(P4_Source* s, P4_Error err, P4_String errmsg) {
    s->err = err;
    s->errmsg = strdup(errmsg);
    /*
    size_t len = strlen(errmsg)+10;
    for (int i=s->frames_len-1; i>=0; i--) {
        if (s->frames[i]->name) {
            len += 9;
        } else {
            len += strlen(s->frames[i]->name);
        }
        len += 2;
    }
    s->errmsg = (P4_String) calloc(len, sizeof(char));

    for (int i=s->frames_len-1; i>=0; i--) {
        if (s->frames[i]->name) {
            strcat(s->errmsg, s->frames[i]->name);
        } else {
            strcat(s->errmsg, "<UNKNOWN>");
        }
        strcat(s->errmsg, ": ");
    }

    strcat(s->errmsg, errmsg);
    */
}


/*
 * Clear an error.
 *
 * It allows the parser to keep parsing the text.
 */
P4_PRIVATE(inline void)
P4_RescueError(P4_Source* s) {
    s->err = P4_Ok;
    if (s->errmsg != NULL) {
        free(s->errmsg);
        s->errmsg = strdup("");
    }
}


/*
 * Initialize a token.
 */
P4_Token*
P4_CreateToken (const P4_String     str,
                size_t              slice_i,
                size_t              slice_j,
                P4_Expression*      expr) {
    P4_Token* token;

    if ((token=malloc(sizeof(P4_Token))) == NULL)
        return NULL;

    token->text         = str;
    token->slice.i      = slice_i;
    token->slice.j      = slice_j;
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
P4_PUBLIC(void)
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
 * Push e into s->frames.
 */
P4_PRIVATE(P4_Error)
P4_PushFrame(P4_Source* s, P4_Expression* e) {

# define DEFAULT_CAP 32
    if (s->frames_cap == 0) {
        s->frames_cap = 32;
        s->frames = malloc(sizeof(P4_Expression*) * s->frames_cap);
    } else if (s->frames_len >= s->frames_cap) {
        s->frames_cap <<= 1;
        s->frames = realloc(s->frames, s->frames_cap); // memory leak?
    }
    if (s->frames == NULL)
        return P4_MemoryError;

    s->frames[s->frames_len] = e;
    s->frames_len++;
    return P4_Ok;
}


/*
 * Pop e from s->frames.
 */
P4_PRIVATE(P4_Error)
P4_PopFrame(P4_Source* s, P4_Expression** e) {
    if (s->frames_cap == 0 || s->frames_len == 0)
        return P4_MemoryError;

    assert(s->frames != NULL);

    s->frames_len--;

    if (e != NULL)
        *e = s->frames[s->frames_len];

    s->frames[s->frames_len+1] = 0;
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
        P4_RaiseError(s, P4_MatchError, "literal not match");
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
    P4_Token* reftok = P4_Expression_match(s, e->ref_expr);
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

    P4_Token *head = NULL,
             *tail = NULL,
             *tok = NULL,
             *whitespace = NULL;

    P4_MarkPosition(s, startpos);

    for EACH(member, e->members, e->count) {
        // Optional `WHITESPACE` and `COMMENT` are inserted between every member.
        if (P4_NeedLoosen(s, e)
                && EACH_INDEX() > 0) {
            whitespace = P4_Expression_match_implicit_whitespace(s, NULL);
            if (!NO_ERROR(s)) goto finalize;
            P4_AdoptToken(head, tail, whitespace);
        }

        tok = P4_Expression_match(s, member);

        // If any of the sequence members fails, the entire sequence fails.
        // Puke the eaten text and free all created tokens.
        if (!NO_ERROR(s)) {
            goto finalize;
        }

        P4_AdoptToken(head, tail, tok);
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

    // A member is attempted if previous yields no match.
    // The oneof match matches successfully immediately if any match passes.
    P4_MarkPosition(s, startpos);
    for EACH(member, e->members, e->count) {
        tok = P4_Expression_match(s, member);
        if (NO_ERROR(s)) break;
        if (NO_MATCH(s)) {
            // retry until the last one.
            if (EACH_INDEX() < e->count-1) {
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
    size_t min = -1, max = -1, repeated = 0;

    assert(e->repeat_min != min || e->repeat_max != max); // need at least one of min/max.
    assert(e->repeat_expr != NULL); // need repeat expression.
    assert(NO_ERROR(s));

# define IS_REF(e) ((e)->kind == P4_Reference)
# define IS_PROGRESSING(k) ((k)==P4_Positive \
                            || (k)==P4_Negative)

    // when expression inside repetition is non-progressing, it repeats indefinitely.
    // let's prevent such a case.
    if (IS_PROGRESSING(e->repeat_expr->kind) ||
            (IS_REF(e->repeat_expr)
             && IS_PROGRESSING(P4_GetReference(s, e->repeat_expr)->kind))) {
        P4_RaiseError(s, P4_AdvanceError, "no progressing in repetition");
        return NULL; // TODO: need to handle SOI as well. pest.rs incurs OOM for SOI+.
    }

    min = e->repeat_min;
    max = e->repeat_max;

    P4_Position startpos = P4_GetPosition(s);
    P4_Token *head = NULL, *tail = NULL, *tok = NULL, *whitespace = NULL;

    while (*P4_RemainingText(s) != '\0') {
        P4_MarkPosition(s, before_implicit);

        // implicit `WHITESPACE` and `COMMENT` are inserted
        // between every repetition.
        if (P4_NeedLoosen(s, e)
                && repeated > 0 ) {
            whitespace = P4_Expression_match_implicit_whitespace(s, NULL);
            if (!NO_ERROR(s)) goto finalize;
            P4_AdoptToken(head, tail, whitespace);
        }

        tok = P4_Expression_match(s, e->repeat_expr);

        if (NO_MATCH(s)) {
            assert(tok == NULL);

            // considering the case: MATCH WHITESPACE MATCH WHITESPACE NO_MATCH
            if (P4_NeedLoosen(s, e)            //           ^          ^
                    && repeated > 0)           //           ^          ^ we are here
                P4_SetPosition(s, before_implicit);  //           ^ puke extra whitespace
                                               //           ^ now we are here

            if (min != -1 && repeated < min) {
                P4_RaiseError(s, P4_MatchError, "insufficient repetitions");
                goto finalize;
            } else {                       // sufficient repetitions.
                P4_RescueError(s);
                break;
            }
        }

        if (!NO_ERROR(s))
            goto finalize;

        repeated++;
        P4_AdoptToken(head, tail, tok);

        if (max != -1 && repeated == max) { // enough attempts
            P4_RescueError(s);
            break;
        }
    }

    // there should be no error when repetition is successful.
    assert(NO_ERROR(s));

    // fails when attempts are excessive, e.g. repeated > max.
    if (max != -1 && repeated > max) {
        P4_RaiseError(s, P4_MatchError, "excessive repetitions");
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

    P4_Token* token = P4_Expression_match(s, e->ref_expr);
    if (token != NULL)
        P4_DeleteToken(token);

    P4_SetPosition(s, startpos);

    return NULL;
}

P4_PRIVATE(P4_Token*)
P4_MatchNegative(P4_Source* s, P4_Expression* e) {
    assert(NO_ERROR(s) && e->ref_expr != NULL);

    P4_MarkPosition(s, startpos);
    P4_Token* token = P4_Expression_match(s, e->ref_expr);
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
P4_Expression_match(P4_Source* s, P4_Expression* e) {
    assert(e != NULL);

    if (s->err != P4_Ok) {
        return NULL;
    }

    P4_Error     err = P4_Ok;
    P4_Token* result = NULL;

    if (!is_intermediate(e) && (err = P4_PushFrame(s, e)) != P4_Ok) {
        P4_RaiseError(s, err, "failed to push frame");
        return NULL;
    }

    result = P4_Expression_dispatch(s, e);

    if (!is_intermediate(e) && (err = P4_PopFrame(s, NULL)) != P4_Ok) {
        P4_RaiseError(s, err, "failed to pop frame");
        P4_DeleteToken(result);
        return NULL;
    }

    if (s->err != P4_Ok) {
        assert(result == NULL);
        return NULL;
    }

    return result;
}

P4_Token*
P4_Expression_match_any(P4_Source* s, P4_Expression* e) {
    P4_String str = P4_RemainingText(s);

# define UTF8_CHARLEN(b) (( 0xe5000000 >> (( (b) >> 3 ) & 0x1e )) & 3 ) + 1

    P4_MarkPosition(s, startpos);
    size_t   len = UTF8_CHARLEN(*str);
    P4_SetPosition(s, startpos+len);

    return NULL;
}

P4_Token*
P4_Expression_match_soi(P4_Source* s, P4_Expression* e) {
    if (P4_GetPosition(s) != 0)
        P4_RaiseError(s, P4_MatchError, "not soi");
    return NULL;
}

P4_Token*
P4_Expression_match_eoi(P4_Source* s, P4_Expression* e) {

    if (P4_GetPosition(s) != strlen(s->content)) {
        P4_RaiseError(s, P4_MatchError, "not eoi");
    }
    return NULL;
}

P4_Token*
P4_Expression_match_implicit_whitespace(P4_Source* s, P4_Expression* e) {
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
    P4_Token* result = P4_Expression_match(s, implicit_whitespace);
    if (NO_MATCH(s))
        P4_RescueError(s);

    // (3) Set implicit whitespace back.
    s->whitespacing = false;

    return result;
}

// Poor performance, refactor me.
P4_PUBLIC(void)
P4_PrintSourceAst(P4_Token* token, P4_String buf, size_t indent) {
    P4_Token* current = token;
    char idstr[6];
    idstr[0] = 'R';
    while (current != NULL) {
        for (size_t i = 0; i < indent; i++) strcat(buf, " ");
        strcat(buf, "- ");
        sprintf(idstr+1, "%d", current->expr->id);
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
P4_PUBLIC(P4_String)
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

/*
P4_PUBLIC(P4_Expression*)
P4_CreateNumeric(size_t num) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Numeric;
    expr->num = num;
    return expr;
}
*/

P4_PUBLIC(P4_Expression*)
P4_CreateLiteral(const P4_String literal, bool sensitive) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Literal;
    expr->literal = strdup(literal);
    expr->sensitive = sensitive;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRange(P4_Rune lower, P4_Rune upper) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Range;
    expr->range[0] = lower;
    expr->range[1] = upper;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateReference(P4_RuleID id) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Reference;
    expr->ref_id = id;
    expr->ref_expr = NULL;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreatePositive(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Positive;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateNegative(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Negative;
    expr->ref_expr = refexpr;
    return expr;
}

P4_PRIVATE(P4_Expression*)
P4_CreateContainer(size_t count) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->count = count;
    expr->members = malloc(sizeof(P4_Expression*) * count);
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateSequence(size_t count, ...) {
    P4_Expression* expr = P4_CreateContainer(count);
    expr->kind = P4_Sequence;

    va_list members;
    va_start (members, count);
    for (int i = 0; i < count; i++)
        expr->members[i] = va_arg(members, P4_Expression*);
    va_end (members);

    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateChoice(size_t count, ...) {
    P4_Expression* expr = P4_CreateContainer(count);
    expr->kind = P4_Choice;

    va_list members;
    va_start (members, count);
    for (int i = 0; i < count; i++)
        expr->members[i] = va_arg(members, P4_Expression*);
    va_end (members);

    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatMinMax(P4_Expression* repeat, size_t min, size_t max) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->flag = 0;
    expr->kind = P4_Repeat;
    expr->repeat_expr = repeat;
    expr->repeat_min = min;
    expr->repeat_max = max;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatMin(P4_Expression* repeat, size_t min) {
    return P4_CreateRepeatMinMax(repeat, min, -1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatMax(P4_Expression* repeat, size_t max) {
    return P4_CreateRepeatMinMax(repeat, -1, max);
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatExact(P4_Expression* repeat, size_t minmax) {
    return P4_CreateRepeatMinMax(repeat, minmax, minmax);
}

P4_PUBLIC(P4_Expression*)
P4_CreateZeroOrOnce(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, 1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateZeroOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 0, -1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateOnceOrMore(P4_Expression* repeat) {
    return P4_CreateRepeatMinMax(repeat, 1, -1);
}

P4_PUBLIC(void)
P4_SetRuleID(P4_Expression* e, P4_RuleID id) {
    e->id = id;
}

P4_PUBLIC(bool)
P4_IsRule(P4_Expression* e) {
    return !is_intermediate(e);
}

P4_PUBLIC(P4_Grammar*)    P4_CreateGrammar(void) {
    P4_Grammar* grammar = malloc(sizeof(P4_Grammar));
    grammar->rules = NULL;
    grammar->count = 0;
    grammar->cap = 0;
    return grammar;
}

P4_PUBLIC(void)
P4_DeleteGrammar(P4_Grammar* grammar) {
    if (grammar) {
        // TODO: free each rule.
        free(grammar->rules);
        free(grammar);
    }
}

P4_PUBLIC(P4_Expression*)
P4_GetGrammarRule(P4_Grammar* grammar, P4_RuleID id) {
    for EACH(rule, grammar->rules, grammar->count)
        if (rule->id == id)
            return rule;
    return NULL;
}

P4_PUBLIC(size_t)
P4_AddGrammarRule(P4_Grammar* grammar, P4_RuleID id, P4_Expression* expr) {
    if (grammar->cap == 0) {
        grammar->cap = 32;
        grammar->rules = malloc(sizeof(P4_Expression*) * grammar->cap);
    } else if (grammar->count >= grammar->cap) {
        grammar->cap <<= 1;
        grammar->rules = realloc(grammar->rules, sizeof(P4_Expression*) * grammar->cap); // TODO: leak
    }

    if (grammar->rules == NULL)
        return 0;

    grammar->rules[grammar->count++] = expr;
    expr->id = id;
    return grammar->count;
}

P4_PUBLIC(P4_Source*)    P4_CreateSource(P4_String content, P4_RuleID rule_id) {
    P4_Source* source = malloc(sizeof(P4_Source));
    source->content = content;
    source->rule_id = rule_id;
    source->pos = 0;
    source->err = P4_Ok;
    source->errmsg = NULL;
    source->root = NULL;
    source->frames = NULL;
    source->frames_len = 0;
    source->frames_cap = 0;
    return source;
}

P4_PUBLIC(void)          P4_DeleteSource(P4_Source* source) {
    if (source == NULL)
        return;

    if (source->frames)
        free(source->frames);

    if (source->errmsg)
        free(source->errmsg);

    if (source->root)
        P4_DeleteToken(source->root);

    free(source);
}

P4_PUBLIC(P4_Token*)     P4_GetSourceAst(P4_Source* source) {
    return source->root;
}

P4_PUBLIC(P4_Error)
P4_Parse(P4_Grammar* grammar, P4_Source* source) {
    if (source->err != P4_Ok)
        return source->err;

    if (source->root != NULL)
        return P4_Ok;

    source->grammar = grammar;

    P4_Expression* expr     = P4_GetGrammarRule(grammar, source->rule_id);
    P4_Token*      tok      = P4_Expression_match(source, expr);

    source->root            = tok;

    return source->err;
}

P4_PUBLIC(P4_Error)
P4_SetWhitespaces(P4_Grammar* g, P4_RuleID whitespace_rule_id, P4_RuleID comment_rule_id) {
    P4_Expression* repeat = NULL;

    size_t count = (whitespace_rule_id != 0) + (comment_rule_id != 0);

    if (count == 0)
        return P4_Ok;

    if (whitespace_rule_id != 0) {
        g->whitespace = P4_CreateReference(whitespace_rule_id);
        if (g->whitespace == NULL)
            goto end;
        g->whitespace->ref_expr = P4_GetGrammarRule(g, whitespace_rule_id);
    }

    if (comment_rule_id != 0) {
        g->comment = P4_CreateReference(comment_rule_id);
        if (g->comment == NULL)
            goto end;
        g->comment->ref_expr = P4_GetGrammarRule(g, whitespace_rule_id);
    }

    if (count == 1 && whitespace_rule_id != 0)
        repeat = g->whitespace;

    else if (count == 1 && comment_rule_id != 0)
        repeat = g->comment;

    else if (count == 2) {
        repeat = P4_CreateChoice(2, g->whitespace, g->comment);
        if (repeat == NULL)
            goto end;
    }

    if ((g->implicit_whitespace = P4_CreateZeroOrMore(repeat))== NULL)
        goto end;

    return P4_Ok;

end:
    if (g->comment)
        free(g->comment); // TODO: change to delete expression

    if (g->whitespace)
        free(g->whitespace); // TODO: change to delete expression

    if (g->implicit_whitespace)
        free(g->implicit_whitespace); // TODO: change to delete expression.

    return P4_MemoryError;
}

P4_PUBLIC(P4_Expression*) P4_GetWhitespaces(P4_Grammar* g) {
    return g == NULL ? NULL : g->implicit_whitespace;
}

P4_PUBLIC(void)
P4_SetExpressionFlag(P4_Expression* e, P4_ExpressionFlag f) {
    assert(e != NULL);
    e->flag |= f;
}

P4_PRIVATE(inline P4_Position)
P4_GetPosition(P4_Source* s) {
    return s->pos;
}

P4_PRIVATE(inline void)
P4_SetPosition(P4_Source* s, P4_Position pos) {
    s->pos = pos;
}

/*
 * Get the remaining text.
 */
P4_PRIVATE(inline P4_String)
P4_RemainingText(P4_Source* s) {
    return s->content + s->pos;
}

P4_PUBLIC(P4_Error)
P4_AddLiteral(P4_Grammar* grammar, P4_RuleID id, const P4_String literal, bool sensitive) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateLiteral(literal, sensitive));
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
P4_AddRange(P4_Grammar* grammar, P4_RuleID id, P4_Rune lower, P4_Rune upper) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateRange(lower, upper));
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
P4_AddPositive(P4_Grammar* grammar, P4_RuleID id, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, id, P4_CreatePositive(ref_expr));
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
P4_AddNegative(P4_Grammar* grammar, P4_RuleID id, P4_Expression* ref_expr) {
    if (ref_expr == NULL)
        return P4_NullError;
    P4_AddSomeGrammarRule(grammar, id, P4_CreateNegative(ref_expr));
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
P4_AddSequence(P4_Grammar* grammar, P4_RuleID id, size_t size) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, id)->kind = P4_Sequence;
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
P4_AddChoice(P4_Grammar* grammar, P4_RuleID id, size_t size) {
    P4_AddSomeGrammarRule(grammar, id, P4_CreateContainer(size));
    P4_GetGrammarRule(grammar, id)->kind = P4_Choice;
    return P4_Ok;
}

P4_PUBLIC(P4_Error)
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

P4_PUBLIC(P4_Error)
P4_SetReferenceMember(P4_Expression* expr, size_t offset, P4_RuleID ref) {
    P4_Expression* ref_expr = P4_CreateReference(ref);
    if (ref_expr == NULL) {
        return P4_MemoryError;
    }

    P4_Error error = P4_SetMember(expr, offset, ref_expr);
    if (error != P4_Ok) {
        // TODO: P4_DeleteExpression(ref_expr);
        return error;
    }

    return P4_Ok;
}

P4_PUBLIC(size_t)
P4_GetMembersCount(P4_Expression* expr) {
    if (expr == NULL) {
        return 0;
    }

    return expr->count;
}


P4_PUBLIC(P4_Expression*)
P4_GetMember(P4_Expression* expr, size_t offset) {
    if (expr == NULL
            || expr->members == NULL
            || expr->count == 0) {
        return NULL;
    }

    if (expr->kind != P4_Sequence
            || expr->kind != P4_Choice) {
        return NULL;
    }

    if (offset < 0
            || offset >= expr->count) {
        return NULL;
    }

    return expr->members[offset];
}

P4_PUBLIC(P4_Error)
P4_AddReference(P4_Grammar* grammar, P4_RuleID id, P4_RuleID ref) {
    if (ref == 0) {
        return P4_NullError;
    }

    P4_AddSomeGrammarRule(grammar, id, P4_CreateReference(ref));
    return P4_Ok;
}

P4_PUBLIC(P4_Slice*)
P4_GetTokenSlice(P4_Token* token) {
    if (token == NULL)
        return NULL;

    return &(token->slice);
}

P4_PUBLIC(P4_String)
P4_CopyTokenString(P4_Token* token) {
    if (token == NULL)
        return NULL;

    size_t    len = token->slice.j - token->slice.i;
    assert(len >= 0);

    P4_String str = malloc(len+1);
    strncpy(str, token->text + token->slice.i, len);
    str[len] = '\0';

    return str;
}
