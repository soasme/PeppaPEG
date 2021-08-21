.. _spec:

Peppa PEG Specification
========================

Objectives
----------

Peppa PEG aims to be a PEG dialect that's easy to use. Peppa PEG is designed to describe a formal language by extending the original version of PEG with some already-commonly-used symbols and notations. Peppa PEG should be easy to parse source code into an abstract syntax tree for a wide variety of languages.

Spec
-----

* Peppa PEG is case-sensitive.
* A Peppa PEG grammar must be a valid UTF-8 encoded Unicode document.

Comment
-------

A hash symbol following by any characters til end of line marks a comment.

.. code-block::

    comment = "#" (!"\n" .)* "\n"?;

Comments are ignored and can be marked between rules.
This is a simple way of including additional information in parallel with the specifications.

For example,

.. code-block::

    # This is a full-line comment.

    rule = "# This is not a comment."; # This is a comment at the end of a line.

Rule Naming
------------

The name of a rule is a sequence of characters, beginning with an alphabet or underscore, and followed by a combination of alphabets, digits, or underscores (_).

.. code-block::

    name = ([a-z]/[A-Z]/"_") ([a-z]/[A-Z]/[0-9]/"_")*;

Rule names are case sensitive.

For example, the rule names listed below are valid:

* `rule`
* `DIGITS`
* `oct_int`

while these are invalid:

* `0to9`
* `ml-string`

Rule Form
---------

The primary building block of a Peppa PEG document is the rule.

A rule is defined by the following sequence:

.. code-block::

    rule = decorator* name "=" expression ";"

where one or more rule decorators may be prefixed before rule names, rule names are on the left-hand side of the equals sign and expressions are one the right-hand side. Rules always ends up with a semicolon (;). The key, equal sign, expressions, and semicolon can be broken over multiple lines. Whitespace is ignored around rule names and between expressions.

For example:

.. code-block::

    rule = rule2 / "expr4" / "expr5" / "expr6";

    @lifted
    rule2 = "expr1"
          / "expr2"
          / "expr3"
          ;

Unspecified expressions are invalid.

.. code-block::

    rule = ; # INVALID!

Rule Expressions
----------------

Rule expressions have the following precedence from highest to lowest:

* Primary
* Repeat
* Sequence
* Choice

Since choice has the lowest precedence, a rule expression always starts interpreting as a choice of one alternatives:

.. code-block::

    expression = choice;

Primary rule expressions must have one of the following types.

* Literal
* Insensitive
* Range
* Reference
* Back Reference
* Positive
* Negative
* Dot
* Cut
* Grouping

.. code-block::

    primary = literal
            / insensitive
            / range
            / (reference !"=")
            / back_reference
            / positive
            / negative
            / dot
            / cut
            / "(" choice ")"
            ;

Grouping notion () is strongly advised which will avoid misinterpretation by casual readers. For example,

.. code-block::

    foobar = (foo  / bar) / (goo / par);

Dot
---

Single dot `.` can match any Unicode character. It's a syntax sugar for [\\u0001-\\U0010ffff].

.. code-block::

    any = .;

Literal
-------

The literal matches an exact same string surrounded by double quotes.

For example,

.. code-block::

    greeting = "hello world";

Unicode is supported:

.. code-block::

    greeting = "你好，世界";

Emoji can be encoded via Unicode so it is supported:

.. code-block::

    greeting = "Peppa 🐷";

You can encode ASCII characters via \\x followed by 2 hex digits.

.. code-block::

    greeting = "\x48\x65\x6c\x6c\x6f, world";

You can encode Unicode characters via \\u followed by 4 hex digits or \\U followed by 8 hex digits.
The escape codes must be valid Unicode `scalar values <https://unicode.org/glossary/#unicode_scalar_value>`_.

.. code-block::

    greeting = "\u4f60\u597D, world\U0000000c";

Range
------

Range **matches a single character in range**. Ranges are enclosed by [ and ]. The full form is:

.. code-block::

    range = "[" (
        (char "-" char (".." number)?)
        / ("\p{" range_category "}")
    ) "]";

