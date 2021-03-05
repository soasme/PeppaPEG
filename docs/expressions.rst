Expressions
===========

Literal
-------

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
-----

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

:seealso: :c:enum:`P4_Range`, :c:func:`P4_CreateRange`, :c:func:`P4_AddRange`.

Sequence
--------

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
-------------

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
------

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
---------

Reference **matches a string based on the referenced grammar rule**.

A grammar includes a set of grammar rules.  Each grammar rule is built from :c:struct:`P4_Expression` and is associated with an id.  A grammar rule can then be referenced in other grammar rules.

.. code-block:: c

    typedef enum { Entry, Text, ... };

    P4_AddLiteral(grammar, Text, "Hello,WORLD", true);

    P4_Expression* expr = P4_CreateReference(Text);

The referenced grammar rule must exist before calling :c:func:`P4_Parse`.

:seealso: :c:enum:`P4_Reference`, :c:func:`P4_CreateReference`, :c:func:`P4_AddReference`.

Positive
--------

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
--------

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
------

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
