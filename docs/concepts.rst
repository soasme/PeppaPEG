Concepts
========

What's a Parser?
----------------

A parser is usually part of a compiler. Considering a statement `print("hello world")`, a parser builds an abstract syntax tree (AST) like:

.. code-block::

    Node(rule_id=RuleStatement, slice=[0,20], children=[
        Node(rule_id=RuleFunction, slice=[0, 5]),
        Node(rule_id=RuleFunctionArgs, slice=[7, 19], children=[
            Node(rule_id=RuleString, slice=[7, 19])
        ])
    ])

Some parser has an additional tokenizing step before building an AST, while PEG parser can directly build such an AST from the input.

The compiler takes over the rest and compiles the AST to either bytecode for interpreter language or assembly code for compiled languages.

But that's far beyond what we're discussing here. As a parser, we only care about how to build an AST from a string input.

A parser is code, a lot of code. You can use Peppa PEG, Lex/Yacc, flex/bison, antlr, or even parse from scratch.


What's a Grammar?
-----------------

A grammar is a set of RULES instructing a parser how to parse from the input, e.g, it is definition of parsing. For example:

.. code-block::

   term = factor ((plus/minus) factor)*;
   factor = number ((mul/div) number);
   number = [0-9] / [1-9] [0-9]+;

A parser generator can read a grammar in such a (or similar) notion and produce a parser. The produced parser then reads input and produces AST.

Peppa PEG provides its own dialect of PEG for describing a grammar. It was inspired by `Pest <https://pest.rs>`_ but is more powerful, faster, and more readable.

See :ref:`peg` for a full specification.

Overview of Process
--------------------

1. Load Grammar. Load your grammar using :c:func:`P4_LoadGrammar` . Alternatively, you can create your grammar using :c:func:`P4_CreateGrammar` and then add grammar rules using :ref:`lowlevel`.
2. Parse Source. Wrap source string using :c:func:`P4_CreateSource` and then parse it using :c:func:`P4_Parse`.
3. Traverse AST. It's up to you what to do when traversing the tree. You may inspect the tree, produce bytecodes, trim the tree, etc.

Unlike one of Peppa PEG alternatives - peg/leg, Peppa PEG doesn't end up with producing C code for the grammar. It holds the grammar and the parser generator in memory so you don't need an extra compilation step.

Parse Tree v/s Abstract Syntax Tree
-----------------------------------

Parse Tree and Abstract Syntax Tree are often used interchangeably but they're different. Both are trees. But Parse Tree is closer to the source, while Abstract Syntax Tree is closer to the actual meaning of code.

Considering `[1,2]`, a parse tree looks like:

.. code-block::

    Node(rule_id=RuleArray, slice=[0,5], children=[
        Node(rule_id=RuleLeftParen, slice=[0,1]),
        Node(rule_id=RuleNumber, slice=[1,2]),
        Node(rule_id=RuleComma, slice=[2,3]),
        Node(rule_id=RuleNumber, slice=[3,4]),
        Node(rule_id=RuleRightParen, slice=[4,5]),
    ])

While an abstract syntax tree looks like:

.. code-block::

    Node(rule_id=RuleArray, slice=[0,5], children=[
        Node(rule_id=RuleNumber, slice=[1,2]),
        Node(rule_id=RuleNumber, slice=[3,4]),
    ])

e.g, left paren, comma, right paren are ignored as they're not part of the semantic of array.

You can consider abstract syntax tree as a polished version of parse tree. 

Peppa PEG provides some decorators allowing you to dropping some tokens; it also allows you reshaping part of the tree by setting callbacks.