In the following example, any character between `'0'` to `'9'` can match.

.. code-block::

    digits = [0-9];

The lower and upper character of the range can be not only ASCII characters but also UTF-8 code points.
The syntax can be \\xXX, \\uXXXX or \\uXXXXXXXX.

.. code-block::

    digits = [\u4e00-\u9fff];

The value of lower must be less or equal than the upper.

.. code-block::

    any = [\U0010ffff-\u0001]; # INVALID!!!

Range supports an optional `stride` to skip certain amount of characters in the range.
In this example, only odd number between `'0'` to `'9'` can match.

.. code-block::

    digits = [0-9..2];

Range can support Unicode general categories and properties by wrapping filters with \\p{}, such as `C`, `Cc`, `Cf`, `Co`, `Cs`, `Ll`, `Lm`, `Lo`, `Lt`, `Lu`, `L`, `Nd`, `Nl`, `No`, `N`, `Id_Start`, `Id_Continue`, `Other_Id_Start`, `Other_Id_Continue`, `White space`, etc.

.. code-block::

    range_category = ([a-z] / [A-Z] / [0-9] / "_" / " ")+;

For example,

.. code-block::

    unicode_letter = [\p{L}];  # Any Unicode Letter (Ll, Lm, Lo, Lt, Lu).
    unicode_digit  = [\p{Nd}]; # Any Number, decimal digit (Nd).

Whether range category is supported depends on the implementations.

Sequence
--------

Sequence **matches a sequence of rules in order**, e.g. a concatenation of contiguous characters.

The full form is:

.. code-block::

    sequence = repeat+;

The first sequence element is attempted. If succeeds, the second is attempted, so on and on.
If any one of the attempts fails, the match fails.

For example:

.. code-block::

    greeter = "Hello" " " "world";

Elements enclosed in parentheses are considered as a single element. Thus,

.. code-block::

    rule = prefix (foo / bar) postfix;

matches either (prefix foo postfix) or (prefix bar postfix), and

.. code-block::

    rule = prefix foo / bar postfix;

matches either (prefix foo) or (bar postfix).

Choice
-------

Choice **matches one of the alternatives**.

Elements in the choice separated by a forward slash (/) are alternatives. The full form of choice is:

.. code-block::

   choice = sequence ("/" sequence)*;

The first alternative is attempted. If fails, the second is attempted, so on and on.
If any one of the attempts succeeds, the match succeeds. If all attempts fail, the match fails.

For example:

.. code-block::

   greeter = "Hello World" / "你好，世界" / "Kia Ora";

Reference
---------

Reference **matches a string based on the referenced grammar rule**. A reference has the same specification of rule name:

.. code-block::

    name = ([a-z] / [A-Z] / "_") ([a-z] / [A-Z] / [0-9] / "_")*;

For example, `greeter` is just a reference rule in `greeting`. When matching `greeting`, it will use the referenced grammar rule `greeter` first, e.g. `"Hello" / "你好"`, then match " world".

.. code-block::

    greeting = greeter " world";
    greeter  = "Hello" / "你好";

The order of defining a rule does not matter.

.. code-block::

    greeter  = "Hello" / "你好";
    greeting = greeter " world";

One should ensure all references must have corresponding rule defined, otherwise, the parse fails due to an undefined rule.

Back Reference
--------------

Back reference **matches an exact same string as previously matched in the sequence**.

Back reference starts with a back slash, followed by a number. The number is zero-based and cannot be a number greater than or equal to the index of itself. The number indicates which previous member in the sequence should be back referenced. The full form is:

.. code-block::

    back_reference = "\" number;

In the following example, \\0 matches whatever `quote` has matched, thus `"a"` or `'a'` are valid. But `"a'` or `'a"` are invalid.

.. code-block::

    str = quote [a-z] \0;
    quote = "\"" / "'";

Back reference applies only to the nearest sequence. Thus,

.. code-block::

    rule = "a" ("b" \0) \0;

