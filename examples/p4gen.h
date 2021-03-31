# ifndef P4_LANG_PEPPA_H
# define P4_LANG_PEPPA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include "../peppapeg.h"

P4_Error P4_PegEval(P4_Token* token, void* result);

# define SLICE_LEN(s) ((s)->stop.pos - (s)->start.pos)

typedef enum {
    P4_PegGrammar         = 1,
    P4_PegRuleRule            = 2,
    P4_PegRuleRuleDecorators  = 3,
    P4_PegRuleRuleName        = 4,
    P4_PegRuleExpression,
    P4_PegRulePrimary,
    P4_PegRuleDecorator,
    P4_PegRuleIdentifier,
    P4_PegRuleLiteral,
    P4_PegRuleInsensitiveLiteral,
    P4_PegRuleRange,
    P4_PegRuleReference,
    P4_PegRulePositive,
    P4_PegRuleNegative,
    P4_PegRuleSequence,
    P4_PegRuleChoice,
    P4_PegRuleBackReference,
    P4_PegRuleRepeat,
    P4_PegRuleRepeatOnceOrMore,
    P4_PegRuleRepeatZeroOrMore,
    P4_PegRuleRepeatZeroOrOnce,
    P4_PegRuleRepeatMin,
    P4_PegRuleRepeatMax,
    P4_PegRuleRepeatExact,
    P4_PegRuleRepeatMinMax,
    P4_PegRuleNumber,
    P4_PegRuleChar,
    P4_PegRuleWhitespace,
} P4_PegRuleRuleID;

P4_Grammar* P4_CreatePegGrammar ();
P4_String   P4_PegKindToName(P4_RuleID);

