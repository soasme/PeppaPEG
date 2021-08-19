<h1 align="center">✨ Peppa PEG 🐷 ✨</h1>

<p align="center">An ultra lightweight PEG Parser in ANSI C.</p>

```
  ____                          ____  _____ ____
 |  _ \ ___ _ __  _ __   __ _  |  _ \| ____/ ___|
 | |_) / _ \ '_ \| '_ \ / _` | | |_) |  _|| |  _
 |  __/  __/ |_) | |_) | (_| | |  __/| |__| |_| |
 |_|   \___| .__/| .__/ \__,_| |_|   |_____\____|
           |_|   |_|
```

[![Test | Build Status](https://github.com/soasme/PeppaPEG/actions/workflows/check.yml/badge.svg?branch=main)](https://github.com/soasme/PeppaPEG/actions/workflows/check.yml)
[![Docs | Build Status](https://github.com/soasme/PeppaPEG/actions/workflows/docs.yml/badge.svg?branch=main)](https://github.com/soasme/PeppaPEG/actions/workflows/docs.yml)
[![Docs](https://img.shields.io/badge/docs-soasme.com-green)](https://www.soasme.com/PeppaPEG)

# Hello, There!

Peppa PEG is an ultra lightweight [PEG] (parsing expression grammar) parser in ANSI C.

References: [GitHub](https://github.com/soasme/PeppaPEG)
| [Project Home Page](https://soasme.com/PeppaPEG/landing.html)
| [Project Documentation Pages](https://soasme.com/PeppaPEG/).

## Installation

Assume your system has `cmake` installed, run

```
$ cd PeppaPEG/
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

Once installed, add include macro and start using the library!

```c
#include <peppa.h>
```

You can use pkg-config to link the library:

```c
$ gcc `pkg-config --cflags --libs libpeppa` example.c
```

## Copy `peppa.h` / `peppa.c`

Peppa PEG has a header file and a C file, so alternatively you can add
it to your project by copying files "peppa.h" and "peppa.c".

Once copied, add include macro and start using the library!

```c
#include "peppa.h"
```

You can manually load the library source:

```bash
$ gcc example.c peppa.c
```

# Usage

## CLI

Peppa PEG ships with a tiny utility: `peppa` to help develop a PEG grammar.

Example: given files: `json.peg` and `data.json`, run with `peppa` utility:

```bash
$ cat json.peg
@lifted entry = &. value !.;
@lifted value = object / array / string / number / true / false / null;
object = "{" (item ("," item)*)? "}";
item = string ":" value;
array = "[" (value ("," value)*)? "]";
@tight string = "\"" ([\u0020-\u0021] / [\u0023-\u005b] / [\u005d-\U0010ffff] / escape )* "\"";
true = "true";
false = "false";
null = "null";
@tight @squashed number = minus? integral fractional? exponent?;
@tight @squashed @lifted escape = "\\" ("\"" / "/" / "\\" / "b" / "f" / "n" / "r" / "t" / unicode);
@tight @squashed unicode = "u" ([0-9] / [a-f] / [A-F]){4};
minus = "-";
plus = "+";
@squashed @tight integral = "0" / [1-9] [0-9]*;
@squashed @tight fractional = "." [0-9]+;
@tight exponent = i"e" (plus / minus)? [0-9]+;
@spaced @lifted whitespace = " " / "\r" / "\n" / "\t";

$ cat data.json
[{"numbers": [1,2.0,3e1]},[true,false,null],"xyz"]

$ peppa ast -g json.peg -e entry data.json | python3 ./scripts/gendot.py | dot -Tsvg -o/tmp/data.svg
```

![Example JSON AST](docs/_static/readme-json-ast2.svg)

## C API

In Peppa PEG, grammar syntax can be loaded from a string. Below is an example of JSON grammar syntax.

```c
P4_Grammar* grammar = P4_LoadGrammar(
    "@lifted\n"
    "entry = &. value !.;\n"

    "@lifted\n"
    "value = object / array / string / number / true / false / null;\n"

    "object = \"{\" (item (\",\" item)*)? \"}\";\n"
    "item = string \":\" value;\n"

    "array = \"[\" (value (\",\" value)*)? \"]\";\n"

    "@tight\n"
    "string = \"\\\"\" ([\\u0020-\\u0021] / [\\u0023-\\u005b] / [\\u005d-\\U0010ffff] / escape )* \"\\\"\";\n"

    "true = \"true\";\n"
    "false = \"false\";\n"
    "null = \"null\";\n"

    "@tight @squashed\n"
    "number = minus? integral fractional? exponent?;\n"

    "@tight @squashed @lifted\n"
    "escape = \"\\\\\" (\"\\\"\" / \"/\" / \"\\\\\" / \"b\" / \"f\" / \"n\" / \"r\" / \"t\" / unicode);\n"

    "@tight @squashed"
    "unicode = \"u\" ([0-9] / [a-f] / [A-F]){4};\n"

    "minus = \"-\";\n"
    "plus = \"+\";\n"

    "@squashed @tight\n"
    "integral = \"0\" / [1-9] [0-9]*;\n"

    "@squashed @tight\n"
    "fractional = \".\" [0-9]+;\n"

    "@tight"
    "exponent = i\"e\" (plus / minus)? [0-9]+;\n"

    "@spaced @lifted\n"
    "whitespace = \" \" / \"\\r\" / \"\\n\" / \"\\t\";\n"
);
```

The input can be parsed via `P4_Parse`:

```c
P4_Source* source = P4_CreateSource("[{\"numbers\": [1,2.0,3e1]},[true,false,null],\"xyz\"]", "entry");
P4_Parse(grammar, source);
```

You can traverse the parse tree. For example, the below function
outputs the parse tree into JSON format:

```c
P4_Node* root = P4_GetSourceAST(source);
P4_JsonifySourceAst(stdout, root, NULL);
```

```javascript
[{"slice":[0,50],"type":"array","children":[
        {"slice":[1,25],"type":"object","children":[
            {"slice":[2,24],"type":"item","children":[
                {"slice":[2,11],"type":"string"},
                {"slice":[13,24],"type":"array","children":[
                    {"slice":[14,15],"type":"number"},
                    {"slice":[16,19],"type":"number"},
                    {"slice":[20,23],"type":"number"}]}]}]},
        {"slice":[26,43],"type":"array","children":[
            {"slice":[27,31],"type":"true"},
            {"slice":[32,37],"type":"false"},
            {"slice":[38,42],"type":"null"}]},
        {"slice":[44,49],"type":"string"}]}]
```

# Peppy Hacking Peppa PEG!

Read the documentation here: <https://soasme.com/PeppaPEG/>.

## Test

Peppa PEG test requires downloading the test framework Unity:

```bash
$ git submodule init
$ git submodule update
```

Assume you have `cmake` and `gcc` installed.

```bash
(root) $ mkdir -p build
(root) $ cd build
(build) $ cmake -DENABLE_CHECK=On ..
(build) $ make check
...
100% tests passed, 0 tests failed
```

If valgrind is installed, you can also run the test along with memory leak check.

```bash
(root) $ mkdir -p build
(root) $ cd build
(build) $ cmake -DENABLE_VALGRIND=ON ..
(build) $ make check
```

If you feel having a testing environment is hard, try docker:

```bash
$ docker run --rm -v `pwd`:/app -it ubuntu:latest bash
# apt-get install gcc gdb valgrind make cmake python3 python3-venv python3-pip doxygen
# mkdir -p build && cd build && cmake .. && make check
```

## Docs

Peppa PEG docs can be built via doxygen:

```bash
(root) $ cd build
(build) $ cmake -DENABLE_DOCS=On ..
(build) $ rm -rf docs && make docs
```

The outputs are stored on `build/docs`.

## Examples

* Write an INI Parser using Peppa PEG: [ini.h](examples/ini.h), [ini.c](examples/ini.c).
* Write a Mustache Parser using Peppa PEG: [mustache.h](examples/mustache.h).
* Write a JSON Parser using Peppa PEG: [json.h](examples/json.h).
* Write a Calculator Parser using Peppa PEG: [calc.h](examples/calc.h), [calc.c](examples/calc.c).
* Write a [Dot](https://graphviz.org/doc/info/lang.html) parser using Peppa PEG: [dot.h](examples/dot.h).

Made with ❤️  by [Ju](https://github.com/soasme).

[PEG]: https://en.wikipedia.org/wiki/Parsing_expression_grammar
