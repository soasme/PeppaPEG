Expression Flags
================

Flags can only be used in the grammar rule expression itself. It can not be used in any sub-expression of the grammar rule expression.

Usually, a grammar rule creates a token. The expression flags modify the behavior of the token generation.

`P4_FLAG_SQUASHED`
------------------

Flag `P4_FLAG_SQUASHED` **prevents generating children tokens**.

For example,

.. code-block:: c

    typedef enum { Entry, Text } MyRuleID;

    P4_AddSequenceWithMembers(grammar, Entry, 2
        P4_CreateReference(Text),
        P4_CreateReference(Text)
    );
    P4_AddLiteral(grammar, Text, "x", false);

This grammar parses the text "xx" into three tokens:

.. code-block::

    Token(0..2): "Xx"
      Token(0..1) "X"
      Token(1..2) "x"

If the grammar rule `Entry` has flag `P4_FLAG_SQUASHED`, the children tokens disappear:

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_SQUASHED);

.. code-block::

    Token(0..2): "Xx"

Flag `P4_FLAG_SQUASHED` takes effects not only on the expression but its all descendant expressions.

`P4_FLAG_LIFTED`
----------------

`P4_FLAG_LIFTED` **replaces the token with its children tokens**.

For example,

.. code-block:: c

    typedef enum { Entry, Text } MyRuleID;

    P4_AddSequenceWithMembers(grammar, Entry, 2
        P4_CreateReference(Text),
        P4_CreateReference(Text)
    );
    P4_AddLiteral(grammar, Text, "x", false);

This grammar can match text "xx" into three tokens:

.. code-block::

    Token(0..2): "Xx"
      Token(0..1) "X"
      Token(1..2) "x"

If the grammar rule `Entry` has flag `P4_FLAG_LIFTED`, the token is lifted and replaced by its children:

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_LIFTED);

.. code-block::

    Token(0..1): "X"
    Token(1..2): "x"

`P4_FLAG_SPACED`
----------------

`P4_FLAG_SPACED` **indicates the expression is for whitespaces**.

For example,

.. code-block:: c

    typedef enum { Entry, Whitespace } MyRuleID;

    P4_AddLiteral(grammar, Whitespace, " ", false);

    P4_SetGrammarRuleFlag(grammar, Whitespace, P4_FLAG_SPACED);

Often, we don't want the whitespace having tokens, so it's usually combined with `P4_FLAG_LIFTED`.

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


.. note::

    Currently, Peppa PEG only supports at maximum two SPACED grammar rules.

`P4_FLAG_TIGHT`
----------------

`P4_FLAG_TIGHT` **prevents inserting the `P4_FLAG_SPACED` expressions**. This tag only works for Sequence and Repeat.

Given the above `P4_FLAG_SPACED` expression, if we set the grammar rule with flag `P4_FLAG_TIGHT`, the SPACED expressions are not inserted.

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_TIGHT);

Peppa PEG applies SPACED expressions on every Sequence and Repeat unless a `P4_FLAG_TIGHT` is explicitly specified on a Sequence or Repeat.

Flag `P4_FLAG_TIGHT` takes effects not only on the expression but its all descendant expressions.

`P4_FLAG_SCOPED`
----------------

`P4_FLAG_SCOPED` **prevents the effect of `P4_FLAG_SQUASHED` and `P4_FLAG_SPACED`**.

.. code-block:: c

    P4_SetGrammarRuleFlag(grammar, Entry, P4_FLAG_SCOPED);

Starting from the SCOPED grammar rule, the token are not squashed; the implicit whitespaces are enabled as well.
