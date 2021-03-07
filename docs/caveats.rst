Caveats
=======

Recursion Limit
---------------

Peppa PEG is a recursion-based parser so it will be crashed if the stack address is exhausted. To prevent that, Peppa PEG set a limit on the recursion depth. By default, it's 8192.

If Peppa PEG should run a very small embedded device, you can adjust it via `P4_SetRecursionLimit`.

.. code-block:: c

    >> P4_SetRecursionLimit(grammar, 1000);
    P4_Ok

    >> P4_GetRecursionLimit(grammar);
    1000

Start-of-Input & End-of-Input
-----------------------------

The expression of Start-of-Input and End-of-Input match the start and end of the input. They don't consume text.

Considering below case,

.. code-block:: c

    P4_Grammar* grammar = P4_CreateGrammar();
    assert(P4_Ok == P4_AddZeroOrMore(grammar, 1, P4_AddLiteral("a", true)));

    P4_Source* source = P4_CreateSource("aaab", 1);
    P4_Parse(grammar, source); // P4_Ok!

Guess what, :c:func:`P4_Parse` returns :c:enum:`P4_Ok`! Peppa PEG eats 3 "a" characters and ignores the rest of the input.

To make "aaab" as a whole, we need to add Start-of-Input and End-of-Input before and after the ZeroOrMore rule:

.. code-block:: c

    P4_Grammar* grammar = P4_CreateGrammar();
    assert(P4_Ok == P4_AddSequenceWithMembers(grammar, 1, 3,
        P4_CreateStartOfInput(),
        P4_CreateZeroOrMore(P4_AddLiteral("a", true)),
        P4_CreateEndOfInput()
    ));
    assert(P4_Ok == P4_AddZeroOrMore(grammar, 1, P4_AddLiteral("a", true)));

    P4_Source* source = P4_CreateSource("aaab", 1);
    P4_Parse(grammar, source); // P4_MatchError

Join
-----

Joining a rule by a separator is a common use, such as `f(p1, p2, p3)`, `[1, 2, 3]`. Peppa PEG provides a sugar to make it easier to match such a pattern.

For example, let's match `1,2,3`:

.. code-block:: c

    # define ROW 1
    # define NUM 2
    P4_Grammar* grammar = P4_CreateGrammar();

    // Or: P4_AddGrammarRule(grammar, ROW, P4_CreateJoin(",", NUM))
    assert(P4_Ok == P4_AddJoin(grammar, ROW, ",", NUM));

    assert(P4_Ok == P4_AddRange(grammar, NUM, '0', '9', 1));

When parsing `1,2,3`, it will produce such a data structure:

.. code-block::

    Token(0..5, ROW):
        Token(0..1, NUM)
        Token(2..3, NUM)
        Token(4..5, NUM)

The separator will not have its corresponding token, while all joined members have their tokens.
