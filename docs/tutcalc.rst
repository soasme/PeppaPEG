.. _tutcalc:

Tutorial: Calculator
====================

In this tutorial, we will write a calculator. It supports

* Positive integers.
* Negative integers.
* Addition operator `+`.
* Subtraction operator `-`.
* Multiplication operator `*`.
* Division operator `/`.
* Zero Division is invalid.
* Parenthesis `()`.

The calculator can take arbitrary valid string calc expressions and yield the result.

Step 1: Create Grammar
----------------------

Let's create a new file "calc.h" and
create the grammar using :c:func:`P4_LoadGrammar`.

The order of rules must be same with `P4_CalcRuleID`.

.. literalinclude:: ../examples/calc.h
   :lines: 53-82

To extend a little bit,

* Rule `statement` is the entry.
* Rule `term` can parse inputs like `a + b - c + d ...` or `a`.
* Rule `factor` can parse inputs like `a * b / c * d ...` or `a`.
* Rule `unary` can parse inputs like `+a`, `++a`, `-a`, `a`, etc.
* Rule `primary` can parse inputs like `(a)`, etc.
* Rule `integer` are some squashed digits. It avoids creating nodes for each digit.
* Rule `whitespace` allows arbitrary number of whitespace or tab in between expressions.

Step 2: Eval Ast
----------------

Next, let's evaluate the tree. We traverse the AST and calculate the result.

.. literalinclude:: ../examples/calc.h
   :lines: 84-146

Step 3: Parse
-------------

Let's create a new file "calc.c" and parse inputs reading from stdin.

The main function does the below things:

* Create the grammar.
* Create the source.
* Parse the source using the grammar.
* Eval the source ast.
* Print the evaluated result.

.. literalinclude:: ../examples/calc.c

Run:

.. code-block:: console

    $ gcc -o calc calc.c peppapeg.c && ./calc
    Type statement to continue. Type ^C to quit.

    1+2*3;
    [Out] 7

    -1 + 4/2*3 - 1;
    [Out] 4

    5/0;
    error: eval: 6
