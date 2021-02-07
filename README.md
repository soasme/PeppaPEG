# Peppa PEG

Ultra lightweight PEG Parser in ANSI C. ✨ 🐷 ✨.

## Hello, There!

Want to parse a programming language using PEG? Use Peppa PEG!

## Usage

### Copy `peppapeg.h` / `peppapeg.c`.

Peppa PEG has a header file and a C file, so you can easily add
it to your project by copying files "peppapeg.h" and "peppapeg.c".

Peppa PEG assumes your project is ANSI C (C89, or C90) compatible.

Once copied, add include macro and start using the library!

```
#include "peppapeg.h"
```

### Basic Usage

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

## Example

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

    P4_Expression* rule = P4_CreateLiteral("HelloWorld", false);
    if (rule == NULL) {
        printf("Error: CreateLiteral: MemoryError.\n");
        return 1;
    }

    if (P4_AddGrammarRule(grammar, SIMPLE_LITERAL, rule) != P4_Ok) {
        printf("Error: AddGrammarRule: MemoryError.\n");
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

## Peppa PEG Rules

### Case Sensitive Literal
### Case Insensitive Literal

Case Sensitive literal matches the exact same string.
When adding the literal rule, set `sensitive=true`.

```c
>> P4_AddLiteral(Grammar, ID, "Case Sensitive", true);
P4_Ok

>> P4_Source* source = P4_Source("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_MatchError
```

Case Insensitive Literal matches the same ignoring the cases.
When adding the literal rule, set `sensitive=false`.

```c
>> P4_AddLiteral(Grammar, ID, "Case Insensitive", true);
P4_Ok

>> P4_Source* source = P4_Source("CASE INSENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_Ok

>> P4_Source* source = P4_Parse("CASE SENSITIVE", ID);
>> P4_Parse(grammar, source);
P4_Ok
```

The `sensitive` option only works for ASCII letters (a-z, A-Z).

The Literal rule support parsing UTF-8 strings.

```c
>> P4_AddLiteral(Grammar, ID, "你好, WORLD", false);
P4_Ok

>> P4_Source* source = P4_Source("你好, world", ID);
>> P4_Parse(grammar, source);
P4_Ok
```

### Range

### Reference

### Positive

### Negative

### Sequence

### Choice

### ZeroOrOnce

### ZeroOrMore

### OnceOrMore

### RepeatMin
### RepeatMax
### RepeatMinMax
### RepeatExact

## Peppa PEG Flags

### SQUASHED

### LIFTED

### TIGHT

### SCOPED

### SPACED

## Peppa PEG Tokens

## Peppa PEG Built-in Rules

### SOI

### EOI

### ASCII_LETTERS

### ASCII_LOWER

### ASCII_UPPER

### DIGITS

### HEXDIGITS

### OCTDIGITS

### PUNCTUATION

### WHITESPACE

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
