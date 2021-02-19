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
import peppapeg as P4

grammar = P4.Grammar()
grammar.add_sequence('entry', 'greeter', grammar.literal(' '), 'target')
grammar.add_literal('greeter', 'Hello')
grammar.add_literal('target', 'world')

```

To parse the text, use method `.parse()`:

```python
>>> ast = grammar.parse("Hello world")
>>> print(ast)
Token(0..11)
```

If something goes wrong, `MyGrammar()` or `.parse()` will raise an Exception, a subclass of `P4Error`.