matches "abba" since the first \\0 back reference "b" and the second \\0 back reference "a".

Insensitive
-----------

Insensitive operator starts with "i" and followed by a literal or back reference.

.. code-block::

    insensitive = "i" (literal / back_reference);

For example,

Given the following rule, back reference \\0 is case-insensitive. Hence, both `a=A` and `a=a` are valid.

.. code-block::

    rule = [a-z] "=" i\0;

Given the following rule, literal "hello world" is case-insensitive. Hence, both `ì` and `Ì` are valid.

.. code-block::

    rule = i"ì";

Positive
--------

Positive **tests if the sub-expression matches**. Positive starts with & and followed by a primary:

.. code-block::

    positive = "&" primary;

Positive attempts to match the sub-expression. If succeeds, the test passes. Positive does not "consume" any text.

Positive can be useful in limiting the possibilities of the latter member in a Sequence. In this example, the Sequence expression must start with "Hello", e.g. "Hello World", "Hello WORLD", "Hello world", etc, will match but "HELLO WORLD" will not match.

.. code-block::

    greeting = &"Hello" i"hello world";

Negative
--------

Negative **tests if the sub-expression does not match**. Negative starts with ! and followed by a primary:

.. code-block::

    negative = "!" primary;

Negative expects the sub-expression doesn't match. If fails, the test passes. Negative does not "consume" any text.

Negative can be useful in limiting the possiblities of the latter member in a Sequence. In this example, the Sequence expression must not start with "Hello", e.g. "HELLO World", "hello WORLD", "hello world", etc, will match but "Hello World" will not match.

.. code-block::

    greeting = !"Hello" i"hello world";

Repetition
----------

Operators `+`, `*`, `?` and `{}` followed by an expression indicates repetition.

The full form of repetition is:

.. code-block::

    repeat = primary (onceormore / zeroormore / zerooronce / repeatexact / repeatminmax / repeatmin / repeatmax)?;

1. Plus (`+`) matches string one or more times.

.. code-block::

    onceormore = "+";

For example,

.. code-block::

    number = [0-9]+;

2. Asterisk (`*`) matches string zero or more times.

.. code-block::

    zeroormore = "*";

For example,

.. code-block::

    number = [0-9] [1-9]*;

3. Question (`?`) matches string one or more times.

.. code-block::

    zerooronce = "?";

For example,

.. code-block::

    number = [0-9] "."?;

4. `{cnt}` matches exactly `cnt` occurrences of an expression, where cnt is a decimal value.

.. code-block::

    repeatexact = "{" number "}";

For example,

.. code-block::

   unicode = "\U" ([0-9] / [a-f] / [A-F]){8};

5. `{min,max}` matches an expression of at least `min` occurrences and at most `max` occurrences, where min and max are decimal values.

.. code-block::

    repeatminmax = "{" number "," number "}";

For example,

.. code-block::

   hex = "\u{" ([0-9] / [a-f] / [A-F]){1,6} "}";

6. `{min,}` matches an expression of at least `min` occurrences, where min is a decimal value.

.. code-block::

    repeatmin = "{" number "," "}";

For example,

.. code-block::

    above_hundred = [1-9] [1-9]{2,};

7. `{,max}` matches an expression of at most `max` occurrences, where max is a decimal value.

.. code-block::

    repeatmax = "{" "," number "}";

For example,

.. code-block::

   below_thousand = [0-9]{,3};

Cut
---

Cut is a decorator written as "@cut". It always succeeds, but cannot be backtracked.
It's used to prevent unwanted backtracking, e.g. to prevent excessive choice options.

Backtracking means if e1 in `rule = e1 / e2;` fails, the parser returns the last position where e1 started, and tries e2.
If there is a `@cut` in e1, any failure after the cutting point will cause rule failed immediately.
Cut ensures the parse sticks to the current rule, even if it fails to parse.
See ideas `1 <http://ceur-ws.org/Vol-1269/paper232.pdf>`_, `2 <https://news.ycombinator.com/item?id=20503245>`_.

Cut starts with @ and followed by "cut", e.g. "@cut".

