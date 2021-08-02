.. _lowlevel:

Low Level APIs
==============

Expressions
-----------

Literal
```````

Literal **matches an exact string**.

.. code-block:: c

    P4_Expression* expr = P4_CreateLiteral("Hello world", true);

Literal can be case insensitive for ASCII characters, if the option sensitive is false.

.. code-block:: c

    P4_Expression* expr = P4_CreateLiteral("HELLO WORLD", false);

Literal can be in UTF-8 format.

.. code-block:: c

    P4_Expression* expr = P4_CreateLiteral("你好, 世界", true);

Literal can include emoji characters.

.. code-block:: c

    P4_Expression* expr = P4_CreateLiteral("Peppa 🐷", false);

Literal cannot be an empty string.

.. code-block:: c

    // WRONG!
    P4_Expression* expr = P4_CreateLiteral("", true);

:seealso: :c:enum:`P4_Literal`, :c:func:`P4_CreateLiteral`, :c:func:`P4_AddLiteral`.

Range
`````

Range **matches a single character in range**.

.. code-block:: c

    P4_Expression* expr = P4_CreateRange('0', '9', 1);

The lower and upper character of range can be ASCII characters or UTF-8 runes.

.. code-block:: c

    P4_Expression* expr = P4_CreateRange(0x4E00, 0x9FFF, 1);

Range can be used to match anything (0, or '\0', or EOF is used for terminating a string in C):

.. code-block:: c

    P4_Expression* expr = P4_CreateRange(1, 0x10ffff, 1);

The stride parameter allows larger steps when iterating characters in range.

.. code-block:: c

    // Only match 1, 3, 5, 7, 9.
    P4_Expression* odd = P4_CreateRange('1', '9', 2);

The value of lower must be less than the upper.

.. code-block:: c

    // WRONG!
    P4_Expression* expr = P4_CreateRange('z', 'a', 1);

Range can be an aggregation of several sub-ranges. The below example is
equivalent to a choice of 3 ranges.

.. code-block:: c

    P4_RuneRange alphadigits[] = {{'a', 'z', 1}, {'A', 'Z', 1}, {'0', '9', 1}};
    P4_Expression* expr = P4_CreateRanges(3, alphadigits);

:seealso: :c:enum:`P4_Range`, :c:func:`P4_CreateRange`, :c:func:`P4_AddRange`,
          :c:func:`P4_CreateRanges`, :c:func:`P4_AddRanges`.

Sequence
````````

Sequence **matches a sequence of sub-expressions in order**.

.. code-block:: c

    P4_Expression* expr = P4_CreateSequenceWithMembers(3,
        P4_CreateLiteral("Hello", true),
        P4_CreateLiteral(",", true),
        P4_CreateLiteral("World", false)
    );

When parsing, the first sequence member is attempted. If succeeds, the second is attempted, so on and on.
If any one of the attempts fails, the match fails.

.. code-block::

    Hello,WORLD
    _____ Literal "Hello", success!
         _ Literal ",", success!
          _____ Insensitive Literal "World", success!
    ___________ Sequence: success!

    Hello,UNIVERSE
    _____ Literal "Hello", success!
         _ Literal ",", success!
          _____ Insensitive Literal "World", failure!
    Sequence: failure!

The members can be set after the Sequence is created:

.. code-block:: c

    P4_Expression* expr = P4_CreateSequence(3);

    if (expr == NULL) goto oom;

    if (P4_SetMember(expr, 0, P4_CreateLiteral("Hello", true)) != P4_Ok) goto oom;
    if (P4_SetMember(expr, 1, P4_CreateLiteral(",", true)) != P4_Ok) goto oom;
    if (P4_SetMember(expr, 2, P4_CreateLiteral("World", false)) != P4_Ok) goto oom;

    oom: P4_DeleteExpression(expr);

:seealso: :c:enum:`P4_Sequence`, :c:func:`P4_CreateSequence`, :c:func:`P4_CreateSequenceWithMembers`, :c:func:`P4_AddSequence`, :c:func:`P4_AddSequenceWithMembers`, :c:func:`P4_SetMember`.

