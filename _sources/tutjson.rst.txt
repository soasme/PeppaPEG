.. _tutjson:

Tutorial: JSON
==============

JSON, or JavaScript Object Notation, is a widely used data exchange format, using human readable text to store and transit data. As a subset of JavaScript, JSON itself is language independent.

There are quite several specs trying to standardize the JSON format, such as RFC 4527, RFC 7159, RFC 8259, ECMA 262, ECMA 404, ISO/IEC 21778:2017, etc. As hinted on `json.org <https://json.org>`_, ECMA-404 is now the JSON data interchange standard.

In this tutorial, we will write a JSON parser using Peppa PEG. There are already many JSON parsers in the wild, but learning how to write a complete JSON parser gives us a better understanding of Peppa PEG. In the end, you should be able to write a parser for other grammars, as the mindset to develop any new grammar using Peppa PEG is the same.

Step 1: Define Grammar
----------------------

Let's create a new file "json.h".

.. code-block:: c

   #include "peppa.h"

We'll create the grammar using :c:func:`P4_LoadGrammar`.
The grammar in Peppa PEG form is identical to the one described on page json.org.

.. literalinclude:: ../examples/json.h
   :lines: 42-86

Step 2: Parse
-------------

Let's create a new file "parse_json.c" and parse a JSON array.

The main function does below things:

* Create JSON grammar: :c:func:`P4_CreateJSONGrammar`.
* Create source: :c:func:`P4_CreateSource`.
* Parse: :c:func:`P4_Parse`.
* Get Root Token: :c:func:`P4_GetSourceAst`.
* Print AST to stdout: :c:func:`P4_JsonifySourceAst`.
* Clean-up: :c:func:`P4_DeleteSource` & :c:func:`P4_DeleteGrammar`.

.. literalinclude:: ../examples/parse_json.c
   :lines: 6-16

Run:

.. code-block:: console

   $ gcc ../examples/parse_json.c ../peppa.c
   $ ./a.out
   [{"slice":[0,39],"type":"array","children":[{"slice":[1,2],"type":"number"},{"slice":[3,6],"type":"number"},{"slice":[7,10],"type":"number"},{"slice":[11,15],"type":"true"},{"slice":[16,21],"type":"false"},{"slice":[22,26],"type":"null"},{"slice":[27,32],"type":"string"},{"slice":[33,35],"type":"object"},{"slice":[36,38],"type":"array"}]}]
