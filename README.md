# Peppa PEG

Ultra lightweight PEG Parser in ANSI C. ✨ 🐷 ✨.

```
  ____                          ____  _____ ____
 |  _ \ ___ _ __  _ __   __ _  |  _ \| ____/ ___|
 | |_) / _ \ '_ \| '_ \ / _` | | |_) |  _|| |  _
 |  __/  __/ |_) | |_) | (_| | |  __/| |__| |_| |
 |_|   \___| .__/| .__/ \__,_| |_|   |_____\____|
           |_|   |_|
```

[![Test | Build Status](https://github.com/soasme/PeppaPEG/actions/workflows/check.yml/badge.svg?branch=main)](https://github.com/soasme/PeppaPEG/actions/workflows/check.yml)
[![Docs | Build Status](https://github.com/soasme/PeppaPEG/actions/workflows/docs.yml/badge.svg?branch=main)](https://github.com/soasme/PeppaPEG/actions/workflows/docs.yml)
[![Docs](https://img.shields.io/badge/docs-soasme.com-green)](https://www.soasme.com/PeppaPEG)

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
    * [BackReference](#backreference)
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
* [Caveats](#caveats)
  * [Recursion Limit](#recursion-limit)
* [Peppy Hacking Peppa PEG!](peppy-hacking-peppa-peg)
  * [Test](#test)
  * [Docs](#docs)
  * [Examples](#examples)

# Hello, There!

[PEG], or parsing expression grammar, describes a programming language using rules.
In human words, it's for parsing programming languages.

Peppa PEG is an ultra lightweight PEG parser in ANSI C.

# Usage

## Copy `peppapeg.h` / `peppapeg.c`

Peppa PEG has a header file and a C file, so you can add
it to your project by copying files "peppapeg.h" and "peppapeg.c".

Peppa PEG assumes your project is ANSI C (C89, or C90) compatible.

Once copied, add include macro and start using the library!

```
#include "peppapeg.h"
```

## Basic Usage

All of the data structures and functions provided by Peppa PEG start with `P4_`. ;)
Just count how many `P`s there are in the library name `Peppa PEG`!

In this example, we'll create a grammar with one literal rule matching "Hello World", and then parse the string "Hello World".

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
# define ENTRY 1

P4_AddGrammarRule(grammar, ENTRY, rule);
```

To make things easier, `P4_CreateLiteral` and `P4_AddGrammarRule` can be combined into one call `P4_AddLiteral`.
This trick works for all of the [Expression Kinds](#peppa-peg-rules), such as `P4_AddRange`, `P4_AddSequence`, etc.

```c
P4_AddLiteral(grammar, ENTRY, "Hello World", false);
```

Next, we wrap the source into a data structure `P4_Source`, which is the handle to the source to parse.

Function `P4_CreateSource` returns a `P4_Source` struct. It needs two parameters: the source string and the entry rule ID.

```c
P4_Source*  source = P4_CreateSource("Hello World", ENTRY);
```

Now the stage is setup; call `P4_Parse`.
If everything is okay, it returns a zero value - `P4_Ok`,
Otherwise a non-zero value `P4_Error`, such as `P4_MatchError`, `P4_MemoryError`, etc.

```c
if (P4_Parse(grammar, source) != P4_Ok) {
    printf("Error: Parse failed.\n");
    exit(1);
}
```

Struct `P4_Source` produces an abstract syntax tree, or AST if successfully.
Each node in the AST is a `P4_Token` struct.
Function `P4_GetSourceAst` returns the root node of the AST.

```c
P4_Token* root = P4_GetSourceAst(source);
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

A complete code for the [Basic Usage](#basic-usage) example is like below.

```c
#include <stdio.h>
#include "peppapeg.h"

# define ENTRY 1

int main(int argc, char* argv[]) {
    P4_Grammar* grammar = P4_CreateGrammar();
    if (grammar == NULL) {
        printf("Error: CreateGrammar: MemoryError.\n");
        return 1;
    }

    if (P4_AddLiteral(grammar, ENTRY, "HelloWorld", false) != P4_Ok) {
        printf("Error: AddLiteral: MemoryError.\n");
        return 1;
    }

    P4_Source*  source = P4_CreateSource("HelloWorld", ENTRY);
    if (source == NULL) {
        printf("Error: CreateSource: MemoryError.\n");
        return 1;
    }

    if (P4_Parse(grammar, source) != P4_Ok) {
        printf("Error: Parse: ErrCode[%lu] Err[%s] Message[%s]\n",
            P4_GetError(source),
            P4_GetErrorString(P4_GetError(source)),
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
>> P4_AddLiteral(grammar, ENTRY, "你好, WORLD", true);
P4_Ok

>> P4_Source* source = P4_Source("你好, WORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

## Case Sensitive Literal

Case Sensitive literal matches the exact same string.
When adding the literal rule, set `sensitive=true`.

```c
//                                              sensitive
>> P4_AddLiteral(grammar, ENTRY , "Case Sensitive", true);
P4_Ok

>> P4_Source* source = P4_Source("Case Sensitive", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Case Insensitive Literal

Case Insensitive Literal matches the same string but ignores the case.
When adding the literal rule, set `sensitive=false`.

The `sensitive` option only works for ASCII letters (a-z, A-Z).

```c
//                                                    sensitive
>> P4_AddLiteral(grammar,  ENTRY, "Case Insensitive", false);
P4_Ok

>> P4_Source* source = P4_Source("CASE INSENSITIVE", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Range

Range matches a character that falls between the lower and upper characters.

In this example we match an ASCII digit.

```c
// ENTRY <- [0-9]

//                               lower     upper    stride
>> P4_AddRange(grammar, ENTRY,   '0',      '9',     1);
P4_Ok

>> P4_Source* source = P4_Source("0", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

Range supports UTF-8 characters as well.

In this example we match the code points starting from `U+4E00` to `U+9FCC`, e.g. CJK unified ideographs block. ([好](https://zh.m.wiktionary.org/zh/%E5%A5%BD) is `U+597D`.)

```c
//                               lower     upper    stride
>> P4_AddRange(grammar, ENTRY,   0x4E00,   0x9FFF,  1);
P4_Ok

>> P4_Source* source = P4_Source("好", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("Good", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

## Sequence

Sequence is a container of multiple rules in order.

When applying the Sequence rule to text, the first sequence member is attempted.
If the attempt succeeds, the second sequence member is attempted, so on and on.
If any one of the attempts fails, the Sequence rule fails.

* Function `P4_CreateSequence` creates a sequence. Example: `P4_CreateSequence(3)`.
  * Function `P4_SetMember` set the member at the exact location. Example: `P4_SetMember(expr, 2, P4_CreateLiteral("HELLO", true))`.
* Function `P4_CreateSequenceWithMembers` combines `P4_CreateSequence` and `P4_SetMember`.
* Function `P4_AddSequence` adds a sequence to grammar rule set.
* Function `P4_AddSequenceWithMembers` adds a sequence along with all the members to grammar rule set.

In this example, we wrap up three literals into a Sequence.

```c
>> P4_AddSequence(grammar, ENTRY, 3);
P4_Ok
>> P4_Expression entry = P4_GetGrammarRule(grammar, ENTRY);
>> P4_SetMember(entry, 0, P4_CreateLiteral("HELLO", true));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateLiteral(" ", true));
P4_Ok
>> P4_SetMember(entry, 2, P4_CreateLiteral("WORLD", true));
P4_Ok

>> P4_Source* source = P4_Source("HELLO WORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

It's equivalent to:

```c
>> P4_AddSequenceWithMembers(
..   grammar,
..   ENTRY,
..   3,
..   P4_CreateLiteral("HELLO", true),
..   P4_CreateLiteral(" ", true),
..   P4_CreateLiteral("WORLD", true)
.. );
P4_Ok

>> P4_Source* source = P4_Source("HELLO WORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

The match fails when any inner rule is not matching.

```c
>> P4_Source* source = P4_Source("HELLO ", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

The match fails when not all inner rules is matching.

```c
>> P4_Source* source = P4_Source("HELLO WORL", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

Note that Sequence "owns" the members so `P4_DeleteExpression(sequence)` will free all members as well. Don't free the members by yourself!

### BackReference

Sequence expression can have BackReference members. They are useful when verbatim string of previous match is needed.

Function `P4_CreateBackReference` creates a back reference with the index of a previous member. The index starts with zero.

BackReference supports case insensitive.

For example, the below grammar can match `'...'` or `"..."`, but cannot match `"...'` or `'..."`.

```c
>> P4_AddSequenceWithMembers(grammar, ENTRY, 3,
..   P4_CreateReference(MARKER),
..   P4_CreateLiteral("...", true),
..   P4_CreateBackReference(0, true)
.. );
>> P4_AddChoiceWithMembers(grammar, MARKER, 2,
..   P4_CreateLiteral("'", true),
..   P4_CreateLiteral("\"", true)
.. );

>> P4_Source* source = P4_Source("'...'", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("\"...'", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

BackReference can only be used in a Sequence and cannot be a grammar rule, therefore there is no `P4_AddBackReference` function.

## Choice

Choice is another container of multiple rules.

When applying the Choice rule to text, the first Choice member is attempted.
If the attempt fails, the second Choice member is attempted, so on and on.
If any one of the attempts succeeds, the Choice rule succeeds immediately and the rest of the Choice members will not be attempted.
If no Choice member matches, the Choice rule fails.

* Function `P4_CreateChoice` creates a choice. Example: `P4_CreateChoice(2)`.
  * Function `P4_SetMember` set the member at the exact location. Example: `P4_SetMember(expr, 1, P4_CreateLiteral("HELLO", true))`.
* Function `P4_CreateChoiceWithMembers` combines `P4_CreateChoice` and `P4_SetMember`. Example: `P4_CreateChoiceWithMembers(2, P4_CreateLiteral("HELLO", true), P4_CreateLiteral("KIA ORA", true))`.
* Function `P4_AddChoice` adds a choice to grammar rule set. Example: `P4_AddChoice(grammar, ENTRY , 2)`.
* Function `P4_AddChoiceWithMembers` adds a choice along with all the members to grammar rule set. Example: `P4_AddChoiceWithMembers(grammar, ENTRY , 2, P4_CreateLiteral("HELLO", true), P4_CreateLiteral("KIA ORA", true))`

In this example, we wrap up two literals into a Choice.

```c
>> P4_AddChoice(grammar, ENTRY, 3);
P4_Ok
>> P4_Expression* entry = P4_GetGrammarRule(grammar, ENTRy);
>> P4_SetMember(entry, 0, P4_CreateLiteral("HELLO WORLD", true));
P4_Ok
>> P4_SetMember(entry, 1, P4_CreateLiteral("你好, 世界", true));
P4_Ok
```

It's equivalent to:

```c
>> P4_AddChoiceWithMembers(
..   grammar,
..   ENTRY,
..   2,
..   P4_CreateLiteral("HELLO WORLD", true),
..   P4_CreateLiteral("你好, 世界", true)
.. );
P4_Ok
```

```c
>> P4_Source* source = P4_Source("HELLO WORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("你好, 世界", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Source("HELLO 世界", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

Note that Choice "owns" the members so `P4_DeleteExpression(sequence)` will free all members as well. Don't free the members by yourself!

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
>> P4_AddSequenceWithMembers(grammar, ENTRY, 2,
..  P4_CreateReference(HELLO),
..  P4_CreateReference(WORLD)
.. );
P4_Ok

>> P4_Source* source = P4_Source("HELLOWORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_Ok
```

Reference resolves the referenced rule expression in runtime.
This allows referencing a rule defined later.

```c
>> P4_AddSequenceWithMembers(grammar, ENTRY, 2,
..  P4_CreateReference(HELLO),
..  P4_CreateReference(WORLD)
.. );
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
>> P4_AddSequenceWithMembers(grammar, ENTRY, 2,
..  P4_CreateReference(HELLO),
..  P4_CreateReference(WORLD)
.. );
P4_Ok

>> P4_Source* source = P4_Source("HELLOWORLD", ENTRY);
>> P4_Parse(grammar, source);
P4_NameError
```

## Positive

Positive rule look text ahead but do not consume it. If the incoming text don't match, the rule fails.

* Function `P4_AddPositive` adds a Positive rule to grammar.

```c
>> # define ENTRY   1
>> # define POS     2
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
>> # define ENTRY   1
>> # define NEG     2
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
>> P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SQUASHED);
```

The flag takes effect on the descending expressions.

## `P4_FLAG_LIFTED`

If the expression has `P4_FLAG_LIFTED`, it will omit creating the node but lift the children nodes to the node where it was supposed to reside in the AST.

```c
>> P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_LIFTED);
```

The flag takes effect on the expression only.

## `P4_FLAG_SCOPED`

If the expression has `P4_FLAG_SCOPED`, it will immediately cancel `P4_FLAG_SQUASHED`'s effect and start creating children token.

```c
>> P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_SCOPED);
```

## `P4_FLAG_SPACED`

If the expression has `P4_FLAG_SPACED` flag, it can be injected between tokens generated from Sequence and Repeat expressions.

```c
>> P4_AddChoiceWithMembers(grammar, WHITESPACE, 3,
..  P4_CreateLiteral(" ", true),
..  P4_CreateLiteral("\t", true),
..  P4_CreateLiteral("\n", true)
.. );
P4_Ok
>> P4_SetGrammarRuleFlag(grammar, WHITESPACE, P4_FLAG_SPACED | P4_FLAG_LIFTED);
P4_Ok
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
>> P4_AddChoiceWithMembers(grammar, WHITESPACE, 3,
..  P4_CreateLiteral(" ", true),
..  P4_CreateLiteral("\t", true),
..  P4_CreateLiteral("\n", true)
.. );
P4_Ok
>> P4_SetGrammarRuleFlag(grammar, WHITESPACE, P4_FLAG_SPACED | P4_FLAG_LIFTED);
P4_Ok

>> P4_AddZeroOrMore(grammar, ENTRY, P4_CreateLiteral("0", true));
P4_Ok
>> P4_SetGrammarRuleFlag(grammar, ENTRY, P4_FLAG_TIGHT);

>> P4_Source* source = P4_Source("0 0 0", ENTRY);
>> P4_Parse(grammar, source);
P4_MatchError
```

# Caveats

## Recursion Limit

Peppa PEG is a recursion-based parser so it will be crashed if the stack address is exhausted. To prevent that, Peppa PEG set a limit on the recursion depth. By default, it's 8192.

If Peppa PEG should run a very small embedded device, you can adjust it via `P4_SetRecursionLimit`.

```c
>> P4_SetRecursionLimit(grammar, 1000);
P4_Ok

>> P4_GetRecursionLimit(grammar);
1000
```

# Peppy Hacking Peppa PEG!

## Test

Peppa PEG test requires downloading the test framework Unity:

```bash
$ git submodule init
$ git submodule update
```

Assume you have `cmake` and `gcc` installed.

```bash
(root) $ mkdir -p build
(root) $ cd build
(build) $ cmake ..
(build) $ make check
...
100% tests passed, 0 tests failed
```

If valgrind is installed, you can also run the test along with memory leak check.

```bash
(root) $ mkdir -p build
(root) $ cd build
(build) $ cmake -DENABLE_VALGRIND=ON ..
(build) $ make check
```

If you feel having a testing environment is hard, try docker:

```bash
$ docker run --rm -v `pwd`:/app -it ubuntu:latest bash
# apt-get install gcc gdb valgrind make cmake python3 python3-venv python3-pip doxygen
# mkdir -p build && cd build && cmake .. && make check
```

## Docs

Peppa PEG docs can be built via doxygen:

```bash
(root) $ cd build
(build) $ rm -rf docs && make docs
```

The outputs are stored on `build/docs`.

## Examples

* Write an INI Parser using Peppa PEG: [ini.h](examples/ini.h), [ini.c](examples/ini.c).
* Write a Mustache Parser using Peppa PEG: [mustache.h](examples/mustache.h).
* Write a JSON Parser using Peppa PEG: [json.h](examples/json.h).
* Write a Calculator Parser using Peppa PEG: [calc.h](examples/calc.h), [calc.c](examples/calc.c).

Made with ❤️  by [Ju](https://github.com/soasme).

[PEG]: https://en.wikipedia.org/wiki/Parsing_expression_grammar
