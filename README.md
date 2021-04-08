# Peppa PEG

Ultra lightweight PEG Parser in ANSI C. ✨ 🐷 ✨.

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

[PEG], or parsing expression grammar, describes a programming language using rules.
In human words, it's for parsing programming languages.

Peppa PEG is an ultra lightweight PEG parser in ANSI C.

# Usage

## Copy `peppapeg.h` / `peppapeg.c`

Peppa PEG has a header file and a C file, so you can add
it to your project by copying files "peppapeg.h" and "peppapeg.c".

Peppa PEG assumes your project is ANSI C (C89, or C90) compatible.

Once copied, add include macro and start using the library!

```
#include "peppapeg.h"
```

## Load Grammar

In Peppa PEG, grammar syntax can be loaded from a string. Below is an example of JSON grammar syntax.

```c
P4_Grammar* json = P4_LoadGrammar(
    "@lifted\n"
    "entry = &[\\u{1}-\\u{10ffff}] value ![\\u{1}-\\u{10ffff}];\n"

    "@lifted\n"
    "value = object / array / string / number / true / false / null;\n"

    "object = \"{\" (item (\",\" item)*)? \"}\";\n"
    "item = string \":\" value;\n"

    "array = \"[\" (value (\",\" value)*)? \"]\";\n"

    "@tight\n"
    "string = \"\\\"\" ([\\u{20}-\\u{21}] / [\\u{23}-\\u{5b}] / [\\u{5d}-\\u{10ffff}] / escape )* \"\\\"\";\n"

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

## Parse

In Peppa PEG, the input can be parsed via `P4_Parse`:

```
P4_Source* source = P4_CreateSource("[1,2.0,3e1,true,false,null,\"xyz\",{},[]]" , 1);
P4_Parse(json, source);
```

If success, the parsed source will contain an AST:

```
P4_JsonifySourceAst(json, stdout, root);
```

```
[
    {"slice":[0,39],"type":"array","children":[
        {"slice":[1,2],"type":"number"},
        {"slice":[3,6],"type":"number"},
        {"slice":[7,10],"type":"number"},
        {"slice":[11,15],"type":"true"},
        {"slice":[16,21],"type":"false"},
        {"slice":[22,26],"type":"null"},
        {"slice":[27,32],"type":"string"},
        {"slice":[33,35],"type":"object"},
        {"slice":[36,38],"type":"array"}
    ]}
]
```

You can traverse the AST and do whatever you can. :)

Read the documentation here: <https://www.soasme.com/PeppaPEG/>.

# Peppy Hacking Peppa PEG!

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
(build) $ cmake ..
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
(build) $ rm -rf docs && make docs
```

The outputs are stored on `build/docs`.

## Examples

* Write an INI Parser using Peppa PEG: [ini.h](examples/ini.h), [ini.c](examples/ini.c).
* Write a Mustache Parser using Peppa PEG: [mustache.h](examples/mustache.h).
* Write a JSON Parser using Peppa PEG: [json.h](examples/json.h).
* Write a Calculator Parser using Peppa PEG: [calc.h](examples/calc.h), [calc.c](examples/calc.c).

Made with ❤️  by [Ju](https://github.com/soasme).

[PEG]: https://en.wikipedia.org/wiki/Parsing_expression_grammar