P4_Grammar* P4_CreatePegGrammar () {
    P4_Grammar* grammar = P4_CreateGrammar();
    P4_Error err = 0;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_PegRuleNumber, 2,
        P4_CreateLiteral("0", true),
        P4_CreateSequenceWithMembers(2,
            P4_CreateRange('1', '9', 1),
            P4_CreateZeroOrMore(P4_CreateRange('0', '9', 1))
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleNumber, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_PegRuleChar, 4,
        P4_CreateRange(0x20, 0x21, 1), /* Can't be 0x22: double quote " */
        P4_CreateRange(0x23, 0x5b, 1), /* Can't be 0x5c: escape leading \ */
        P4_CreateRange(0x5d, 0x10ffff, 1),
        P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("\\", true),
            P4_CreateChoiceWithMembers(9,
                P4_CreateLiteral("\"", true),
                P4_CreateLiteral("/", true),
                P4_CreateLiteral("\\", true),
                P4_CreateLiteral("b", true),
                P4_CreateLiteral("f", true),
                P4_CreateLiteral("n", true),
                P4_CreateLiteral("r", true),
                P4_CreateLiteral("t", true),
                P4_CreateSequenceWithMembers(2,
                    P4_CreateLiteral("u", true),
                    P4_CreateRepeatExact(
                        P4_CreateChoiceWithMembers(3,
                            P4_CreateRange('0', '9', 1),
                            P4_CreateRange('a', 'f', 1),
                            P4_CreateRange('A', 'F', 1)
                        ), 4
                    )
                )
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleChar, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleLiteral, 3,
        P4_CreateLiteral("\"", true),
        P4_CreateZeroOrMore(P4_CreateReference(P4_PegRuleChar)),
        P4_CreateLiteral("\"", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleLiteral, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRange, 6,
        P4_CreateLiteral("[", true),
        P4_CreateReference(P4_PegRuleChar),
        P4_CreateLiteral("-", true),
        P4_CreateReference(P4_PegRuleChar),
        P4_CreateZeroOrOnce(P4_CreateSequenceWithMembers(2,
            P4_CreateLiteral("..", true),
            P4_CreateReference(P4_PegRuleNumber)
        )),
        P4_CreateLiteral("]", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleReference, 2,
        P4_CreateChoiceWithMembers(3,
            P4_CreateRange('a', 'z', 1),
            P4_CreateRange('A', 'Z', 1),
            P4_CreateLiteral("_", true)
        ),
        P4_CreateZeroOrMore(
            P4_CreateChoiceWithMembers(4,
                P4_CreateRange('a', 'z', 1),
                P4_CreateRange('A', 'Z', 1),
                P4_CreateRange('0', '9', 1),
                P4_CreateLiteral("_", true)
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleReference, P4_FLAG_SQUASHED | P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRulePositive, 2,
        P4_CreateLiteral("&", true),
        P4_CreateReference(P4_PegRulePrimary)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleNegative, 2,
        P4_CreateLiteral("!", true),
        P4_CreateReference(P4_PegRulePrimary)
    ))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_PegRuleRepeatOnceOrMore, "+", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_PegRuleRepeatZeroOrMore, "*", true))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, P4_PegRuleRepeatZeroOrOnce, "?", true))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRepeatMin, 4,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_PegRuleNumber),
        P4_CreateLiteral(",", true),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRepeatMax, 4,
        P4_CreateLiteral("{", true),
        P4_CreateLiteral(",", true),
        P4_CreateReference(P4_PegRuleNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRepeatMinMax, 5,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_PegRuleNumber),
        P4_CreateLiteral(",", true),
        P4_CreateReference(P4_PegRuleNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRepeatExact, 3,
        P4_CreateLiteral("{", true),
        P4_CreateReference(P4_PegRuleNumber),
        P4_CreateLiteral("}", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRepeat, 2,
        P4_CreateReference(P4_PegRulePrimary),
        P4_CreateZeroOrOnce(
            P4_CreateChoiceWithMembers(7,
                P4_CreateReference(P4_PegRuleRepeatOnceOrMore),
                P4_CreateReference(P4_PegRuleRepeatZeroOrMore),
                P4_CreateReference(P4_PegRuleRepeatZeroOrOnce),
                P4_CreateReference(P4_PegRuleRepeatExact),
                P4_CreateReference(P4_PegRuleRepeatMinMax),
                P4_CreateReference(P4_PegRuleRepeatMin),
                P4_CreateReference(P4_PegRuleRepeatMax)
            )
        )
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleRepeat, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_PegRulePrimary, 7,
        P4_CreateReference(P4_PegRuleLiteral),
        P4_CreateReference(P4_PegRuleInsensitiveLiteral),
        P4_CreateReference(P4_PegRuleRange),
        P4_CreateSequenceWithMembers(2,
            P4_CreateReference(P4_PegRuleReference),
            P4_CreateNegative(P4_CreateLiteral("=", true))
        ),
        P4_CreateReference(P4_PegRulePositive),
        P4_CreateReference(P4_PegRuleNegative),
        P4_CreateSequenceWithMembers(3,
            P4_CreateLiteral("(", true),
            P4_CreateReference(P4_PegRuleChoice),
            P4_CreateLiteral(")", true)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleInsensitiveLiteral, 2,
        P4_CreateLiteral("i", true),
        P4_CreateReference(P4_PegRuleLiteral)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleInsensitiveLiteral, P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRulePrimary, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddJoin(grammar, P4_PegRuleChoice, "/", P4_PegRuleSequence))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleChoice, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_PegRuleSequence,
                P4_CreateReference(P4_PegRuleRepeat)))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleSequence, P4_FLAG_NON_TERMINAL))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, P4_PegRuleExpression, P4_PegRuleChoice))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleExpression, P4_FLAG_LIFTED))
        goto finalize;

    if (P4_Ok != P4_AddReference(grammar, P4_PegRuleRuleName, P4_PegRuleReference))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleRuleName, P4_FLAG_SQUASHED))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleDecorator, 2,
        P4_CreateLiteral("@", true),
        P4_CreateChoiceWithMembers(6,
            P4_CreateLiteral("squashed", true),
            P4_CreateLiteral("scoped", true),
            P4_CreateLiteral("spaced", true),
            P4_CreateLiteral("lifted", true),
            P4_CreateLiteral("tight", true),
            P4_CreateLiteral("nonterminal", true)
        )
    ))
        goto finalize;

    if (P4_Ok != P4_AddZeroOrMore(grammar, P4_PegRuleRuleDecorators,
            P4_CreateReference(P4_PegRuleDecorator)
    ))
        goto finalize;

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, P4_PegRuleRule, 5,
        P4_CreateReference(P4_PegRuleRuleDecorators),
        P4_CreateReference(P4_PegRuleRuleName),
        P4_CreateLiteral("=", true),
        P4_CreateReference(P4_PegRuleExpression),
        P4_CreateLiteral(";", true)
    ))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, P4_PegGrammar, P4_CreateReference(P4_PegRuleRule)))
        goto finalize;

    if (P4_Ok != P4_AddChoiceWithMembers(grammar, P4_PegRuleWhitespace, 4,
        P4_CreateLiteral(" ", true),
        P4_CreateLiteral("\t", true),
        P4_CreateLiteral("\r", true),
        P4_CreateLiteral("\n", true)
    ))
        goto finalize;

    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, P4_PegRuleWhitespace,
                P4_FLAG_LIFTED | P4_FLAG_SPACED))
        goto finalize;

    return grammar;

