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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "peppapeg.h"

# define MARK_POS(s, p) P4_Position (p) = P4_GetPosition((s));
# define MOVE_POS(s, p) P4_SetPosition((s), (p));

typedef struct P4_State {
    /* The grammar. */
    P4_Grammar*     grammar;
    /* The input text. */
    P4_String       text;
    /* The size of input text. Shortcut for strlen(s->text). */
    size_t          size;
    /* The depth of nested AST. */
    size_t          depth;
    /* The parsed position of the input text. */
    P4_Position     pos;
    /* The expression call stack. */
    P4_Expression** frames;
    size_t          frames_size;
    size_t          frames_cap;
} P4_State;

P4_PRIVATE(inline size_t)       P4_ReadRune(P4_String, P4_Rune*);
P4_PRIVATE(inline int)          P4_CaseCmpInsensitive(P4_String, P4_String, size_t);

P4_PRIVATE(P4_State*)           P4_CreateState(P4_Grammar*, P4_String, size_t);
P4_PRIVATE(void)                P4_DeleteState(P4_State*);

P4_PRIVATE(P4_Position)         P4_GetPosition(P4_State*);
P4_PRIVATE(void)                P4_SetPosition(P4_State*, P4_Position);

P4_PRIVATE(inline P4_String)    P4_RemainingText(P4_State*);

P4_PRIVATE(inline bool)         P4_NeedLoosen(P4_State*, P4_Expression*);
P4_PRIVATE(inline bool)         P4_NeedSquash(P4_State*, P4_Expression*);
P4_PRIVATE(inline bool)         P4_NeedSilent(P4_State*, P4_Expression*);

P4_PRIVATE(void)                P4_RaiseError(P4_State*, P4_Error, P4_String);
P4_PRIVATE(void)                P4_RescueError(P4_State*);

P4_PRIVATE(size_t)              P4_PushFrame(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Expression*)      P4_PopFrame(P4_State*);

