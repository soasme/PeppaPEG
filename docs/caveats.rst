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
    P4_AddZeroOrMore(grammar, 1, P4_AddLiteral("a", true));

    P4_Source* source = P4_CreateSource("aaab", 1);
    P4_Parse(grammar, source);

Guess what, :c:func:`P4_Parse` returns :c:enum:`P4_Ok`! Peppa PEG eats 3 "a" characters and ignores the rest of the input.

To make "aaab" as a whole, we need to add Start-of-Input and End-of-Input before and after the ZeroOrMore rule:

.. code-block:: c

    P4_Grammar* grammar = P4_CreateGrammar();
    P4_AddSequenceWithMembers(grammar, 1, 3,
        P4_CreateStartOfInput(),
        P4_CreateZeroOrMore(P4_AddLiteral("a", true)),
        P4_CreateEndOfInput()
    );
    P4_AddZeroOrMore(grammar, 1, P4_AddLiteral("a", true));

    P4_Source* source = P4_CreateSource("aaab", 1);
    P4_Parse(grammar, source); // P4_MatchError