For example, let's first consider the following grammar,

.. code-block::

    value = array / null;
    array = "[" "]";
    null  = "null";

Given input "[", it attempts matching array first. After failed, it will try null next. At last, value match is failed.

Let's add a cut operator:

.. code-block::

    value = array / null;
    array = "[" @cut "]";
    null  = "null";

Given input "[", it attempts matching array first. After failed, value match is failed immediately.

Given input "null", it attempts matching array first. It fails before `@cut` and then failed matching array. Parser then match "null" successfully.

Decorators
----------

Decorators are characters @ followed by some selected keywords.
Valid decorators include: `@spaced`, `@squashed`, `@scoped`, `@tight`, `@lifted` and `@nonterminal`.

.. code-block::

    decorator = "@" ("squashed" / "scoped" / "spaced" / "lifted" / "nonterminal");

For example,

.. code-block::

    @spaced @lifted
    ws = " " / "\t" / "\n";

@spaced
```````

A `@spaced` rule will be auto-inserted in elements of sequences and repetitions.

For example, my_sequence can match "helloworld", "hello world", "hello  \t  \n world", etc.

.. code-block::

    my_sequence = "hello" "world";

    @spaced
    ws = " " / "\t" / "\n";

Rule my_sequence is in essence equivalent to:

.. code-block::

    my_sequence = "hello" (" " / "\t" / "\n")* "world";

There may be multiple `@spaced` rules in the grammar. Thus,

.. code-block::

   my_sequence2 = "hello" "world";

   @spaced
   ws = " " / "\t" / "\n";

   @spaced
   dot = ".";

Rule my_sequence is equivalent to:

.. code-block::

    my_sequence2 = "hello" ((" " / "\t" / "\n") / ".")* "world";

The `@spaced` rules only take effects for rules having no `@tight` decorators.

@tight
```````

A `@tight` rule deters any `@spaced` rule from auto-inserted.

For example, my_sequence can only match "helloworld".

.. code-block::

    @tight
    my_sequence = "hello" "world";

    @spaced
    ws = " " / "\t" / "\n";

@lifted
```````

A `@lifted` rule replaces the node with its children in the parse tree, if exists any children.

For example,

.. code-block::

    rule = lit;

    @lifted
    lit = digit / char;

    number = [0-9]+;
    char   = ([a-z] / [A-Z])+;

given input "42", the parsed tree looks like:

.. code-block::

    {
        "type": "rule",
        "children": [
            {"type": "number", "text": "4"},
            {"type": "number", "text": "2"}
        ]
    }

This decorator is useful for trimming some unnecessary nodes in the parse tree.

@nonterminal
````````````

A `@nonterminal` rule replaces the node with its children in the parse tree, if exists one child.
If the rule produces multiple children, this decorator has no effect.

For example,

.. code-block::

    @nonterminal
    add = number ("+" number)?;

    number = [0-9];

given input "1", the tree is like:

.. code-block::

    {"type": "add", "text": "1"}

given input "1+2", the tree is like:

.. code-block::

    {
        "type": "add",
        "children": [
            {"type": "number", "text": "1"},
            {"type": "number", "text": "2"},
        ]
    }

@squashed
`````````

A `@squashed` rule drops all node children in the parse tree.

For example,

.. code-block::

    @squashed
    float = number ("." number)?;

    number = [0-9];

given input "1.0", rule `float` drops all `number` nodes, leaving only one single node in the ast:

.. code-block::

    {"type": "float", "text": "1.0"}

@scoped
```````

A `@scoped` rule reset all decorators that inherit from upstream rules.

For example, despite `greeting2` set to not using spaced rule `ws`, `greeting` can still apply to `ws` since it's under its own scope.

.. code-block::

    @tight
    greeting2 = greeting greeting;

    @scoped
    greeting = "hello" "world";

    @spaced
    ws = " ";

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
   * - \\0
     - back reference
   * - `&foo`
     - positive
   * - `!foo`
     - negative
   * - `@cut`
     - prevent unwanted backtracking
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
