.. _getting_started:

Getting Started
===============

Header File and Source File
--------------------------------

Peppa PEG has a header file and a C file, so you can add it to your project by copying files `peppapeg.h` and `peppapeg.c`.

Peppa PEG assumes your project is ANSI C (C89, or C90) compatible.

Once copied, add include macro and start using the library::

    #include "peppapeg.h"

Data Structures
---------------

All of the data structures and functions provided by Peppa PEG start with `P4_`
(Count the number of character "P" in **P** e **p** **p** a **P** E G).

Let's learn some basic P4 data structures:

* :c:struct:`P4_Grammar`: The grammar object that defines all grammar rules.
* :c:struct:`P4_Expression`: The grammar rule object.
* :c:struct:`P4_Source`: The content to parse.
* :c:struct:`P4_Node`. The node of an AST (abstract syntax tree), e.g. the final parsing result of a grammar given the source.

Step 1: Create Grammar
----------------------

In Peppa PEG, we always start with creating a :c:struct:`P4_Grammar`. We create such a data structure using :c:func:`P4_LoadGrammar`.

.. literalinclude:: ../example.c
   :lines: 7-11

Step 2: Create Source
---------------------

We create a :c:struct:`P4_Source` using :c:func:`P4_CreateSource()`.

.. literalinclude:: ../example.c
   :lines: 4,13-17

The first parameter is the content of the source.
The second parameter is the ID of the entry grammar rule, e.g. the numeric index of the rule in the loaded grammar (1-based).

In this example, we have only single rule `entry`, so the ID is 1. (If you have multiple rules, the rule IDs increments.)

Step 3: Parse
-------------

Now the stage is setup; call :c:func:`P4_Parse`. If everything is okay, it returns a zero value - :c:enum:`P4_Ok<P4_Error>`.

.. literalinclude:: ../example.c
   :lines: 19-26

Step 4: Traverse AST
---------------------------

P4_Source contains a tree if parse successfully. We get the root node of such a tree using :c:func:`P4_GetSourceAst()`.

To traverse the AST,

* `node->head` is the first children.
* `node->tail` is the last children.
* `node->next` is the next sibling.
* `node->slice.start` is the start position in the source string that the slice covers.
* `node->slice.stop` is the end position in the source string that the slice covers.
* :c:func:`P4_CopyNodeString()` returns the string the AST node covers.

.. literalinclude:: ../example.c
   :lines: 28-39

It may be helpful to output the source AST in JSON format:

.. literalinclude:: ../example.c
   :lines: 41

Step 5: Clean Up
----------------

Last but not least, don't forget to free all the allocated memory.

* :c:func:`P4_DeleteSource()` deletes the source along with the entire tree.
* :c:func:`P4_DeleteGrammar()` deletes the grammar along with all the grammar rules.

.. literalinclude:: ../example.c
   :language: c
   :lines: 43-44

Full Example Code
-----------------

The complete code for this example:

.. literalinclude:: ../example.c

The output of the example looks like:

.. code-block::

    $ gcc -o example ../example.c ../peppapeg.c
    $ ./example
    root span: [0 11]
    root start: line=1 offset=0
    root stop: line=2 offset=5
    root next: (nil)
    root head: (nil)
    root tail: (nil)
    root text: Hello
    WORLD
    [{"slice":[0,11],"type":"entry"}]

Conclusion
----------

In this tutorial, we walk through the basic data structures and combine them in one example. The example parses nothing but "Hello World" to a single node.

I hope this example can get you a basic understanding of Peppa PEG. Now you can go back to :ref:`main` and pick more docs to read!
