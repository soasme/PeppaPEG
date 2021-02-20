from enum import IntEnum
import pytest
import peppapeg as P4

def test_literal():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_literal(Rule.entry, 'hello world')
    assert grammar.parse("hello world").slice == slice(0, 11)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("hello,world")

def test_range():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_range(Rule.entry, 'a', 'z')
    assert grammar.parse("x").slice == slice(0, 1)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("1")

def test_sequence():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_sequence(Rule.entry,
        grammar.literal('a'),
        grammar.literal('b'),
        grammar.literal('c'),
    )
    assert grammar.parse("abc").slice == slice(0, 3)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("cba")

def test_choice():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_choice(Rule.entry,
        grammar.literal('a'),
        grammar.literal('b'),
        grammar.literal('c'),
    )
    assert grammar.parse("a").slice == slice(0, 1)

    with pytest.raises(P4.MatchError):
        grammar.parse("d")

def test_reference():
    class Rule(IntEnum):
        entry = 1
        dest  = 2

    grammar = P4.Grammar()
    grammar.add_literal(Rule.dest, 'world')
    grammar.add_sequence(Rule.entry, grammar.literal('hello '), grammar.reference(Rule.dest))
    ast = grammar.parse("hello world", entry=Rule.entry)
    assert ast.slice == slice(0, 11)
    assert ast.children
    assert ast.children[0].slice == slice(6, 11)

def test_back_reference():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_sequence(Rule.entry,
        grammar.literal('o'),
        grammar.literal('x'),
        grammar.back_reference(0),
    )
    assert grammar.parse("oxo").slice == slice(0, 3)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("oxx")

def test_positive():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_positive(Rule.entry,
        grammar.literal('o'),
    )
    assert grammar.parse("o") is None

    with pytest.raises(P4.MatchError):
        assert grammar.parse("x")

def test_negative():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_negative(Rule.entry,
        grammar.literal('o'),
    )
    assert grammar.parse("x") is None

    with pytest.raises(P4.MatchError):
        assert grammar.parse("o")

def test_zero_or_more():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_zero_or_more(Rule.entry,
        grammar.literal('o'),
    )
    assert grammar.parse("") is None
    assert grammar.parse("o").slice == slice(0, 1)
    assert grammar.parse("oooo").slice == slice(0, 4)

def test_once_or_more():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_once_or_more(Rule.entry,
        grammar.literal('o'),
    )
    assert grammar.parse("o").slice == slice(0, 1)
    assert grammar.parse("oooo").slice == slice(0, 4)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("") is None

def test_zero_or_once():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_zero_or_once(Rule.entry,
        grammar.literal('o'),
    )
    assert grammar.parse("") is None
    assert grammar.parse("o").slice == slice(0, 1)

def test_repeat_min():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_repeat_min(Rule.entry,
        grammar.literal('o'),
        3
    )
    assert grammar.parse("ooo").slice == slice(0, 3)
    assert grammar.parse("oooo").slice == slice(0, 4)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("") is None

    with pytest.raises(P4.MatchError):
        assert grammar.parse("oo").slice == slice(0, 2)


def test_repeat_max():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_repeat_max(Rule.entry,
        grammar.literal('o'),
        3
    )

    assert grammar.parse("") is None
    assert grammar.parse("oo").slice == slice(0, 2)
    assert grammar.parse("ooo").slice == slice(0, 3)
    assert grammar.parse("oooo").slice == slice(0, 3)

def test_repeat():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_repeat_min_max(Rule.entry,
        grammar.literal('o'),
        2,
        3
    )

    assert grammar.parse("oo").slice == slice(0, 2)
    assert grammar.parse("ooo").slice == slice(0, 3)
    assert grammar.parse("oooo").slice == slice(0, 3)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("") is None

    with pytest.raises(P4.MatchError):
        assert grammar.parse("o") == slice(0, 1)

def test_repeat_exact():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_repeat_exact(Rule.entry,
        grammar.literal('o'),
        3
    )

    assert grammar.parse("ooo").slice == slice(0, 3)
    assert grammar.parse("oooo").slice == slice(0, 3)

    with pytest.raises(P4.MatchError):
        grammar.parse("oo")

def test_lifted():
    class Rule(IntEnum):
        entry = 1

    grammar = P4.Grammar()
    grammar.add_literal(Rule.entry, 'hello world')
    grammar.set_flag(Rule.entry, P4.LIFTED)

    assert grammar.parse("hello world") is None

def test_squashed():
    class Rule(IntEnum):
        entry = 1
        dest  = 2

    grammar = P4.Grammar()
    grammar.add_literal(Rule.dest, 'world')
    grammar.add_sequence(Rule.entry, grammar.literal('hello '), grammar.reference(Rule.dest))
    grammar.set_flag(Rule.entry, P4.SQUASHED)
    ast = grammar.parse("hello world", entry=Rule.entry)
    assert ast.slice == slice(0, 11)
    assert ast.children == []

def test_spaced():
    class Rule(IntEnum):
        entry = 1
        whitespace = 2

    grammar = P4.Grammar()
    grammar.add_literal(Rule.whitespace, ' ')
    grammar.set_flag(Rule.whitespace, P4.SPACED)
    grammar.add_sequence(Rule.entry, grammar.literal('o'), grammar.literal('x'))
    ast = grammar.parse("o x", entry=Rule.entry)
    assert ast.slice == slice(0, 3)