P4_PRIVATE(P4_Token*)           P4_Match(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchNumeric(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchLiteral(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRange(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchReference(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchPositive(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchNegative(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchSequence(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchChoice(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchZeroOrOnce(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchZeroOrMore(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchOnceOrMore(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeatMin(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeatMax(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeatMinMax(P4_State*, P4_Expression*);
P4_PRIVATE(P4_Token*)           P4_MatchRepeatExact(P4_State*, P4_Expression*);

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

P4_PRIVATE(P4_State*)
P4_CreateState(P4_Grammar* grammar, P4_String text, size_t size) {
    P4_State* state = (P4_State*) malloc (sizeof(P4_Grammar));
    state->grammar = grammar;
    state->text = text;
    state->size = size;
    state->depth = 0;
    state->pos = 0;
    state->frames = 0;
    state->frames_size = 0;
    state->frames_cap = 0;
    return state;
}

P4_PRIVATE(void)
P4_DeleteState(P4_State* state) {
    if (state) {
        if (state->frames) {
            free(state->frames);
        }
        free(state);
    }
}

P4_PRIVATE(P4_Position)
P4_GetPosition(P4_State* state) {
    return state->pos;
}

P4_PRIVATE(void)
P4_SetPosition(P4_State* state, P4_Position pos) {
    state->pos = pos;
}

P4_PRIVATE(inline P4_String)
P4_RemainingText(P4_State* state) {
    if (state->pos > state->size)
        return NULL;

    return state->text+state->pos;
}


/* Determine if the implicit whitespace should be applied. */
P4_PRIVATE(inline bool)
P4_NeedLoosen(P4_State* s, P4_Expression* e) {
    // (1) when there is no implicit whitespace rule.
    if (s->grammar->whitespaces == NULL)
        return false;


    for (int i = 0; i < s->frames_size; i++) {
        // (2) when we're already matching whitespace.
        if (s->grammar->whitespaces == s->frames[i])
            return false;

        // (3) when e is a tighted expression.
        if (P4_IsTighted(s->frames[i]))
            return false;
    }

    return true;
}

/* Determine if the inner token should be squashed. */
P4_PRIVATE(inline bool)
P4_NeedSquash(P4_State* s, P4_Expression* e) {
    for (int i = s->frames_size-2; i>=0; i--) {
        if (P4_IsSquashed(s->frames[i]))
            return true;
    }
    return false;
}

/* Determine if token should be dismissed. */
P4_PRIVATE(inline bool)
P4_NeedSilent(P4_State* s, P4_Expression* e) {
    return !P4_IsRule(e) || P4_IsLifted(e) || P4_NeedSquash(s, e);
}


P4_PRIVATE(void)
P4_RaiseError(P4_State* s, P4_Error err, P4_String errmsg) {
    s->grammar->err = err;
    if (errmsg)
        s->grammar->errmsg = strdup(errmsg);
    else
        s->grammar->errmsg = 0;
}

P4_PRIVATE(void)
P4_RescueError(P4_State* s) {
    s->grammar->err = 0;
    if (s->grammar->errmsg) {
        free(s->grammar->errmsg);
        s->grammar->errmsg = 0;
    }
}

P4_PRIVATE(size_t)
P4_PushFrame(P4_State* s, P4_Expression* e) {

# define DEFAULT_CAP 32

    if (s->frames_cap == 0) {
        s->frames_cap = DEFAULT_CAP;
        s->frames = malloc(sizeof(P4_Expression*) * s->frames_cap);
    } else if (s->frames_size >= s->frames_cap){
        s->frames_cap <<= 1;
        s->frames = realloc(s->frames, s->frames_cap); // potential memory leak
    }

    if (s->frames == NULL) {
        return 0;
    }

    s->frames[s->frames_size++] = e;
    return s->frames_size;
}

P4_PRIVATE(P4_Expression*)
P4_PopFrame(P4_State* s) {
    if (s->frames_cap == 0 || s->frames_size == 0)
        return NULL;
    s->frames_size--;
    P4_Expression* result = s->frames[s->frames_size];
    s->frames[s->frames_size+1] = NULL;
    return result;
}

P4_PRIVATE(P4_Token*)
P4_Match(P4_State* s, P4_Expression* e) {
    P4_Token* result = NULL;

    if (e == NULL) {
        P4_SetError(s->grammar, P4_NullError, 0);
        return result;
    }

    if (P4_IsRule(e) && P4_PushFrame(s, e) == 0) {
        P4_SetError(s->grammar, P4_MemoryError, 0);
        return result;
    }

    switch (e->kind) {
        case P4_Literal:
            result = P4_MatchLiteral(s, e);
            break;
        default:
            P4_SetError(s->grammar, P4_InternalError, 0);
            result = NULL;
            break;
    }

    if (P4_IsRule(e))
        P4_PopFrame(s);

    if (P4_HasError(s->grammar)) {
        assert(result == NULL);
        return result;
    }

    return result;
}

P4_PRIVATE(P4_Token*)
P4_MatchLiteral(P4_State* s, P4_Expression* e) {
    P4_String str = NULL;
    P4_Token* result = NULL;

    if (P4_HasError(s->grammar))
        return NULL;

    if (*(str = P4_RemainingText(s)) == '\0') {
        P4_SetError(s->grammar, P4_AdvanceError, 0);
        return NULL;
    }

    size_t litlen = strlen(e->literal);

    MARK_POS(s, startpos);
    if ((!e->sensitive && P4_CaseCmpInsensitive(e->literal, str, litlen) != 0)
            || (e->sensitive && memcmp(e->literal, str, litlen) != 0)) {
        P4_SetError(s->grammar, P4_MatchError, 0);
        return NULL;
    }
    MOVE_POS(s, startpos+litlen);
    MARK_POS(s, endpos);

    if (P4_NeedSilent(s, e)) {
        return NULL;
    }

    P4_Slice slice = {startpos, endpos};
    if ((result = P4_CreateToken(s->text, slice, e)) == NULL) {
        P4_SetError(s->grammar, P4_MemoryError, 0);
        return NULL;
    }

    return result;
}

// PUBLIC functions start from here.

/*
 * Get version string.
 */
P4_PUBLIC(P4_String)
P4_Version(void) {
    static char version[15];
    sprintf(version, "%i.%i.%i", P4_MAJOR_VERSION, P4_MINOR_VERSION, P4_PATCH_VERSION);
    return version;
}

P4_PUBLIC(P4_Expression*)
P4_CreateNumeric(size_t num) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Numeric;
    expr->num = num;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateLiteral(const P4_String literal, bool sensitive) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Literal;
    expr->literal = strdup(literal);
    expr->sensitive = sensitive;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRange(P4_Rune lower, P4_Rune upper) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Range;
    expr->lower = lower;
    expr->upper = upper;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateReference(P4_RuleID id) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Reference;
    expr->refid = id;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreatePositive(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Positive;
    expr->refexpr = refexpr;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateNegative(P4_Expression* refexpr) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Negative;
    expr->refexpr = refexpr;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateSequence(size_t count, ...) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Sequence;
    expr->count = count;
    expr->members = malloc(sizeof(P4_Expression*) * count);

    va_list members;
    va_start (members, count);
    for (int i = 0; i < count; i++)
        expr->members[i] = va_arg(members, P4_Expression*);
    va_end (members);

    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateChoice(size_t count, ...) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Choice;
    expr->count = count;
    expr->members = malloc(sizeof(P4_Expression*) * count);

    va_list members;
    va_start (members, count);
    for (int i = 0; i < count; i++)
        expr->members[i] = va_arg(members, P4_Expression*);
    va_end (members);

    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeat(P4_Expression* repeat, uint64_t min, uint64_t max) {
    P4_Expression* expr = malloc(sizeof(P4_Expression));
    expr->kind = P4_Repeat;
    expr->repeat = repeat;
    expr->min = min;
    expr->max = max;
    return expr;
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatMin(P4_Expression* repeat, uint64_t min) {
    return P4_CreateRepeat(repeat, min, -1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatMax(P4_Expression* repeat, uint64_t max) {
    return P4_CreateRepeat(repeat, -1, max);
}

P4_PUBLIC(P4_Expression*)
P4_CreateRepeatExact(P4_Expression* repeat, uint64_t minmax) {
    return P4_CreateRepeat(repeat, minmax, minmax);
}

P4_PUBLIC(P4_Expression*)
P4_CreateZeroOrOnce(P4_Expression* repeat) {
    return P4_CreateRepeat(repeat, 0, 1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateZeroOrMore(P4_Expression* repeat) {
    return P4_CreateRepeat(repeat, 0, -1);
}

P4_PUBLIC(P4_Expression*)
P4_CreateOnceOrMore(P4_Expression* repeat) {
    return P4_CreateRepeat(repeat, 1, -1);
}

P4_PUBLIC(P4_Expression**)
P4_AddMember(P4_Expression* expr, P4_Expression* member) {
    // TODO: more efficient memory management.
    size_t newcount= expr->count + 1;
    P4_Expression** newbuf = realloc(expr->members, newcount*sizeof(P4_Expression*));

    if (newbuf == NULL) {
        return NULL;
    }

    expr->members = newbuf;
    expr->members[newcount-1] = member;

    return newbuf;
}

P4_PUBLIC(size_t)
P4_GetMembersCount(P4_Expression* expr) {
    return expr->count;
}

P4_PUBLIC(void)
P4_DeleteExpression(P4_Expression* expr) {
    if (expr == NULL)
        return;

    switch (expr->kind) {
        case P4_Literal:
            free(expr->literal);
            expr->literal = NULL;
            break;

        case P4_Positive:
        case P4_Negative:
            P4_DeleteExpression(expr->refexpr);
            expr->refexpr = NULL;
            break;

        case P4_Sequence:
        case P4_Choice:
            for (int i = 0; i < expr->count; i++)
                P4_DeleteExpression(expr->members[i]);
            free(expr->members);
            expr->members = NULL;
            break;

        case P4_Repeat:
            P4_DeleteExpression(expr->repeat);
            expr->repeat = NULL;
            break;

        default:
            break;
    }

    free(expr);
}


P4_PUBLIC(P4_String)
P4_PrintExpression(P4_Expression* expr) {
    return strdup("<Expression>"); // TODO
}

P4_PUBLIC(void)
P4_SetRuleID(P4_Expression* expr, P4_RuleID id) {
    expr->id = id;
}

P4_PUBLIC(bool)
P4_IsRule(P4_Expression* expr) {
    return expr->id != 0;
}

P4_PUBLIC(bool)
P4_IsSquashed(P4_Expression* expr) {
    return (expr->flags & P4_FLAG_SQUASHED) != 0;
}

P4_PUBLIC(bool)
P4_IsLifted(P4_Expression* expr) {
    return (expr->flags & P4_FLAG_LIFTED) != 0;
}

P4_PUBLIC(bool)
P4_IsTighted(P4_Expression* expr) {
    return (expr->flags & P4_FLAG_TIGHTED) != 0;
}

P4_PUBLIC(void)
P4_SetExpressionFlag(P4_Expression* expr, uint8_t flag) {
    expr->flags |= flag;
}

P4_PUBLIC(void)
P4_SetSquashed(P4_Expression* expr) {
    P4_SetExpressionFlag(expr, P4_FLAG_SQUASHED);
}

P4_PUBLIC(void)
P4_SetLifted(P4_Expression* expr) {
    P4_SetExpressionFlag(expr, P4_FLAG_LIFTED);
}

P4_PUBLIC(void)
P4_SetTighted(P4_Expression* expr) {
    P4_SetExpressionFlag(expr, P4_FLAG_TIGHTED);
}

P4_PUBLIC(void)
P4_UnsetExpressionFlag(P4_Expression* expr, uint8_t flag) {
    expr->flags &= ~flag;
}


P4_PUBLIC(void)
P4_UnsetSquashed(P4_Expression* expr) {
    P4_UnsetExpressionFlag(expr, P4_FLAG_SQUASHED);
}

P4_PUBLIC(void)
P4_UnsetLifted(P4_Expression* expr) {
    P4_UnsetExpressionFlag(expr, P4_FLAG_LIFTED);
}

P4_PUBLIC(void)
P4_UnsetTighted(P4_Expression* expr) {
    P4_UnsetExpressionFlag(expr, P4_FLAG_TIGHTED);
}

P4_PUBLIC(P4_Grammar*)
P4_CreateGrammar(void) {
    P4_Grammar* grammar = malloc(sizeof(P4_Grammar));
    grammar->rules = P4_CreateChoice(0);
    grammar->err   = 0;
    grammar->errmsg = 0;
    grammar->whitespaces = 0;
    return grammar;
}

P4_PUBLIC(void)
P4_DeleteGrammar(P4_Grammar* grammar) {
    if (grammar->rules != NULL) {
        P4_DeleteExpression(grammar->rules);
        grammar->rules = NULL;
    }

    if (grammar->errmsg != NULL) {
        free(grammar->errmsg);
        grammar->errmsg = NULL;
    }

    if (grammar->whitespaces != NULL) {
        P4_DeleteExpression(grammar->whitespaces);
        grammar->whitespaces = NULL;
    }

    free(grammar);
}

P4_PUBLIC(void)
P4_AddGrammarRule(P4_Grammar* grammar, P4_RuleID id, P4_Expression* expr) {
    if (P4_AddMember(grammar->rules, expr) == NULL) {
        P4_SetError(grammar, P4_MemoryError, 0);
        return;
    }

    P4_SetRuleID(expr, id);
}

P4_PUBLIC(P4_Expression*)
P4_GetGrammarRule(P4_Grammar* grammar, P4_RuleID id) {
    P4_Expression* expr = NULL;
    for (int i = 0; i < grammar->rules->count; i++) {
        expr = grammar->rules->members[i];
        if (expr != NULL && expr->id == id)
            return expr;
    }
    return NULL;
}

P4_PUBLIC(bool)
P4_HasError(P4_Grammar* grammar) {
    return grammar->err != 0;
}

P4_PUBLIC(P4_String)
P4_PrintError(P4_Grammar* grammar) {
    return strdup(grammar->errmsg);
}

P4_PUBLIC(P4_Token*)
P4_Parse(P4_Grammar* grammar, P4_RuleID id, P4_String input) {
    return P4_ParseWithLength(grammar, id, input, strlen(input));
}

P4_PUBLIC(P4_Token*)
P4_ParseWithLength(P4_Grammar* grammar, P4_RuleID id, P4_String input, P4_Position len) {
    P4_State* state = NULL;
    P4_Expression* expr = NULL;
    P4_Token* result = NULL;

    if ((expr = P4_GetGrammarRule(grammar, id))== NULL) {
        P4_SetError(grammar, P4_NameError, 0);
        goto end;
    }

    if ((state = P4_CreateState(grammar, input, len)) == NULL) {
        P4_SetError(grammar, P4_MemoryError, 0);
        goto end;
    }

    result = P4_Match(state, expr);

end:
    if (state != NULL)
        P4_DeleteState(state);

    return result;
}

P4_PUBLIC(void)
P4_SetError(P4_Grammar* grammar, P4_Error err, P4_String errmsg) {
    grammar->err = err;

    if (errmsg != NULL) {
        free(grammar->errmsg);
        grammar->errmsg = strdup(errmsg);
    }
}

P4_PUBLIC(P4_Token*)
P4_CreateToken(P4_String text, P4_Slice slice, P4_Expression* expr) {
    P4_Token* token = malloc(sizeof(P4_Token));
    token->text = text;
    token->slice[0] = slice[0];
    token->slice[1] = slice[1];
    token->expr = expr;
    token->next = token->head = token->tail = NULL;
    return token;
}

P4_PUBLIC(void)
P4_DeleteToken(P4_Token* token) {
    if (token == NULL)
        return;

    if (token->head == NULL) {
        free(token);
        return;
    }

    P4_Token* current = token;
    P4_Token* tmp = NULL;
    while (current != NULL) {
        tmp = current->next;
        P4_DeleteToken(current);
        current = tmp;
    }
}

P4_PUBLIC(void)
P4_AppendToken(P4_Token* token, P4_Token* sibling) {
    token->next = sibling;
}

P4_PUBLIC(void)
P4_AdoptToken(P4_Token* token, P4_Token* child) {
    if (child == NULL)
        return;

    if (token->head == NULL)
        token->head = child;

    if (token->tail == NULL)
        token->tail = child;
    else
        token->tail->next = child;

    if (token->tail != NULL) {
        while (token->tail->next != NULL) {
            token->tail = token->tail->next;
        }
    }
}

P4_PUBLIC(P4_String)
P4_PrintToken(P4_Token* token) {
    return strdup("<Token>"); // TODO
}
