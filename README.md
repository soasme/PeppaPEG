# Peppa PEG

Ultra lightweight PEG Parser in ANSI C. ✨ 🐷 ✨.

# Table of Contents

* [Hello, There](#hello-there)
* [Usage](#usage)
  * [Copy `peppapeg.h` / `peppapeg.c`](#copy-peppapegh--peppapegc)
  * [Basic Usage](#basic-usage)
  * [Full Example Code](#full-example-code)
* [Peppa PEG Rules](#peppa-peg-rules)
  * [Literal](#literal)
    * [Case Sensitive Literal](#case-sensitive-literal)
    * [Case Insensitive Literal](#case-insensitive-literal)
  * [Range](#range)
  * [Sequence](#sequence)
  * [Choice](#choice)
  * [Reference](#reference)
  * [Positive](#positive)
  * [Negative](#negative)
  * [ZeroOrOnce](#zerooronce)
  * [ZeroOrMore](#zeroormore)
  * [OnceOrMore](#onceormore)
  * [RepeatMin](#repeatmin)
  * [RepeatMax](#repeatmax)
  * [RepeatMinMax](#repeatminmax)
  * [RepeatExact](#repeatexact)
* [Peppa PEG Flags](#peppa-peg-flags)
  * [`P4_FLAG_SQUASHED`](#p4_flag_squashed)
  * [`P4_FLAG_LIFTED`](#p4_flag_lifted)
  * [`P4_FLAG_SCOPED`](#p4_flag_scoped)
  * [`P4_FLAG_SPACED`](#p4_flag_spaced)
  * [`P4_FLAG_TIGHT`](#p4_flag_tight)
* [Peppy Hacking Peppa PEG!](peppy-hacking-peppa-peg)
  * [Test](#test)

# Hello, There!

[PEG], or parsing expression grammar, describes a programming language using rules.
In human words, it's for parsing programming languages.

Peppa PEG is an ultra lightweight PEG parser in ANSI C.

WARNING: Peppa PEG hasn't been benchmarked and has not been widely used. Use at your own risk!

# Usage

## Copy `peppapeg.h` / `peppapeg.c`

Peppa PEG has a header file and a C file, so you can easily add
it to your project by copying files "peppapeg.h" and "peppapeg.c".

Peppa PEG assumes your project is ANSI C (C89, or C90) compatible.

Once copied, add include macro and start using the library!

```
#include "peppapeg.h"
```

## Basic Usage

All of the data structures and functions provided by Peppa PEG
start with `P4_`. ;) \
Just count how many `P`s there are in the library name `Peppa PEG`!

In this example, we'll create a grammar with one literal rule matching "Hello World", and let the grammar parse the string "Hello World".

In Peppa PEG, we always start with the struct `P4_Grammar`, which represents a grammar of many PEG rules.
We can create such a data structure using `P4_CreateGrammar`.

```c
P4_Grammar* grammar = P4_CreateGrammar();
```

Each PEG rule is a `P4_Expression` struct. For example, function `P4_CreateLiteral` creates a case-insensitive literal rule.

```c
P4_Expression* rule = P4_CreateLiteral("Hello World", false);
```

Once done, add the rule to the grammar with an integer ID. The ID should be greater than 0.

```c
# define HW_ID 1

P4_AddGrammarRule(grammar, HW_ID, rule);
```

To make our lift easier, `P4_CreateLiteral` and `P4_AddGrammarRule` can be combined into one call `P4_AddLiteral`.
This trick works for all the other [Rules](#peppa-peg-rules), such as `P4_AddRange`, `P4_AddSequence`, etc.

```c
# define HW_ID 1

P4_AddLiteral(grammar, HW_ID, "Hello World", false);
```

You may have seen a similar definition like below in a declarative way.

```
HW <- i"Hello World"
```

However, in Peppa PEG, we define rules in an imperative coding style. Yeah, pretty old school. ;)

Next, we need to wrap the source string into a data structure `P4_Source`.

Function `P4_CreateSource` returns a `P4_Source` struct. It needs two parameters: the source string and the entry rule ID.

```c
P4_Source*  source = P4_CreateSource("Hello World", HW_ID);
```

Now the stage is setup. All we need to do is to parse the source by calling `P4_Parse`.
If everything is all right, it gives us a zero error code `P4_Ok`,
otherwise other `P4_Error`, such as `P4_MatchError`, `P4_MemoryError`, etc.

```c
if (P4_Parse(grammar, source) != P4_Ok) {
    printf("Error: Parse failed.\n");
    exit(1);
}
```

Struct `P4_Source` keeps the parsed result as an AST (abstract syntax tree).
Each node in the AST is a `P4_Token` struct.

Function `P4_GetSourceAst` returns the root node of the AST.

```c
P4_Token* root = P4_GetSourceAst(source);
// traverse: root->next, root->head, root->tail
```

To traverse the AST,

* `node->head` is the first children AST node.
* `node->tail` is the last children AST node.
* `node->next` is the next sibling AST node.
* `node->slice.i` is the start position in the source string that the node covers.
* `node->slice.j` is the end position in the source string that the node covers.
* Function `P4_CopyTokenString` returns the string the AST node covers. Note that it's our responsibility to free the string when finished.

```c
char*       text = P4_CopyTokenString(root);

printf("root span: [%lu %lu]\n", root->slice.i, root->slice.j);
printf("root next: %p\n", root->next);
printf("root head: %p\n", root->head);
printf("root tail: %p\n", root->tail);
printf("root text: %s\n", text);

free(text);
```

Last but not least, don't forget cleaning things up.

* `P4_DeleteSource` deletes the source along with all the AST tokens.
* `P4_DeleteGrammar` deletes the grammar along with all the rule expressions.

```c
P4_DeleteSource(source);
P4_DeleteGrammar(grammar);
```

## Full Example Code

A more complete code for the [Basic Usage](#basic-usage) example is like below.

```c
#include <stdio.h>
#include "peppapeg.h"

# define HW_ID 1

int main(int argc, char* argv[]) {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        printf("Error: CreateGrammar: MemoryError.\n");
        return 1;
    }

    if (P4_AddLiteral(grammar, HW_ID, "HelloWorld", false) != P4_Ok) {
        printf("Error: AddLiteral: MemoryError.\n");
        return 1;
    }

    P4_Source*  source = P4_CreateSource("HelloWorld", HW_ID);
    if (source == NULL) {
        printf("Error: CreateSource: MemoryError.\n");
        return 1;
    }

    if (P4_Parse(grammar, source) != P4_Ok) {
        printf("Error: Parse: ErrCode[%lu] Message[%s]\n",
            P4_GetError(source),
            P4_GetErrorMessage(source)
        );
        return 1;
    }

    P4_Token*   root = P4_GetSourceAst(source);
    char*       text = P4_CopyTokenString(root);

    printf("root span: [%lu %lu]\n", root->slice.i, root->slice.j);
    printf("root next: %p\n", root->next);
    printf("root head: %p\n", root->head);
    printf("root tail: %p\n", root->tail);
    printf("root text: %s\n", text);

    free(text);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);

    return 1;
}
```

Hope you have a basic understanding of how Peppa PEG is used.
Let's explore more Peppa PEG Rules.

# Peppa PEG Rules

# Literal

The Literal rule supports parsing UTF-8 strings.

```c
>> P4_AddLiteral(grammar, ID, "你好, WORLD", true);
P4_Ok

>> P4_Source* source = P4_Source("你好, WORLD", ID);
>> P4_Parse(grammar, source);
P4_Ok
```

## Case Sensitive Literal

Case Sensitive literal matches the exact same string.
When adding the literal rule, set `sensitive=true`.

```c
//                                              sensitive
>> P4_AddLiteral(grammar, ID, "Case Sensitive", true);
P4_Ok

>> P4_Source* source = P4_Source("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Case Insensitive Literal

Case Insensitive Literal matches the same string but ignoring the cases.
When adding the literal rule, set `sensitive=false`.

The `sensitive` option only works for ASCII letters (a-z, A-Z).

```c
//                                                    sensitive
>> P4_AddLiteral(grammar,     ID, "Case Insensitive", false);
P4_Ok

>> P4_Source* source = P4_Source("CASE INSENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Range

Range matches a character that falls between the lower and upper characters.

In this example we match an ASCII digit.

```c
//                            lower     upper
>> P4_AddLiteral(grammar, ID, '0',      '9');
P4_Ok

>> P4_Source* source = P4_Source("0", ID);
>> P4_Parse(grammar, source);
P4_Ok
```

Range supports UTF-8 characters.

In this example we match code points from `U+4E00` to `U+9FCC`, e.g. CJK unified ideographs block. ([好](https://zh.m.wiktionary.org/zh/%E5%A5%BD) is `U+597D`.)

```c
//                            lower     upper
>> P4_AddLiteral(grammar, ID, 0x4E00,   0x9FFF);
P4_Ok

>> P4_Source* source = P4_Source("好", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("Good", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Sequence

Sequence is a container of multiple rules in order.

When applying the Sequence rule to text, the first sequence member is attempted.
If the attempt succeeds, the second sequence member is attempted, so on and on.
If any one of the attempts fails, the Sequence rule fails.

An analogy to Sequence is logic operator `and`.

In this example, we wrap up three literals into a Sequence.

* You need to specify `size=3` when calling `P4_AddSequence`.
* You need to specify the exact offset `0`, `1`, `2` when calling `P4_SetMember`. WARNING: Resetting member at offset may lead to memory leak. Be cautious.

```c
>> P4_AddSequence(grammar, ID, 3);
P4_Ok
>> P4_Expression exprID = P4_GetGrammarRule(grammar, ID);
>> P4_SetMember(exprID, 0, P4_CreateLiteral("HELLO", true));
P4_Ok
>> P4_SetMember(exprID, 1, P4_CreateLiteral(" ", true));
P4_Ok
>> P4_SetMember(exprID, 2, P4_CreateLiteral("WORLD", true));
P4_Ok

>> P4_Source* source = P4_Source("HELLO WORLD", ID);
>> P4_Parse(grammar, source);
P4_Ok
```

Match fails when any inner rule not matching successfully.

```c
>> P4_Source* source = P4_Source("HELLO ", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

Match fails when not all inner rules matching the input.

```c
>> P4_Source* source = P4_Source("HELLO WORL", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```


## Choice

Choice is another container of multiple rules.

When applying the Choice rule to text, the first Choice member is attempted.
If the attempt fails, the second Choice member is attempted, so on and on.
If any one of the attempts succeeds, the Choice rule succeeds immediately and the rest of the Choice members will not be attempted.

When multiple rules match, the first matched rule is used.

An analogy to Choice is logic operator `or`.

In this example, we wrap up two literals into a Choice.

* You need to specify `size=2` when calling `P4_AddChoice`.
* You need to specify the exact offset `0`, `1`  when calling `P4_SetMember`.

```c
>> P4_AddChoice(grammar, ID, 2);
P4_Ok
>> P4_Expression exprID = P4_GetGrammarRule(grammar, ID);
>> P4_SetMember(exprID, 0, P4_CreateLiteral("HELLO WORLD", true));
P4_Ok
>> P4_SetMember(exprID, 1, P4_CreateLiteral("你好, 世界", true));
P4_Ok

>> P4_Source* source = P4_Source("HELLO WORLD", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("你好, 世界", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("HELLO 世界", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Reference

Reference is a lazy-referenced rule.

Analogy to Reference is variable.

In this example we create two references inside a sequence rule using `P4_CreateReference`.

* Function `P4_CreateReference` accepts one parameter: `P4_RuleID id`. The id must be defined before calling `P4_Parse`.

```c
>> # define ENTRY 1
>> # define HELLO 2
>> # define WORLD 3

>> P4_AddLiteral(grammar, HELLO, "HELLO", true);
P4_Ok
>> P4_AddLiteral(grammar, WORLD, "WORLD", true);
P4_Ok
>> P4_AddSequence(grammar, ENTRY, 2);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);

>> P4_SetMember(entry, 0, P4_CreateReference(HELLO));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateReference(WORLD);
P4_Ok

>> P4_Source* source = P4_Source("HELLOWORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

Reference resolves the referenced rule expression in runtime.
This allows referencing a rule defined later.

```c
>> P4_AddSequence(grammar, ENTRY, 2);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);

>> P4_SetMember(entry, 0, P4_CreateReference(HELLO));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateReference(WORLD);
P4_Ok

>> P4_AddLiteral(grammar, HELLO, "HELLO", true);
P4_Ok
>> P4_AddLiteral(grammar, WORLD, "WORLD", true);
P4_Ok

>> P4_Source* source = P4_Source("HELLOWORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

Match fails when the referenced ID doesn't have a rule expression registered.

```c
>> P4_AddSequence(grammar, ENTRY, 2);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetMember(entry, 0, P4_CreateReference(HELLO));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateReference(WORLD);
P4_Ok

>> P4_Source* source = P4_Source("HELLOWORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_NameError
```

## Positive

Positive rule look text ahead but do not consume it. If the incoming text don't match, the rule fails.

* Function `P4_AddPositive` adds a Positive rule to grammar.

```c
>> # define POS     1
>> # define ENTRY   2
>> P4_AddPositive(grammar, POS, P4_CreateLiteral("H", true));
P4_Ok
>> P4_AddSequence(grammar, ENTRY, 2);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetMember(entry, 0, P4_CreateReference(POS));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateLiteral("Hello World", false));
P4_Ok

>> P4_Source* source = P4_Source("Hello world", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("hello world", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Negative

Negative rule look text ahead but do not consume it.
If the incoming text matches, the rule fails.

* Function `P4_AddNegative` adds a Negative rule to grammar.

```c
>> # define NEG     1
>> # define ENTRY   2
>> P4_AddNegative(grammar, NEG, P4_CreateLiteral("H", true));
P4_Ok
>> P4_AddSequence(grammar, ENTRY, 2);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetMember(entry, 0, P4_CreateReference(NEG));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateLiteral("Hello World", false));
P4_Ok

>> P4_Source* source = P4_Source("Hello world", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError

>> P4_Source* source = P4_Source("hello world", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## ZeroOrOnce

Analogy to ZeroOrOnce is Regex `rule?`.

Function `P4_AddZeroOrOnce` adds a repetition expression that repeat an expression for zero or once.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddZeroOrOnce(grammar, ENTRY, ZERO);

>> P4_Source* source = P4_Source("0", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## ZeroOrMore

Analogy to ZeroOrMore is Regex `rule*`.

Function `P4_AddZeroOrMore` adds a repetition expression that repeat an expression for zero or more times.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddZeroOrMore(grammar, ENTRY, ZERO);

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## OnceOrMore

Analogy to OnceOrMore is Regex `rule+`.

Function `AddZeroOrMore` adds a repetition expression that repeat an expression for once or more times.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddOnceOrMore(grammar, ENTRY, ZERO);

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## RepeatMin

Function `P4_AddRepeatMin` adds a repetition expression that repeat an expression for a minimum MIN times.

Analogy to RepeatMin is Regex `rule{min,}`.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddRepeatMin(grammar, ENTRY, ZERO, 5);

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("0000", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## RepeatMax

Function `P4_AddRepeatMin` adds a repetition expression that repeat an expression for a maximum MAX times.

Analogy to RepeatMax is Regex `rule{,max}`.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddRepeatMax(grammar, ENTRY, ZERO, 5);

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("000000", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## RepeatMinMax

Analogy to RepeatMinMax is Regex `rule{min,max}`.

Function `P4_AddRepeatMinMax` adds a repetition expression that repeat an expression for a minimum MIN times and a maximum MAX times.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddRepeatMinMax(grammar, ENTRY, ZERO, 4, 5);

>> P4_Source* source = P4_Source("000", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("000000", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## RepeatExact

Analogy to RepeatExact is Regex `rule{n}`.

Function `P4_AddRepeatExact` adds a repetition expression that repeat an expression for exact N times.
It's equivalent to `P4_AddRepeatMinMax(grammar, id, N, N)`.

```c
>> P4_Expression* ZERO = P4_CreateLiteral("0", true);
>> P4_AddRepeatMinMax(grammar, ENTRY, ZERO, 5);

>> P4_Source* source = P4_Source("00000", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("000000", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

# Peppa PEG Flags

## `P4_FLAG_SQUASHED`

If the expression has `P4_FLAG_SQUASHED`, it will create no children tokens.

```c
>> P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetExpressionFlag(entry, P4_FLAG_SQUASHED);
```

The flag takes effect on the descending expressions.

## `P4_FLAG_LIFTED`

If the expression has `P4_FLAG_LIFTED`, it will omit creating the node but lift the children nodes to the node where it was supposed to reside in the AST.

```c
>> P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetExpressionFlag(entry, P4_FLAG_LIFTED);
```

The flag takes effect on the expression only.

## `P4_FLAG_SCOPED`

If the expression has `P4_FLAG_SCOPED`, it will immediately cancel `P4_FLAG_SQUASHED`'s effect and start creating children token.

```c
>> P4_Expression* entry = P4_GetGrammarRule(grammar, RULE);
>> P4_SetExpressionFlag(entry, P4_FLAG_SCOPED);
```

## `P4_FLAG_SPACED`

If the expression has `P4_FLAG_SPACED` flag, it can be injected between tokens generated from Sequence and Repeat expressions.

```c
>> P4_AddChoice(grammar, WHITESPACE, 3);
P4_Ok
>> P4_Expression* ws = P4_GetGrammarRule(grammar, id);
>> P4_SetMember(ws, 0, P4_CreateLiteral(" ", true));
P4_Ok
>> P4_SetMember(ws, 1, P4_CreateLiteral("\t", true));
P4_Ok
>> P4_SetMember(ws, 2, P4_CreateLiteral("\n", true));
P4_Ok
>> P4_SetExpressionFlag(ws, P4_FLAG_SPACED | P4_FLAG_LIFTED);
```

In this example, the ENTRY rule can match "0 0 0", "0\t0\t0", "00\n0".
For example, `P4_CreateZeroOrMore(P4_CreateLiteral("0", true))` 

```c
>> P4_AddZeroOrMore(grammar, ENTRY, P4_CreateLiteral("0", true));
P4_Ok

>> P4_Source* source = P4_Source("0 0 0", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## `P4_FLAG_TIGHT`

When the expression has `P4_FLAG_SPACED` flag, no expression with `P4_FLAG_SPACED` flag will be injected.

The flag only works for Sequence and Repeat (ZeroOrMore, ZeroOrOnce, OnceOrMore, RepeatExact, etc).

```c
>> P4_AddChoice(grammar, WHITESPACE, 3);
P4_Ok
>> P4_Expression* ws = P4_GetGrammarRule(grammar, WHITESPACE);
>> P4_SetMember(ws, 0, P4_CreateLiteral(" ", true));
P4_Ok
>> P4_SetMember(ws, 1, P4_CreateLiteral("\t", true));
P4_Ok
>> P4_SetMember(ws, 2, P4_CreateLiteral("\n", true));
P4_Ok
>> P4_SetExpressionFlag(ws, P4_FLAG_SPACED | P4_FLAG_LIFTED);

>> P4_AddZeroOrMore(grammar, ENTRY, P4_CreateLiteral("0", true));
P4_Ok
>> P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetExpressionFlag(entry, P4_FLAG_TIGHT);

>> P4_Source* source = P4_Source("0 0 0", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

# Peppy Hacking Peppa PEG!

## Test

Assume you have `cmake` and `gcc` installed.

```bash
$ cmake .
$ make check
...
100% tests passed, 0 tests failed
```

Made with ❤️  by [Ju Lin](https://github.com/soasme).

[PEG]: https://en.wikipedia.org/wiki/Parsing_expression_grammar