finalize:
    P4_DeleteGrammar(grammar);
    return NULL;
}

void* P4_ConcatRune(void *str, P4_Rune chr, size_t n) {
  char *s = (char *)str;

  if (0 == ((P4_Rune)0xffffff80 & chr)) {
    /* 1-byte/7-bit ascii (0b0xxxxxxx) */
    if (n < 1) {
      return 0;
    }
    s[0] = (char)chr;
    s += 1;
  } else if (0 == ((P4_Rune)0xfffff800 & chr)) {
    /* 2-byte/11-bit utf8 code point (0b110xxxxx 0b10xxxxxx) */
    if (n < 2) {
      return 0;
    }
    s[0] = 0xc0 | (char)((chr >> 6) & 0x1f);
    s[1] = 0x80 | (char)(chr & 0x3f);
    s += 2;
  } else if (0 == ((P4_Rune)0xffff0000 & chr)) {
    /* 3-byte/16-bit utf8 code point (0b1110xxxx 0b10xxxxxx 0b10xxxxxx) */
    if (n < 3) {
      return 0;
    }
    s[0] = 0xe0 | (char)((chr >> 12) & 0x0f);
    s[1] = 0x80 | (char)((chr >> 6) & 0x3f);
    s[2] = 0x80 | (char)(chr & 0x3f);
    s += 3;
  } else { /* if (0 == ((int)0xffe00000 & chr)) { */
    /* 4-byte/21-bit utf8 code point (0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx) */
    if (n < 4) {
      return 0;
    }
    s[0] = 0xf0 | (char)((chr >> 18) & 0x07);
    s[1] = 0x80 | (char)((chr >> 12) & 0x3f);
    s[2] = 0x80 | (char)((chr >> 6) & 0x3f);
    s[3] = 0x80 | (char)(chr & 0x3f);
    s += 4;
  }

  return s;
}

P4_String   P4_PegKindToName(P4_RuleID id) {
    switch (id) {
        case P4_PegRuleNumber: return "number";
        case P4_PegRuleChar: return "char";
        case P4_PegRuleLiteral: return "literal";
        case P4_PegRuleInsensitiveLiteral: return "insensitive";
        case P4_PegRuleRange: return "range";
        case P4_PegRuleReference: return "reference";
        case P4_PegRulePositive: return "positive";
        case P4_PegRuleNegative: return "negative";
        case P4_PegRuleChoice: return "choice";
        case P4_PegRuleSequence: return "sequence";
        case P4_PegRuleRepeat: return "repeat";
        case P4_PegRuleRepeatOnceOrMore: return "onceormore";
        case P4_PegRuleRepeatZeroOrMore: return "zeroormore";
        case P4_PegRuleRepeatZeroOrOnce: return "zerooronce";
        case P4_PegRuleRepeatMin: return "repeatmin";
        case P4_PegRuleRepeatMax: return "repeatmax";
        case P4_PegRuleRepeatMinMax: return "repeatminmax";
        case P4_PegRuleRepeatExact: return "repeatexact";
        case P4_PegRuleRuleName: return "name";
        case P4_PegRuleRuleDecorators: return "decorators";
        case P4_PegRuleDecorator: return "decorator";
        case P4_PegRuleRule: return "rule";
        case P4_PegGrammar: return "grammar";
        default: return "<unknown>";
    }
}

