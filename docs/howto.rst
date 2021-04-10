How-To Guides
=============

How to Prevent StackError?
--------------------------

Considering a json nested array: `[[[[...]]]]`. If the depth is too deep, Peppa PEG may end up with a StackError. This is due to Peppa PEG is a recursion-based parser. To prevent the stack address exhausts, one can set a limit on the recursion depth. By default, the depth is 8192.

If you need to adjust the depth, try :c:func:`P4_SetRecursionLimit`.

.. code-block:: c

    >> P4_SetRecursionLimit(grammar, 1000);
    P4_Ok

    >> P4_GetRecursionLimit(grammar);
    1000

:seealso: :c:func:`P4_SetRecursionLimit`.

How to Parse Substring?
-----------------------

Peppa PEG supports parsing a subset of source input. By default, all source content are attempted.

One can use `P4_SetSourceSlice` to set the start position and stop position in the source input.

In this example, Peppa PEG only parses the substring "YXXXY"[1:4], e.g. "XXX".

.. code-block:: c

    >> P4_Source* source = P4_CreateSource("YXXXY", ENTRY);
    >> P4_SetSourceSlice(source, 1, 4);
    P4_Ok
    >> P4_Parse(grammar, source);
    P4_Ok

:seealso: :c:func:`P4_SetSourceSlice`.

How to Prevent Partial Parse?
-----------------------------

Considering below case,

.. code-block:: c

    P4_Grammar* grammar = P4_CreateGrammar();
    assert(P4_Ok == P4_AddZeroOrMore(grammar, 1, P4_AddLiteral("a", true)));

    P4_Source* source = P4_CreateSource("aaab", 1);
    P4_Parse(grammar, source); // P4_Ok!

Guess what, :c:func:`P4_Parse` returns :c:enum:`P4_Ok`! Peppa PEG eats 3 "a" characters and ignores the rest of the input.

The expression of Start-of-Input and End-of-Input match the start and end of the input. They don't consume text.

To make "aaab" as a whole, we need to add Start-of-Input and End-of-Input before and after the ZeroOrMore rule:

If you use using PEG API, use `&.` and `!.`:
.. code-block::

    grammar = &. a*  !. ;

If you are using low level API, use :c:func:`P4_CreateStartOfInput` and :c:func:`P4_CreateEndOfInput`.

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

:seealso: :c:func:`P4_CreateStartOfInput`, :c:func:`P4_CreateEndOfInput`.

How to Join Expressions by Separators?
--------------------------------------

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

How to Replace Malloc/Free/Realloc?
-----------------------------------

You may choose your own memory management solution by replacing macros `P4_MALLOC`, `P4_FREE` and `P4_REALLOC`.

Say you want to replace stdlib malloc/free/realloc with bdwgc `GC_*`, you can define the above macros before including "peppapeg.h":

.. code-block::

    # include "gc.h"

    # define P4_MALLOC GC_MALLOC
    # define P4_FREE
    # define P4_REALLOC GC_REALLOC

    # define "peppapeg.h"

How to Transfer the Ownership of Source AST?
--------------------------------------------

You may transfer the ownership of AST out of the source object using :c:func:`P4_AcquireSourceAst`.

Say you want to get AST while do not want to keep track of the source, you can:

.. code-block::

    P4_Token* root = P4_AcquireSourceAst(source);
    P4_DeleteSource(source);

    // do something.
    P4_DeleteToken(root);
