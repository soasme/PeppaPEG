Concepts
========

What is a Parser?
-----------------

A parser is a program that takes string as input and produces a parse tree as output.

For example, given input `print("hello world")`, a parser can produce a tree like below:

.. code-block::

    Node(type="statement", slice=[0,20], children=[
        Node(type="func_call", slice=[0, 20], children=[
            Node(type="identifier", slice=[0, 5]),
            Node(type="func_args", slice=[7, 19], children=[
                Node(rule_id="string_literal", slice=[7, 19])
            ])
        ])
    ])

Some parsers have an additional step called "tokenize" before constructing the parse tree, while PEG parser can directly build the tree from the input.

You can use Peppa PEG, Lex/Yacc, flex/bison, antlr, or even parse from scratch.

What is a Grammar?
-----------------

A grammar is a set of RULES instructing a parser how to parse from the input, e.g, it is the definition of the parsing process.

For example, the grammar snippet below defines how to parse arithmetic expression.

.. code-block::

   term = factor (("+"/"-") factor)*;
   factor = number (("*"/"/") number);
   number = [0-9] / [1-9] [0-9]+;

A parser generator can read a grammar in such a (or similar) notion and produce a parser. The produced parser then reads input and produces AST.

Peppa PEG provides its own dialect of PEG for describing a grammar. It was inspired by `Pest <https://pest.rs>`_ but is more powerful and more readable.

See :ref:`peg` for all of the grammar rules you can use.

Parse Tree v/s Abstract Syntax Tree
-----------------------------------

Parse Tree and Abstract Syntax Tree are often used interchangeably but they're different. Both are trees. But Parse Tree is closer to the source, while Abstract Syntax Tree is closer to the semantic.

Considering `[1,2]`, a parse tree looks like:

.. code-block::

    Node(type="array", slice=[0,5], children=[
        Node(type="left_paren", slice=[0,1]),
        Node(type="number", slice=[1,2]),
        Node(type="comma", slice=[2,3]),
        Node(type="number", slice=[3,4]),
        Node(type="right_paren", slice=[4,5]),
    ])

While an abstract syntax tree looks like:

.. code-block::

    Node(type="array", slice=[0,5], children=[
        Node(type="number", slice=[1,2]),
        Node(type="number", slice=[3,4]),
    ])

e.g, left paren, comma, right paren are ignored as they're not part of the semantic of array.

You can consider abstract syntax tree as a polished version of parse tree. 

Peppa PEG provides two approaches to modify the tree:

* Use decorators `@` to drop, lift, squash nodes when providing the grammar.
* Use callbacks to manipulate nodes after the tree is produced.

By reshaping the tree, you can produce whichever kind of tree.

Runtime v/s Compile-time
------------------------

Unlike one of Peppa PEG alternatives - peg/leg, Peppa PEG doesn't end up with producing C code for the grammar. It holds the grammar and the parser generator in memory so you don't need an extra compilation step. You will use Peppa PEG in a tree-steps:

1. Load Grammar. Load your grammar using :c:func:`P4_LoadGrammar` . Alternatively, you can create your grammar using :c:func:`P4_CreateGrammar` and then add grammar rules using :ref:`lowlevel`.
2. Parse Source. Wrap source string using :c:func:`P4_CreateSource` and then parse it using :c:func:`P4_Parse`.
3. Traverse AST. It's up to you what to do when traversing the tree. You may inspect the tree, produce bytecodes, trim the tree, etc.