P4_Error P4_PegEvalFlag(P4_Token* token, P4_ExpressionFlag *flag) {
    P4_String token_str = token->text + token->slice.start.pos; /* XXX: need slice api. */
    size_t token_len = SLICE_LEN(&token->slice); /* XXX: need slice api. */

    if (memcmp("@squashed", token_str, token_len) == 0)
        *flag = P4_FLAG_SQUASHED;
    else if (memcmp("@scoped", token_str, token_len) == 0)
        *flag = P4_FLAG_SCOPED;
    else if (memcmp("@spaced", token_str, token_len) == 0)
        *flag = P4_FLAG_SPACED;
    else if (memcmp("@lifted", token_str, token_len) == 0)
        *flag = P4_FLAG_LIFTED;
    else if (memcmp("@tight", token_str, token_len) == 0)
        *flag = P4_FLAG_TIGHT;
    else if (memcmp("@nonterminal", token_str, token_len) == 0)
        *flag = P4_FLAG_NON_TERMINAL;
    else {
        *flag = 0; return P4_ValueError;
    }

    return P4_Ok;
}

P4_Error P4_PegEvalRuleFlags(P4_Token* token, P4_ExpressionFlag* flag) {
    P4_Token* child = NULL;
    P4_ExpressionFlag child_flag = 0;
    P4_Error err = P4_Ok;
    for (child = token->head; child != NULL; child = child->next) {
        if ((err = P4_PegEvalFlag(child, &child_flag)) != P4_Ok) {
            *flag = 0;
            return err;
        }
        *flag |= child_flag;
    }
    return P4_Ok;
}

P4_Error P4_PegEvalNumber(P4_Token* token, size_t* num) {
    P4_String s = P4_CopyTokenString(token);

    if (s == NULL)
        return P4_MemoryError;

    *num = atol(s);
    free(s);

    return P4_Ok;
}

size_t P4_CopyRune(P4_String text, size_t start, size_t stop, P4_Rune* rune) {
    char ch0 = text[start];
    if (ch0 == '\\') {
        char ch1 = text[start+1];
        switch (ch1) {
            case 'b': *rune = 0x8; return 2;
            case 't': *rune = 0x9; return 2;
            case 'n': *rune = 0xa; return 2;
            case 'f': *rune = 0xc; return 2;
            case 'r': *rune = 0xd; return 2;
            case '"': *rune = 0x22; return 2;
            case '/': *rune = 0x2f; return 2;
            case '\\': *rune = 0x5c; return 2;
            case 'u': {
                char chs[5] = {0, 0, 0, 0, 0};
                memcpy(chs, text + start + 2, 4);
                *rune = strtoul(chs, NULL, 16);
                return 6;
            }
            default: return 0;
        }
    } else {
        return P4_ReadRune(text+start, rune);
    }
}

P4_Error P4_PegEvalChar(P4_Token* token, P4_Rune* rune) {
    size_t size = P4_CopyRune(
        token->text, token->slice.start.pos, token->slice.stop.pos, rune
    );
    if (size == 0) return P4_ValueError;
    return P4_Ok;
}