BackReference
`````````````

BackReference **matches an exact string previously matched in a Sequence**. BackReference can and only can be used as a Sequence member. For example, the below snippet matches "a:a", but not "a:A" or "a:b".

.. code-block:: c

    P4_Expression* expr = P4_CreateSequenceWithMembers(3,
        P4_CreateRange('a', 'z', 1);
        P4_CreateLiteral(":", true),
        P4_CreateBackReference(0, true)
    );

The BackReference can be case insensitive, regardless whether the original match was case sensitive. For example, the below snippet matches "a:a" and "a:A".

.. code-block:: c

    P4_Expression* expr = P4_CreateSequenceWithMembers(3,
        P4_CreateRange('a', 'z', 1);
        P4_CreateLiteral(":", true),
        P4_CreateBackReference(0, false)
    );

The index value of a BackReference must be less than the total number of members in a Sequence.

.. code-block:: c

    // WRONG!
    P4_Expression* expr = P4_CreateSequenceWithMembers(3,
        P4_CreateLiteral("a", true),
        P4_CreateLiteral(":", true),
        P4_CreateBackReference(3, true)
    );

The index value of a BackReference must not be the index of itself.

.. code-block:: c

    // WRONG!
    P4_Expression* expr = P4_CreateSequenceWithMembers(3,
        P4_CreateLiteral("a", true),
        P4_CreateLiteral(":", true),
        P4_CreateBackReference(2, true)
    );

:seealso: :c:enum:`P4_BackReference`, :c:func:`P4_CreateBackReference`.

Choice
```````

Choice **matches one of the sub-expression.**

.. code-block:: c

    P4_Expression* expr = P4_CreateChoiceWithMembers(3,
        P4_CreateLiteral("Hello", true),
        P4_CreateLiteral("Kia Ora", true),
        P4_CreateLiteral("你好", false)
    );

When parsing, the first sequence member is attempted. If fails, the second is attempted, so on and on.
If any one of the attempts succeeds, the match succeeds. If all attempts fail, the match fails.

.. code-block::

    你好
    Literal "Hello", failure!
    Literal "Kia Ora", failure|
    ____ Literal "你好", success!
    ____ Choice: success!

    Ciao
    Literal "Hello", failure!
    Literal "Kia Ora", failure|
    Literal "你好", failure!
    Choice: failure!

Similar to Sequence, the members can be set after the Choice is created.

.. code-block:: c

    P4_Expression* expr = P4_CreateChoice(3);

    if (expr == NULL) goto oom;

    if (P4_SetMember(expr, 0, P4_CreateLiteral("Hello", true)) != P4_Ok) goto oom;
    if (P4_SetMember(expr, 1, P4_CreateLiteral("Kia Ora", true)) != P4_Ok) goto oom;
    if (P4_SetMember(expr, 2, P4_CreateLiteral("你好", true)) != P4_Ok) goto oom;

    oom: P4_DeleteExpression(expr);

:seealso: :c:enum:`P4_Choice`, :c:func:`P4_CreateChoice`, :c:func:`P4_CreateChoiceWithMembers`, :c:func:`P4_AddChoice`, :c:func:`P4_AddChoiceWithMembers`, :c:func:`P4_SetMember`.

Reference
`````````

Reference **matches a string based on the referenced grammar rule**.

A grammar includes a set of grammar rules.  Each grammar rule is built from :c:struct:`P4_Expression` and is associated with an id.  A grammar rule can then be referenced in other grammar rules.

.. code-block:: c

    typedef enum { Entry, Text, ... };

    P4_AddLiteral(grammar, Text, "Hello,WORLD", true);

    P4_Expression* expr = P4_CreateReference(Text);

The referenced grammar rule must exist before calling :c:func:`P4_Parse`.

:seealso: :c:enum:`P4_Reference`, :c:func:`P4_CreateReference`, :c:func:`P4_AddReference`.

Positive
````````

Positive **tests if the sub-expression matches**.

.. code-block:: c

    P4_Expression* expr = P4_CreatePositive(P4_CreateLiteral("Hello", true));

Positive attempts to match the sub-expression. If succeeds, the test passes. Positive does not "consume" any text.

Positive can be useful in limiting the possibilities of the latter member in a Sequence. In this example, the Sequence expression must start with "Hello", e.g. "Hello World", "Hello WORLD", "Hello world", etc, will match but "HELLO WORLD" will not match.

.. code-block:: c

    P4_Expression* expr = P4_CreateSequenceWithMembers(2,
        P4_CreatePositive(
            P4_CreateLiteral("Hello", true)
        );
        P4_CreateLiteral("Hello World", false)
    );

:seealso: :c:enum:`P4_Positive`, :c:func:`P4_CreatePositive`, :c:func:`P4_AddPositive`.

Negative
````````

Negative **tests if the sub-expression does not match**.

.. code-block:: c

    P4_Expression* expr = P4_CreateNegative(P4_CreateLiteral("Hello", true));

Negative expects the sub-expression doesn't match. If fails, the test passes. Negative does not "consume" any text.

Negative can be useful in limiting the possiblities of the latter member in a Sequence. In this example, the Sequence expression must not start with "Hello", e.g. "HELLO World", "hello WORLD", "hello world", etc, will match but "Hello World" will not match.

.. code-block:: c

    P4_Expression* expr = P4_CreateSequenceWithMembers(2,
        P4_CreateNegative(
            P4_CreateLiteral("Hello", true)
        );
        P4_CreateLiteral("Hello World", false)
    );

:seealso: :c:enum:`P4_Negative`, :c:func:`P4_CreateNegative`, :c:func:`P4_AddNegative`.

Repeat
`````````

