.. _peg:

PEG APIs
==========

Grammar Rules
-------------

The grammar is simply a list of rules with names. Rules are defined like this:

.. code-block::

    rule_1 = ... ;
    rule_2 = ... ;
    rule_3 = ... ;
    (more rules)

On the left hand side, they are rule names.

One the right hand side, they are rule expressions, which we will cover in this rest of the document.

In the middle, they are equal signs (`=`).

They should end with colons (`;`).

The first grammar rule will be assigned with rule id 1.
The ids for the rest increase one by one.

The grammar rule names must start with an alphabet or underscore,
followed by a sequence of either alphabets or digits or underscores `_`.

Literal
-------

The literal matches an exact same string surrounded by double quotes.

For example,

.. code-block::

    greeting = "hello world";

To ignore the case, insert `i` to the left quote:

.. code-block::

    greeting = i"hello world";

The case insensitive literal applies to not only ASCII chars, but also extended ASCII chars, such as ì / Ì.

.. code-block::

    greeting = i"hello worìd";

UTF-8 is supported:

.. code-block::

    greeting = "你好，世界";

Emoji is supported:

.. code-block::

    greeting = "Peppa 🐷";

You can encode ASCII characters via `\\x` followed by 2 hex digits.

.. code-block::

    greeting = "\x48\x65\x6c\x6c\x6f, world";

You can encode UTF-8 characters via `\\u` followed by 4 hex digits or `\\U` followed by 8 hex digits.

.. code-block::

    greeting = "\u4f60\u597D, world\u000c";

Range
------

Range **matches a single character in range**.

In this example, any character between `'0'` to `'9'` can match.

.. code-block::

    digits = [0-9];

The lower and upper character of the range can be not only ASCII characters but also UTF-8 code points.
The syntax can be `\\uXXXX` or `\\uXXXXXXXX`.

.. code-block::

    digits = [\u4e00-\u9fff];

A small trick to match any character is to specify the range from `\\u0001` to `\\U0010ffff`,
which are the minimum and the maximum code point in UTF-8 encoding.

.. code-block::

    any = [\u0001-\U0010ffff];

The value of lower must be less or equal than the upper.

.. code-block::

    // INVALID
    any = [\U0010ffff-\u0001];

Range supports an optional `stride` to skip certain amount of characters in the range.
In this example, only odd number between `'0'` to `'9'` can match.

.. code-block::

    digits = [0-9..2];

Range also supports certain unicode character sets,  such as `C`, `Cc`, `Cf`, `Co`, `Cs`,
`Ll`, `Lm`, `Lo`, `Lt`, `Lu`, `L`, `Nd`, `Nl`, `No`, `N`, etc.
They're wrapped via `\\p{}`, for example:

* unicode_letter: a Unicode code point classified as "Letter" (Ll+Lm+Lo+Lt+Lu).
* unicode_digit: a Unicode code point classified as "Number, decimal digit"(Nd).

.. code-block::

    unicode_letter = [\p{L}];
    unicode_digit  = [\p{Nd}];

Dot
---

Single dot `.` can match any UTF-8 code point. It's a syntax sugar for `[\\u0001-\\U0010ffff]`.

.. code-block::

    any = .;

Sequence
--------

Sequence **matches a sequence of sub-expressions in order**.

When parsing, the first sequence member is attempted. If succeeds, the second is attempted, so on and on.
If any one of the attempts fails, the match fails.

For example:

.. code-block::

    greeter = "Hello" " " "world";


Choice
-------

Choice **matches one of the sub-expression.**

When parsing, the first sequence member is attempted. If fails, the second is attempted, so on and on.
If any one of the attempts succeeds, the match succeeds. If all attempts fail, the match fails.

For example:

.. code-block::

   greeter = "Hello World" / "你好，世界" / "Kia Ora";

Reference
---------

Reference **matches a string based on the referenced grammar rule**.

For example, `greeter` is just a reference rule in `greeting`. When matching `greeting`, it will use the referenced grammar rule `greeter` first, e.g. `"Hello" / "你好"`, then match " world".

.. code-block::

    greeting = greeter " world";
    greeter  = "Hello" / "你好";

The order of defining a rule does not matter.

.. code-block::

    greeter  = "Hello" / "你好";
    greeting = greeter " world";

One should ensure all references must have corresponding rule defined, otherwise, the parse will fail with :c:enum:`P4_MatchError`.

Positive
--------

Positive **tests if the sub-expression matches**.

Positive attempts to match the sub-expression. If succeeds, the test passes. Positive does not "consume" any text.

Positive can be useful in limiting the possibilities of the latter member in a Sequence. In this example, the Sequence expression must start with "Hello", e.g. "Hello World", "Hello WORLD", "Hello world", etc, will match but "HELLO WORLD" will not match.

.. code-block::

    greeting = &"Hello" i"hello world";

Negative
--------

Negative **tests if the sub-expression does not match**.

Negative expects the sub-expression doesn't match. If fails, the test passes. Negative does not "consume" any text.

Negative can be useful in limiting the possiblities of the latter member in a Sequence. In this example, the Sequence expression must not start with "Hello", e.g. "HELLO World", "hello WORLD", "hello world", etc, will match but "Hello World" will not match.

.. code-block::

    greeting = !"Hello" i"hello world";

Repeat
------

Repeat **matches the sub-expression several times**.

`+` match string one or more times.

.. code-block::

    number = [0-9]+;

`*` match string zero or more times.

.. code-block::

    number = [0-9] [1-9]*;

`?` match string one or more times.

.. code-block::

    number = [0-9] "."?;

`{min,}` match string minimum `min` times.

.. code-block::

    above_hundred = [1-9] [1-9]{2,};