P4_Error P4_PegEvalLiteral(P4_Token* token, P4_Expression** expr) {
    size_t len = SLICE_LEN(&token->slice) - 2; /* remove quotes */
    if (len < 0)
        return P4_ValueError;

    size_t i = 0,
           size = 0;
    P4_Error err = 0;
    P4_Rune rune = 0;
    P4_String lit = malloc((len+1) * sizeof(char)),
              cur = lit;

    if (lit == NULL)
        return P4_MemoryError;

    for (i = token->slice.start.pos+1; i < token->slice.stop.pos-1; i += size) {
        size = P4_CopyRune(token->text, i, token->slice.stop.pos-1, &rune);
        if (size == 0) {
            err = P4_ValueError;
            goto finalize;
        }
        cur = P4_ConcatRune(cur, rune, size);
    }
    *cur = '\0';

    *expr = P4_CreateLiteral(lit, true);

finalize:
    free(lit);
    return err;
}

P4_Error P4_PegEvalInsensitiveLiteral(P4_Token* token, P4_Expression** expr) {
    P4_Error err = P4_Ok;

    if ((err = P4_PegEvalLiteral(token->head, expr)) != P4_Ok)
        return err;

    (*expr)->sensitive = false;

    return P4_Ok;
}

P4_Error P4_PegEvalRange(P4_Token* token, P4_Expression** expr) {
    P4_Error err = P4_Ok;
    P4_Rune lower = 0, upper = 0;

    if ((err = P4_PegEvalChar(token->head, &lower)) != P4_Ok)
        return err;

    if ((err = P4_PegEvalChar(token->tail, &upper)) != P4_Ok)
        return err;

    if (lower > upper || lower == 0 || upper == 0)
        return P4_ValueError;

    *expr = P4_CreateRange(lower, upper, 1);
    if (*expr == NULL)
        return P4_MemoryError;

    return P4_Ok;
}

size_t P4_GetChildrenCount(P4_Token* token) {
    P4_Token* child = token->head;

    size_t    child_count = 0;
    while (child != NULL) {
        child_count++; child = child->next;
    }

    return child_count;
}

P4_Error P4_PegEvalMembers(P4_Token* token, P4_Expression* expr) {
    size_t i = 0;
    P4_Token* child = token->head;
    P4_Error err = P4_Ok;
    while (child != NULL) {
        P4_Expression* child_expr = NULL;
        if ((err = P4_PegEval(child, &child_expr)) != P4_Ok)
            return err;
        P4_SetMember(expr, i, child_expr);
        child = child->next;
        i++;
    }
    return P4_Ok;
}

