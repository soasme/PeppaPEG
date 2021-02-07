# Peppa PEG

Ultra lightweight PEG Parser in ANSI C. ✨ 🐷 ✨.

# Hello, There!

Want to parse a programming language using PEG? Use Peppa PEG!

# Usage

## Copy `peppapeg.h` / `peppapeg.c`.

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

Struct `P4_Grammar` represents a grammar of many PEG rules.

You can create such a data structure using `P4_CreateGrammar`.

```c
P4_Grammar* grammar = P4_CreateGrammar();
```

Each PEG rule is a `P4_Expression` struct.

* Function `P4_CreateLiteral` creates a literal rule. The parameter `true` indicates the rule being case sensitive.

```c
P4_Expression* rule = P4_CreateLiteral("Hello World", false);
```

Once done, associate the rule with an integer ID to the grammar.

```c
# define HW_ID 1

P4_AddGrammarRule(grammar, HW_ID, rule);
```

To make it easier, `P4_CreateLiteral` and `P4_AddGrammarRule` can be merged into one single call `P4_AddLiteral`.

```c
# define HW_ID 1

P4_AddLiteral(grammar, HW_ID, "Hello World", false);
```

Next, you need to wrap up the source string in to a data structure `P4_Source`.

* Function `P4_CreateSource` returns a `P4_Source` struct. Except the source string, you also specify the rule ID, which decided the exact rule to apply when parsing.

```c
P4_Source*  source = P4_CreateSource("Hello World", HW_ID);
```

Now the stage is setup. All you need to do is to parse it and check the result.
If everything is all right, it gives you a zero error code `P4_Ok`,
otherwise other `P4_Error` codes.

```c
if (P4_Parse(grammar, source) != P4_Ok) {
    printf("Error: Parse failed.\n");
    exit(1);
}
```

The parsed result is an AST (abstract syntax tree), and each node in the
AST is a `P4_Token` struct.

* Function `P4_GetSourceAst` gives you the root node of the AST. You may traverse the AST and do your homework now.
  * `node->head` is the first children AST node.
  * `node->tail` is the last children AST node.
  * `node->next` is the next sibling AST node.
  * `node->slice.i` is the start position in the source string that the node covers.
  * `node->slice.j` is the end position in the source string that the node covers.

In this example, `root->head`, `root->tail` and `root->next` are all NULL due to having only one rule and `root->slice` is `[0, strlen("Hello World")]`.

```c
P4_Token* root = P4_GetSourceAst(source);
// traverse: root->next, root->head, root->tail
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

    if (P4_AddLiteral(grammar, SIMPLE_LITERAL, "HelloWorld", false) != P4_Ok) {
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
    printf("root span: [%lu %lu]\n", root->slice.i, root->slice.j);
    printf("root next: %p\n", root->next);
    printf("root head: %p\n", root->head);
    printf("root tail: %p\n", root->tail);

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

Case Insensitive Literal matches the same ignoring the cases.
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
P4_Ok
```

## Range

Range matches a character that falls between lower and upper code points.

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
It matches to the input only when all of the inner rules match successfully.

An analogy to Sequence is logic operator `and`.

In this example, we wrap up three literals into a Sequence.

* You need to specify `size=3` when calling `P4_AddSequence`.
* You need to specify the exact offset `0`, `1`, `2` when calling `P4_AddMember`.

```c
>> P4_AddSequence(grammar, ID, 3);
P4_Ok
>> P4_Expression exprID = P4_GetGrammarRule(grammar, ID);
>> P4_AddMember(exprID, 0, P4_CreateLiteral("HELLO", true));
P4_Ok
>> P4_AddMember(exprID, 1, P4_CreateLiteral(" ", true));
P4_Ok
>> P4_AddMember(exprID, 2, P4_CreateLiteral("WORLD", true));
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
It matches to the input only when one of the inner rules matches successfully.

When multiple rules match, the first matched rule is used.

An analogy to Choice is logic operator `or`.

In this example, we wrap up two literals into a Choice.

* You need to specify `size=2` when calling `P4_AddChoice`.
* You need to specify the exact offset `0`, `1`  when calling `P4_AddMember`.

```c
>> P4_AddChoice(grammar, ID, 2);
P4_Ok
>> P4_Expression exprID = P4_GetGrammarRule(Grammar, ID);
>> P4_AddMember(exprID, 0, P4_CreateLiteral("HELLO WORLD", true));
P4_Ok
>> P4_AddMember(exprID, 1, P4_CreateLiteral("你好, 世界", true));
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

## Positive

## Negative

## ZeroOrOnce

## ZeroOrMore

## OnceOrMore

## RepeatMin
## RepeatMax
## RepeatMinMax
## RepeatExact

# Peppa PEG Flags

## SQUASHED

## LIFTED

## TIGHT

## SCOPED

## SPACED

# Peppa PEG Tokens

# Peppa PEG Built-in Rules

## SOI

## EOI

## ASCII_LETTERS

## ASCII_LOWER

## ASCII_UPPER

## DIGITS

## HEXDIGITS

## OCTDIGITS

## PUNCTUATION

## WHITESPACE

## TODO

- [ ] Print error messages for Human.
- [ ] New Expression Kind: Numeric.
- [ ] New Expression Kind: CharacterSet.
- [ ] New Expression Kind: Complement.
- [ ] New Expression Kind: Panic.
- [ ] New Expression Kind: BackReference.
- [ ] New Expression Kind: Function.
- [ ] Run in VERBOSE mode.
- [ ] Performance test.
- [ ] Callbacks for P4_Token.

# Peppy Packing Peppa PEG!

Made with ❤️  by [Ju Lin](https://github.com/soasme).
