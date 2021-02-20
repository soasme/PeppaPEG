import cffi

ffi = cffi.FFI()

ffi.cdef("""
# define P4_FLAG_NONE                   0x0
# define P4_FLAG_SQUASHED               0x1
# define P4_FLAG_LIFTED                 0x10
# define P4_FLAG_TIGHT                  0x100
# define P4_FLAG_SCOPED                 0x1000
# define P4_FLAG_SPACED                 0x10000
# define P4_DEFAULT_RECURSION_LIMIT     8192

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
    /* P4_Numeric, */
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

typedef enum {
    /* No error is like a bless. */
    P4_Ok                   = 0,
    /* When there is an internal error.
     * Please raise an issue: https://github.com/soasme/peppapeg/issues. */
    P4_InternalError        = 1,
    /* When no text is matched. */
    P4_MatchError           = 2,
    /* When no name is resolved. */
    P4_NameError            = 3,
    /* When the parse gets stuck forever or has reached the end. */
    P4_AdvanceError         = 4,
    /* When out of memory. */
    P4_MemoryError          = 5,
    /* When the given value is of unexpected type. */
    P4_ValueError           = 6,
    /* When the index is out of boundary. */
    P4_IndexError           = 7,
    /* When the id is out of the table. */
    P4_KeyError             = 8,
    /* When null is encountered. */
    P4_NullError            = 9,
    /* When recursion limit is reached. */
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
    struct P4_Grammar*      grammar;
    P4_RuleID               rule_id;
    P4_String               content;
    P4_Position             pos;
    P4_Error                err;
    P4_String               errmsg;
    struct P4_Token*        root;
    bool                    verbose;
    bool                    whitespacing;
    /* The top frame in the stack. */
    struct P4_Frame*        frame_stack;
    /* The size of frame stack. */
    size_t                  frame_stack_size;
} P4_Source;

typedef struct P4_Expression {
    P4_RuleID            id;
    P4_ExpressionKind     kind;
    P4_ExpressionFlag      flag;

    union {
        size_t                      num;

        struct {
            P4_String               literal;
            bool                    sensitive;
        };

        struct {
            P4_String               reference;
            P4_RuleID               ref_id;
            struct P4_Expression*   ref_expr;
        };

        P4_RuneRange                range;

        struct {
            struct P4_Expression**  members;
            size_t                  count;
        };

        struct {
            struct P4_Expression*   repeat_expr; /* maybe we can merge it with ref_expr? */
            size_t                  repeat_min;
            size_t                  repeat_max;
        };

        size_t                      backref_index;
    };
} P4_Expression;

typedef struct P4_Token {
    P4_String               text;
    P4_Slice                slice;
    struct P4_Expression*   expr;
    struct P4_Token*        next;
    struct P4_Token*        head;
    struct P4_Token*        tail;
} P4_Token;

typedef struct P4_Grammar{
    struct P4_Expression**  rules;
    int                     count;
    int                     cap;
    size_t                  spaced_count;
    struct P4_Expression*   spaced_rules;
    size_t                  depth;
} P4_Grammar;
P4_String      P4_Version(void);
P4_Expression* P4_CreateLiteral(const P4_String, bool sensitive);
P4_Expression* P4_CreateRange(P4_Rune, P4_Rune);
P4_Expression* P4_CreateReference(P4_RuleID);
P4_Expression* P4_CreatePositive(P4_Expression*);
P4_Expression* P4_CreateNegative(P4_Expression*);
P4_Expression* P4_CreateSequence(size_t);
P4_Expression* P4_CreateChoice(size_t);
P4_Expression* P4_CreateSequenceWithMembers(size_t, ...);
P4_Expression* P4_CreateChoiceWithMembers(size_t, ...);
P4_Expression* P4_CreateRepeatMinMax(P4_Expression*, size_t, size_t);
P4_Expression* P4_CreateRepeatMin(P4_Expression*, size_t);
P4_Expression* P4_CreateRepeatMax(P4_Expression*, size_t);
P4_Expression* P4_CreateRepeatExact(P4_Expression*, size_t);
P4_Expression* P4_CreateZeroOrOnce(P4_Expression*);
P4_Expression* P4_CreateZeroOrMore(P4_Expression*);
P4_Expression* P4_CreateOnceOrMore(P4_Expression*);
P4_Expression* P4_CreateBackReference(size_t);
P4_Error       P4_SetMember(P4_Expression*, size_t, P4_Expression*);
P4_Error       P4_SetReferenceMember(P4_Expression*, size_t, P4_RuleID);
size_t         P4_GetMembersCount(P4_Expression*);
P4_Expression* P4_GetMember(P4_Expression*, size_t);
void           P4_DeleteExpression(P4_Expression*);
P4_Error       P4_SetRuleID(P4_Expression*, P4_RuleID);
bool           P4_IsRule(P4_Expression*);
bool           P4_IsSquashed(P4_Expression*);
bool           P4_IsLifted(P4_Expression*);
bool           P4_IsTight(P4_Expression*);
bool           P4_IsScoped(P4_Expression*);
bool           P4_IsSpaced(P4_Expression*);
void           P4_SetExpressionFlag(P4_Expression*, P4_ExpressionFlag);
void           P4_SetSquashed(P4_Expression*);
void           P4_SetLifted(P4_Expression*);
void           P4_SetTight(P4_Expression*);
void           P4_SetSpaced(P4_Expression*);
void           P4_SetScoped(P4_Expression*);
P4_Grammar*    P4_CreateGrammar(void);
void           P4_DeleteGrammar(P4_Grammar*);
P4_Error       P4_AddGrammarRule(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddLiteral(P4_Grammar*, P4_RuleID, const P4_String, bool sensitive);
P4_Error       P4_AddRange(P4_Grammar*, P4_RuleID, P4_Rune, P4_Rune);
P4_Error       P4_AddReference(P4_Grammar*, P4_RuleID, P4_RuleID);
P4_Error       P4_AddPositive(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddNegative(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddSequence(P4_Grammar*, P4_RuleID, size_t);
P4_Error       P4_AddSequenceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);
P4_Error       P4_AddChoice(P4_Grammar*, P4_RuleID, size_t);
P4_Error       P4_AddChoiceWithMembers(P4_Grammar*, P4_RuleID, size_t, ...);
P4_Error       P4_AddZeroOrOnce(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddZeroOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddOnceOrMore(P4_Grammar*, P4_RuleID, P4_Expression*);
P4_Error       P4_AddRepeatMin(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_Error       P4_AddRepeatMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_Error       P4_AddRepeatMinMax(P4_Grammar*, P4_RuleID, P4_Expression*, size_t, size_t);
P4_Error       P4_AddRepeatExact(P4_Grammar*, P4_RuleID, P4_Expression*, size_t);
P4_Expression* P4_GetGrammarRule(P4_Grammar*, P4_RuleID);
P4_Error       P4_SetGrammarRuleFlag(P4_Grammar*, P4_RuleID, P4_ExpressionFlag);
P4_Error       P4_SetRecursionLimit(P4_Grammar*, size_t limit);
size_t         P4_GetRecursionLimit(P4_Grammar*);
P4_Source*     P4_CreateSource(P4_String, P4_RuleID);
void           P4_DeleteSource(P4_Source*);
P4_Token*      P4_GetSourceAst(P4_Source*);
P4_Position    P4_GetSourcePosition(P4_Source*);
void           P4_PrintSourceAst(P4_Token*, P4_String, size_t);
P4_Error       P4_Parse(P4_Grammar*, P4_Source*);
bool           P4_HasError(P4_Source*);
P4_Error       P4_GetError(P4_Source*);
P4_String      P4_GetErrorMessage(P4_Source*);
P4_Token*      P4_CreateToken(P4_String, size_t, size_t, P4_Expression*);
void           P4_DeleteToken(P4_Token*);
P4_Slice*      P4_GetTokenSlice(P4_Token*);
P4_String      P4_CopyTokenString(P4_Token*);
""")

ffi.set_source("_peppapeg",
    '#include "peppapeg.h"',
    sources=(
        "../../peppapeg.c",
    ),
    include_dirs=(
        "../../",
    ),
)


if __name__ == '__main__':
    ffi.compile()
