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
    assert grammar.parse("hello world", entry=Rule.entry).slice == slice(0, 11)