`{,max}` match string maximum `max` times.

.. code-block::

   below_thousand = [0-9]{,3};

`{min,max}` match string minimum `min` times, maximum `max` times.

.. code-block::

   hex = "\u{" ([0-9] / [a-z] / [A-Z]){1,6} "}";

Comment
-------

Comment are any characters followed by a # (included) in a line.

.. code-block::

   # THIS IS A COMMENT.
   rule = "hello"; # THIS IS ANOTHER COMMENT.

Comments are ignored.

Grammar Rule Flags
------------------

The grammar rule allows setting flags by inserting some `@decorator` (s) before the names.
The supported decorators include: `@spaced`, `@squashed`, `@scoped`, `@tight`, `@lifted` and `@nonterminal`.
For example,

.. code-block::

    @spaced @lifted
    ws = " " / "\t" / "\n";

:seealso: :c:enum:`P4_FLAG_SPACED`, :c:enum:`P4_FLAG_SQUASHED`, :c:enum:`P4_FLAG_SCOPED`, :c:enum:`P4_FLAG_TIGHT`, :c:enum:`P4_FLAG_LIFTED`, :c:enum:`P4_FLAG_NON_TERMINAL`.

@spaced
```````

If a rule has `@spaced` decorator, it will be auto-inserted in between every element of sequences and repetitions.

For example, my sequence can match "helloworld", "hello world", "hello  \t  \n world", etc.

.. code-block::

    my_sequence = "hello" "world";

    @spaced
    ws = " " / "\t" / "\n";

@tight
```````

If a sequence or repetition rule has `@tight` decorator, no `@spaced` rules will be applied.

For example, my_another_sequence can only match "helloworld".

.. code-block::

    my_another_sequence = "hello" "world";

    @spaced
    ws = " " / "\t" / "\n";

@lifted
```````

If a rule has `@lifted` decorator, its children will replace the parent.

In this example, the parsed tree has no node mapping to primary rule, but rather either digit or char.

.. code-block::

    @lifted
    primary = digit / char;

    number = [0-9];
    char   = [a-z] / [A-Z];

@nonterminal
````````````

If a rule has `nonterminal` decorator, and it has only one single child node, the child node will replace the parent node.

If it produces multiple children, this decorator has no effect.

In this example,

.. code-block::

    @lifted
    add = number ("+" number)?;

    number = [0-9];

If we feed the input "1", the tree is like:

.. code-block::

    Number(0,1)

If we feed the input "1+1", the tree is like:

.. code-block::

    Add(0,3)
        Number(0,1)
        Number(1,3)

@squashed
`````````

If a rule has `@squashed` decorator, its children will be trimmed.

In this example, the rule `float` will drop all `number` nodes, leaving only one single node in the ast.

.. code-block::

    @squashed
    float = number ("." number)?;

    number = [0-9];


Use Peg API
------------

Function :c:func:`P4_LoadGrammar` can load a grammar from a string.

.. code-block::

    P4_Grammar* grammar = P4_LoadGrammar(
        "add = int + int;"

        "@squashed @tight "
        "int = [0-9]+;"

        "@spaced @lifted "
        "ws  = \" \";";
    );

The one-statement code is somewhat equivalent to the below code written in low-level C API:

.. code-block::

    P4_Grammar* grammar = P4_CreateGrammar();

    if (P4_Ok != P4_AddSequenceWithMembers(grammar, RuleAdd, 3,
        P4_CreateReference(RuleInt),
        P4_CreateLiteral("+", true),
        P4_CreateReference(RuleInt)
    ))
        goto finalize;

    if (P4_Ok != P4_AddOnceOrMore(grammar, RuleInt, P4_CreateRange('0', '9', 1)))
        goto finalize;
    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, RuleInt, P4_FLAG_SQUASHED|P4_FLAG_TIGHT))
        goto finalize;

    if (P4_Ok != P4_AddLiteral(grammar, RuleWs, " ", true))
        goto finalize;
    if (P4_Ok != P4_SetGrammarRuleFlag(grammar, RuleWs, P4_FLAG_SPACED|P4_FLAG_LIFTED))
        goto finalize;

Cheatsheet
----------

.. list-table:: Cheatsheet
   :header-rows: 1

   * - Syntax
     - Meaning
   * - `foo = ...;`
     - grammar rule
   * - `@lifted foo = ...;`
     - drop node
   * - `@spaced foo = ...;`
     - mark as space
   * - `@squashed foo = ...;`
     - ignore children
   * - `@tight foo = ...;`
     - ignore spaced rules
   * - `@non_terminal foo = ...;`
     - ignore single child node
   * - `@scoped foo = ...;`
     - cancle effects
   * - `"literal"`
     - exact match
   * - `"\x0d\x0a"`
     - exact match by using ascii digits
   * - `"\u4f60\u597D"`
     - exact match utf-8 characters
   * - `i"literal"`
     - case-insensitive match
   * - `[a-z]`
     - range
   * - `[0-9..2]`
     - range with stride
   * - `[\\u0001-\\U0010ffff]`
     - range using unicode runes
   * - `[\\p{L}]`
     - range using unicode categories
   * - `.`
     - any character
   * - `foo bar`
     - sequence
   * - `foo / bar`
     - choice
   * - `&foo`
     - positive
   * - `!foo`
     - negative
   * - `foo*`
     - zero or more
   * - `foo+`
     - once or more
   * - `foo?`
     - optional
   * - `foo{m,}`
     - repeat at least m times
   * - `foo{,n}`
     - repeat at most n times
   * - `foo{m,n}`
     - repeat between m-n times
   * - `foo{m}`
     - repeat exact n times
   * - `# IGNORE`
     - comment