Repeat **matches the sub-expression several times**.

ZeroOrOnce, ZeroOrMore, OnceOrMore consume zero or one , zero or more, or one or more consecutive repetitions of their sub-expression.

.. code-block:: c

    P4_Expression* expr = P4_CreateZeroOrOnce(P4_CreateLiteral("Hello", true));

.. code-block:: c

    P4_Expression* expr = P4_CreateZeroOrMore(P4_CreateLiteral("Hello", true));

.. code-block:: c

    P4_Expression* expr = P4_CreateOnceOrMore(P4_CreateLiteral("Hello", true));

ZeroOrOnce and ZeroOrMore always succeeds because it allows matching zero times.

The repetition can also be set with designated min or max times.

.. code-block:: c

    P4_Expression* expr = P4_CreateRepeatMin(P4_CreateLiteral("Hello", true), 3);

.. code-block:: c

    P4_Expression* expr = P4_CreateRepeatMax(P4_CreateLiteral("Hello", true), 3);

.. code-block:: c

    P4_Expression* expr = P4_CreateRepeatExact(P4_CreateLiteral("Hello", true), 3);

.. code-block:: c

    P4_Expression* expr = P4_CreateRepeatMinMax(P4_CreateLiteral("Hello", true), 1, 3);

.. note::

    All Repeat expressions can be rewritten with P4_CreateRepeatMinMax.

    * ZeroOrOnce: P4_CreateRepeatMinMax(expr, 0, 1);
    * ZeroOrMore: P4_CreateRepeatMinMax(expr, 0, SIZE_MAX);
    * OnceOrMore: P4_CreateRepeatMinMax(expr, 1, SIZE_MAX);
    * RepeatMin: P4_CreateRepeatMinMax(expr, min, SIZE_MAX);
    * RepeatMax: P4_CreateRepeatMinMax(expr, 0, max);
    * RepeatExact: P4_CreateRepeatMinMax(expr, n, n);

    However, using the derived names can improve the readability of the code.

:seealso: :c:enum:`P4_Repeat`, :c:func:`P4_CreateZeroOrOnce`, :c:func:`P4_CreateZeroOrMore`, :c:func:`P4_CreateOnceOrMore`, :c:func:`P4_CreateRepeatMin`, :c:func:`P4_CreateRepeatMax`, :c:func:`P4_CreateRepeatMinMax`, :c:func:`P4_CreateRepeatExact`, :c:func:`P4_AddZeroOrOnce`, :c:func:`P4_AddZeroOrMore`, :c:func:`P4_AddOnceOrMore`, :c:func:`P4_AddRepeatMin`, :c:func:`P4_AddRepeatMax`, :c:func:`P4_AddRepeatMinMax`, :c:func:`P4_AddRepeatExact`.

Expression Flags
----------------

Flags can only be used in the grammar rule expression itself. It can not be used in any sub-expression of the grammar rule expression.

Usually, a grammar rule creates a node. The expression flags modify the behavior of the node generation.

`P4_FLAG_SQUASHED`
``````````````````

Flag `P4_FLAG_SQUASHED` **prevents generating children nodes**.

For example,

.. code-block:: c

    P4_AddSequenceWithMembers(grammar, Entry, 2
        P4_CreateReference(Text),
        P4_CreateReference(Text)
    );
    P4_AddLiteral(grammar, Text, "x", false);

This grammar parses the text "xx" into three nodes:

.. code-block::

    Token(0..2): "Xx"
      Token(0..1) "X"
      Token(1..2) "x"

