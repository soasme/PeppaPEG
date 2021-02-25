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

* **P4_Grammar**: The grammar object that defines all grammar rules.
* **P4_Expression**: The grammar rule object.
* **P4_Source**: The content to parse.
* **P4_Token**. The building block of an AST (abstract syntax tree), e.g. the final parsing result of a grammar given the source.

Step 1: Create Grammar
----------------------

In Peppa PEG, we always start with creating a P4_Grammar. We create such a data structure using **P4_CreateGrammar**.

.. code-block:: c

   P4_Grammar* grammar = P4_CreateGrammar();
   if (grammar == NULL) {
        printf("Error: CreateGrammar: MemoryError.\n");
        return 1;
   }

Step 2: Add Grammar Rule
-------------------------

There are plenty kinds of rules in PEG. In this example, we use the simplest, **P4_Literal**. We add such a rule using **P4_AddLiteral**.

.. code-block:: c

    if (P4_AddLiteral(grammar, 1, "HelloWorld", false) != P4_Ok) {
        printf("Error: AddLiteral: MemoryError.\n");
        return 1;
    }

The second parameter is a numeric ID for the grammar rule. Each rule has its unique ID. The third parameter is the exact string to match. The fourth parameter determines if the string is case sensitive.

Step 3: Create Source
---------------------

We create P4_Source using **P4_CreateSource**.

.. code-block:: c

    P4_Source*  source = P4_CreateSource("HelloWorld", 1);
    if (source == NULL) {
        printf("Error: CreateSource: MemoryError.\n");
        return 1;
    }

The first parameter is the content of the source. The second parameter is the ID of the entry grammar rule.

Step 4: Parse
-------------

Now the stage is setup; call **P4_Parse**. If everything is okay, it returns a zero value - `P4_Ok`.

.. code-block:: c

    if (P4_Parse(grammar, source) != P4_Ok) {
        printf("Error: Parse failed.\n");
        exit(1);
    }

Step 5: Traverse Token Tree
---------------------------

P4_Source contains a token tree if parse successfully. We get the root node of such a token tree using **P4_GetSourceAst**.


.. code-block:: c

    P4_Token* node   = P4_GetSourceAst(source);
    char*     text   = P4_CopyTokenString(node);

To traverse the AST,

* `node->head` is the first children.
* `node->tail` is the last children.
* `node->next` is the next sibling.
* `node->slice.i` is the start position in the source string that the token covers.
* `node->slice.j` is the end position in the source string that the token covers.
* Function `P4_CopyTokenString` returns the string the AST node covers.

.. code-block:: c

    printf("root span: [%lu %lu]\n", node->slice.i, node->slice.j);
    printf("root next: %p\n", node->next);
    printf("root head: %p\n", node->head);
    printf("root tail: %p\n", node->tail);
    printf("root text: %s\n", text);

    free(text);

Step 6: Clean Up
----------------

Last but not least, don't forget to free all the allocated memory.

* **P4_DeleteSource** deletes the source along with the entire token tree.
* **P4_DeleteGrammar** deletes the grammar along with all the grammar rules.

.. code-block:: c

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);


Full Example Code
-----------------

The complete code for this example:

.. code-block:: c

    #include <stdio.h>
    #include "peppapeg.h"

    # define ENTRY 1

    int main(int argc, char* argv[]) {
        P4_Grammar* grammar = P4_CreateGrammar();
        if (grammar == NULL) {
            printf("Error: CreateGrammar: MemoryError.\n");
            return 1;
        }

        if (P4_AddLiteral(grammar, ENTRY, "HelloWorld", false) != P4_Ok) {
            printf("Error: AddLiteral: MemoryError.\n");
            return 1;
        }

        P4_Source*  source = P4_CreateSource("HelloWorld", ENTRY);
        if (source == NULL) {
            printf("Error: CreateSource: MemoryError.\n");
            return 1;
        }

        if (P4_Parse(grammar, source) != P4_Ok) {
            printf("Error: Parse: ErrCode[%lu] Message[%s]\n",
                P4_GetError(source),
                P4_GetErrorMessage(source)
            );
            return 1;
        }

        P4_Token*   root = P4_GetSourceAst(source);
        char*       text = P4_CopyTokenString(root);

        printf("root span: [%lu %lu]\n", root->slice.i, root->slice.j);
        printf("root next: %p\n", root->next);
        printf("root head: %p\n", root->head);
        printf("root tail: %p\n", root->tail);
        printf("root text: %s\n", text);

        free(text);
        P4_DeleteSource(source);
        P4_DeleteGrammar(grammar);

        return 1;
    }

Conclusion
----------

In this tutorial, we walk through the basic data structures and combine them in one example. The example parses nothing but "HelloWorld" to a single token.

I hope this example can get you a basic understanding of Peppa PEG. Now you can go back to :ref:`main` and pick more docs to read!