P4_Error P4_PegEvalSequence(P4_Token* token, P4_Expression** expr) {
    P4_Error  err = P4_Ok;

    if ((*expr = P4_CreateSequence(P4_GetChildrenCount(token))) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    if ((err = P4_PegEvalMembers(token, *expr)) != P4_Ok)
        goto finalize;

    return P4_Ok;

finalize:
    P4_DeleteExpression(*expr);
    return err;
}

P4_Error P4_PegEvalChoice(P4_Token* token, P4_Expression** expr) {
    P4_Error  err = P4_Ok;

    if ((*expr = P4_CreateChoice(P4_GetChildrenCount(token))) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    if ((err = P4_PegEvalMembers(token, *expr)) != P4_Ok)
        goto finalize;

    return P4_Ok;

finalize:
    P4_DeleteExpression(*expr);
    return err;
}

P4_Error P4_PegEvalPositive(P4_Token* token, P4_Expression** expr) {
    P4_Error        err = P4_Ok;
    P4_Expression*  ref = NULL;

    if ((err = P4_PegEval(token->head, &ref)) != P4_Ok) {
        return err;
    }

    if (ref == NULL)
        return P4_MemoryError;

    if ((*expr = P4_CreatePositive(ref)) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    return P4_Ok;

finalize:
    P4_DeleteExpression(ref);
    P4_DeleteExpression(*expr);
    return err;
}

P4_Error P4_PegEvalNegative(P4_Token* token, P4_Expression** expr) {
    P4_Error        err = P4_Ok;
    P4_Expression*  ref = NULL;

    if ((err = P4_PegEval(token->head, &ref)) != P4_Ok) {
        return err;
    }

    if (ref == NULL)
        return P4_MemoryError;

    if ((*expr = P4_CreateNegative(ref)) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    return P4_Ok;

finalize:
    P4_DeleteExpression(ref);
    P4_DeleteExpression(*expr);
    return err;
}

P4_Error P4_PegEvalRepeat(P4_Token* token, P4_Expression** expr) {
    P4_Error        err = P4_Ok;
    P4_Expression*  ref = NULL;
    size_t          min = 0, max = SIZE_MAX;

    if ((err = P4_PegEval(token->head, &ref)) != P4_Ok) {
        return err;
    }

    if (ref == NULL)
        return P4_MemoryError;

    switch (token->head->next->rule_id) {
        case P4_PegRuleRepeatZeroOrMore: min = 0; max = SIZE_MAX; break;
        case P4_PegRuleRepeatZeroOrOnce: min = 0; max = 1; break;
        case P4_PegRuleRepeatOnceOrMore: min = 1; max = SIZE_MAX; break;
        case P4_PegRuleRepeatMin:
            if ((err = P4_PegEvalNumber(token->head->next->head, &min)) != P4_Ok)
                goto finalize;
            break;
        case P4_PegRuleRepeatMax:
            if ((err = P4_PegEvalNumber(token->head->next->head, &max)) != P4_Ok)
                goto finalize;
            break;
        case P4_PegRuleRepeatMinMax:
            if ((err = P4_PegEvalNumber(token->head->next->head, &min)) != P4_Ok)
                goto finalize;
            if ((err = P4_PegEvalNumber(token->head->next->tail, &max)) != P4_Ok)
                goto finalize;
            break;
        case P4_PegRuleRepeatExact:
            if ((err = P4_PegEvalNumber(token->head->next->head, &min)) != P4_Ok)
                goto finalize;
            max = min;
            break;
        default:
            err = P4_ValueError;
            goto finalize;
    }

    if (min > max) {
        err = P4_ValueError;
        goto finalize;
    }


    if ((*expr = P4_CreateRepeatMinMax(ref, min, max)) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    return P4_Ok;

finalize:
    P4_DeleteExpression(ref);
    P4_DeleteExpression(*expr);
    return err;
}

P4_Error P4_PegEvalRuleName(P4_Token* token, P4_String* result) {
    size_t i = 0;
    size_t len = SLICE_LEN(&token->slice); /* remove quotes */
    if (len <= 0)
        return P4_ValueError;

    *result = malloc((len+1) * sizeof(char));
    if (*result == NULL)
        return P4_MemoryError;

    memcpy(*result, token->text + token->slice.start.pos, len);
    (*result)[len] = '\0';

    return P4_Ok;
}

P4_Error P4_PegEvalReference(P4_Token* token, P4_Expression** result) {
    P4_Error err = P4_Ok;
    P4_String reference = NULL;

    if ((err = P4_PegEvalRuleName(token, &reference)) != P4_Ok)
        return err;

    if (reference == NULL)
        return P4_ValueError;

    /* We can't know the ref_id at this stage.
     * So, let's just simply create a placeholder.
     */
    if ((*result = P4_CreateReference(SIZE_MAX)) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    /* The string reference is set.
     * When the full grammar is evaluated, we will refresh all ref_ids.
     */
    (*result)->reference = reference;

    return P4_Ok;

finalize:
    if (reference)
        free(reference);

    return err;
}

P4_Error P4_PegEvalGrammarRule(P4_Token* token, P4_Expression** result) {
    P4_String           rule_name = NULL;
    P4_ExpressionFlag   rule_flag = 0;
    P4_Token*           child     = NULL;
    P4_Error            err       = P4_Ok;

    *result = NULL;

    for (child = token->head; child != NULL; child = child->next) {
        switch (child->rule_id) {
            case P4_PegRuleRuleDecorators:
                err = P4_PegEvalRuleFlags(child, &rule_flag);
                break;
            case P4_PegRuleRuleName:
                err = P4_PegEvalRuleName(child, &rule_name);
                break;
            default:
                err = P4_PegEval(child, result);
                break;
        }
        if (err != P4_Ok)
            goto finalize;
    }

    (*result)->name = rule_name;
    (*result)->flag = rule_flag;

    return P4_Ok;

finalize:
    if (rule_name) free(rule_name);
    if (*result) P4_DeleteExpression(*result);
    return err;
}

P4_Error P4_PegSetReferences(P4_Grammar* grammar, P4_Expression* expr) {
    if (expr == NULL)
        return P4_NullError;

    size_t i = 0;
    P4_Error err = P4_Ok;
    switch (expr->kind) {
        case P4_Positive:
        case P4_Negative:
            if (expr->ref_expr)
                err = P4_PegSetReferences(grammar, expr->ref_expr);
            break;
        case P4_Sequence:
        case P4_Choice:
            for (i = 0; i < expr->count; i++)
                if (expr->members[i])
                    if ((err = P4_PegSetReferences(grammar, expr->members[i])) != P4_Ok)
                        return err;
            break;
        case P4_Repeat:
            if (expr->repeat_expr)
                err = P4_PegSetReferences(grammar, expr->repeat_expr);
            break;
        case P4_Reference:
        {
            if (expr->reference == NULL)
                return P4_NullError;

            P4_Expression* ref = P4_GetGrammarRuleByName(grammar, expr->reference);
            if (expr == NULL)
                return P4_NameError;

            expr->ref_id = ref->id;
            break;
        }
        default:
            break;
    }

    return err;
}

P4_Error P4_PegEvalGrammar(P4_Token* token, P4_Grammar** result) {
    P4_Error    err = P4_Ok;
    size_t      i = 0;
    size_t      count = P4_GetChildrenCount(token);

    if ((*result = P4_CreateGrammar()) == NULL) {
        err = P4_MemoryError;
        goto finalize;
    }

    P4_Token* child = NULL;
    P4_RuleID id = 1;
    for (child = token->head; child != NULL; child = child->next) {
        P4_Expression* rule = NULL;

        if ((err = P4_PegEvalGrammarRule(child, &rule)) != P4_Ok) {
            goto finalize;
        }

        if ((err = P4_AddGrammarRule(*result, id, rule)) != P4_Ok) {
            goto finalize;
        }

        id++;
    }

    for (i = 0; i < (*result)->count; i++) {
        if ((err = P4_PegSetReferences(*result, (*result)->rules[i])) != P4_Ok) {
            goto finalize;
        }
    }

finalize:
    if (err)
        P4_DeleteGrammar(*result);

    return err;
}

P4_Error P4_PegEval(P4_Token* token, void* result) {
    P4_Error err = P4_Ok;
    switch (token->rule_id) {
        case P4_PegRuleDecorator:
            return P4_PegEvalFlag(token, result);
        case P4_PegRuleRuleDecorators:
            return P4_PegEvalRuleFlags(token, result);
        case P4_PegRuleNumber:
            return P4_PegEvalNumber(token, result);
        case P4_PegRuleChar:
            return P4_PegEvalChar(token, result);
        case P4_PegRuleLiteral:
            return P4_PegEvalLiteral(token, result);
        case P4_PegRuleInsensitiveLiteral:
            return P4_PegEvalInsensitiveLiteral(token, result);
        case P4_PegRuleRange:
            return P4_PegEvalRange(token, result);
        case P4_PegRuleSequence:
            return P4_PegEvalSequence(token, result);
        case P4_PegRuleChoice:
            return P4_PegEvalChoice(token, result);
        case P4_PegRulePositive:
            return P4_PegEvalPositive(token, result);
        case P4_PegRuleNegative:
            return P4_PegEvalNegative(token, result);
        case P4_PegRuleRepeat:
            return P4_PegEvalRepeat(token, result);
        case P4_PegRuleReference:
            return P4_PegEvalReference(token, result);
        case P4_PegGrammar:
            return P4_PegEvalGrammar(token, result);
        default: return P4_ValueError;
    }
    return P4_Ok;
}

#ifdef __cplusplus
}
#endif

# endif