If the grammar rule `Entry` has flag `P4_FLAG_SQUASHED`, the children nodes disappear:

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_SQUASHED);

.. code-block::

    Token(0..2): "Xx"

Flag `P4_FLAG_SQUASHED` takes effects not only on the expression but its all descendant expressions.

`P4_FLAG_LIFTED`
``````````````````

`P4_FLAG_LIFTED` **replaces the node with its children nodes**.

For example,

.. code-block:: c

    typedef enum { Entry, Text } MyRuleID;

    P4_AddSequenceWithMembers(grammar, Entry, 2
        P4_CreateReference(Text),
        P4_CreateReference(Text)
    );
    P4_AddLiteral(grammar, Text, "x", false);

This grammar can match text "xx" into three nodes:

.. code-block::

    Token(0..2): "Xx"
      Token(0..1) "X"
      Token(1..2) "x"

If the grammar rule `Entry` has flag `P4_FLAG_LIFTED`, the node is lifted and replaced by its children:

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_LIFTED);

.. code-block::

    Token(0..1): "X"
    Token(1..2): "x"

`P4_FLAG_NON_TERMINAL`
``````````````````````

`P4_FLAG_NON_TERMINAL` **replaces the node with its single child node or does nothing.**.

For example,

.. code-block:: c

    P4_AddSequenceWithMembers(grammar, Entry, 2
        P4_CreateLiteral("(", true),
        P4_CreateReference(Text),
        P4_CreateLiteral(")", true)
    );
    P4_AddLiteral(grammar, Text, "x", false);

This grammar can match text "(x)" into two nodes:

.. code-block::

    Token(0..3): "(x)"
      Token(1..2) "x"

If the grammar rule `Entry` has flag `P4_FLAG_NON_TERMINAL`, the node is lifted and replaced by its single child node:

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_NON_TERMINAL);

.. code-block::

    Token(1..2): "x"

This flag only works for Sequence and Repeat expressions.

This flag has no effect if the Sequence or Repeat expressions produces over one node, e.g, the parent node will not be lifted.

`P4_FLAG_SPACED`
``````````````````````

`P4_FLAG_SPACED` **indicates the expression is for whitespaces**.

For example,

.. code-block:: c

    typedef enum { Entry, Whitespace } MyRuleID;

    P4_AddLiteral(grammar, Whitespace, " ", false);

    P4_SetGrammarRuleFlag(grammar, Whitespace, P4_FLAG_SPACED);

Often, we don't want the whitespace having nodes, so it's usually combined with `P4_FLAG_LIFTED`.

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Whitespace, P4_FLAG_SPACED | P4_FLAG_LIFTED);

This flag does not work on its own. It takes effect on Sequence or Repeat.

When parsing Sequence and Repeat, the grammar will match as many whitespaces as possible between every sequence member or every repetition sub-expression.

For example, this rule matches "HelloWorld", "Hello World", "Hello       World", etc.

.. code-block:: c

    P4_AddSequenceWithMembers(grammar, Entry, 2,
        P4_AddLiteral("Hello", true),
        P4_AddLiteral("World", true)
    );

For example, this rule matches "xxx", "x   x         x", etc.

.. code-block:: c

    P4_AddOnceOrMore(grammar, Entry, P4_AddLiteral("x", true));

The SPACED expressions are not inserted before or after the Sequence and Repeat, hence "  Hello World  ", "  xxx  " not matching.


`P4_FLAG_TIGHT`
``````````````````````

`P4_FLAG_TIGHT` **prevents inserting the `P4_FLAG_SPACED` expressions**. This tag only works for Sequence and Repeat.

Given the above `P4_FLAG_SPACED` expression, if we set the grammar rule with flag `P4_FLAG_TIGHT`, the SPACED expressions are not inserted.

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_TIGHT);

Peppa PEG applies SPACED expressions on every Sequence and Repeat unless a `P4_FLAG_TIGHT` is explicitly specified on a Sequence or Repeat.

Flag `P4_FLAG_TIGHT` takes effects not only on the expression but its all descendant expressions.

`P4_FLAG_SCOPED`
``````````````````````

`P4_FLAG_SCOPED` **prevents the effect of `P4_FLAG_SQUASHED` and `P4_FLAG_SPACED`**.

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_SCOPED);

Starting from the SCOPED grammar rule, the nodes are not squashed; the implicit whitespace is enabled as well.
