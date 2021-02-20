# Peppa PEG

A CFFI binding for [Peppa PEG](https://github.com/soasme/PeppaPEG), an ultra lightweight PEG parsing library.

## Installation

Install it via pip:

```bash
pip install python-peppapeg
```

Or manually:

```bash
python setup.py install
```

## Example

A example of how to define a grammar using python-peppapeg:

```python
from enum import IntEnum

import peppapeg as P4

class Rule(IntEnum):
    entry = 1
    greeter = 2
    target = 3

grammar = P4.Grammar()
grammar.add_sequence(Rule.entry,
    grammar.reference(Rule.greeter),
    grammar.literal(' '),
    grammar.reference('target')
)
grammar.add_literal(Rule.greeter, 'Hello')
grammar.add_literal(Rule.target, 'world')
```

To parse the text, use method `.parse()`:

```python
>>> ast = grammar.parse("Hello world")
>>> print(ast)
Token(0..11)
```

If something goes wrong, `MyGrammar()` or `.parse()` will raise an Exception, a subclass of `P4Error`.
