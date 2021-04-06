import cffi

ffi = cffi.FFI()

ffi.cdef("""
typedef uint32_t        P4_ExpressionFlag;
typedef uint32_t        P4_Rune;
typedef char*           P4_String;
typedef uint64_t        P4_RuleID;
typedef void*   P4_UserData;
typedef void    (*P4_UserDataFreeFunc)(P4_UserData);

typedef enum {
    P4_Literal,
    P4_Range,
    P4_Reference,
    P4_Positive,
    P4_Negative,
    P4_Sequence,
    P4_BackReference,
    P4_Choice,
    P4_Repeat,
} P4_ExpressionKind;

typedef enum {
    P4_Ok                   = 0,
    P4_InternalError        = 1,
    P4_MatchError           = 2,
    P4_NameError            = 3,
    P4_AdvanceError         = 4,
    P4_MemoryError          = 5,
    P4_ValueError           = 6,
    P4_IndexError           = 7,
    P4_KeyError             = 8,
    P4_NullError            = 9,
    P4_StackError           = 10,
} P4_Error;

typedef struct P4_Position {
    size_t              pos;
    size_t              lineno;
    size_t              offset;
} P4_Position;

typedef struct P4_Slice {
    P4_Position         start;
    P4_Position         stop;
}                       P4_Slice;

typedef struct P4_Frame {
    struct P4_Expression*   expr;
    bool                    space;
    bool                    silent;
    struct P4_Frame*        next;
} P4_Frame;

typedef struct P4_Source {
    struct P4_Grammar*      grammar;
    P4_RuleID               rule_id;
    P4_String               content;
    P4_Slice                slice;
    size_t                  pos;
    size_t                  lineno;
    size_t                  offset;
    P4_Error                err;
    P4_String               errmsg;
    struct P4_Token*        root;
    bool                    verbose;
    bool                    whitespacing;
    struct P4_Frame*        frame_stack;
    size_t                  frame_stack_size;
} P4_Source;

typedef struct P4_Expression {
    P4_String               name;
    P4_RuleID               id;
    P4_ExpressionKind       kind;
    P4_ExpressionFlag       flag;
    union {
        size_t                      num;
        struct {
            P4_String               literal;
            bool                    sensitive;
            size_t                  backref_index;
        };
        struct {
            P4_String               reference;
            P4_RuleID               ref_id;
            struct P4_Expression*   ref_expr;
        };
        struct {
            P4_Rune                 lower;
            P4_Rune                 upper;
            size_t                  stride;
        };
        struct {
            struct P4_Expression**  members;
            size_t                  count;
        };
        struct {
            struct P4_Expression*   repeat_expr;
            size_t                  repeat_min;
            size_t                  repeat_max;
        };
    };
} P4_Expression;

typedef struct P4_Token {
    P4_String               text;
    P4_Slice                slice;
    P4_RuleID               rule_id;
    P4_UserData             userdata;
    struct P4_Token*        next;
    struct P4_Token*        head;
    struct P4_Token*        tail;
} P4_Token;

typedef P4_Error (*P4_MatchCallback)(struct P4_Grammar*, struct P4_Expression*, struct P4_Token*);
typedef P4_Error (*P4_ErrorCallback)(struct P4_Grammar*, struct P4_Expression*);

typedef struct P4_Grammar{
    struct P4_Expression**  rules;
    size_t                  count;
    int                     cap;
    size_t                  spaced_count;
    struct P4_Expression*   spaced_rules;
    size_t                  depth;
    P4_MatchCallback        on_match;
    P4_ErrorCallback        on_error;
    P4_UserDataFreeFunc     free_func;
} P4_Grammar;

P4_String      P4_Version(void);
P4_Grammar*    P4_LoadGrammar(P4_String rules);
void           P4_DeleteGrammar(P4_Grammar*);
P4_Expression* P4_GetGrammarRuleByName(P4_Grammar* grammar, P4_String name);
P4_Source*     P4_CreateSource(P4_String, P4_RuleID);
void           P4_DeleteSource(P4_Source*);
P4_Error       P4_Parse(P4_Grammar*, P4_Source*);
P4_Token*      P4_GetSourceAst(P4_Source*);
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
