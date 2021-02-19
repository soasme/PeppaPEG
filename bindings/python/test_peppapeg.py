import pytest
import peppapeg as P4

def test_literal():
    grammar = P4.Grammar()
    grammar.add_literal('entry', 'hello world')
    assert grammar.parse("hello world").slice == slice(0, 11)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("hello,world")

def test_range():
    grammar = P4.Grammar()
    grammar.add_range('entry', 'a', 'z')
    assert grammar.parse("x").slice == slice(0, 1)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("1")

def test_sequence():
    grammar = P4.Grammar()
    grammar.add_sequence('entry',
        grammar.literal('a'),
        grammar.literal('b'),
        grammar.literal('c'),
    )
    assert grammar.parse("abc").slice == slice(0, 3)

    with pytest.raises(P4.MatchError):
        assert grammar.parse("cba")

def test_choice():
    grammar = P4.Grammar()
    grammar.add_choice('entry',
        grammar.literal('a'),
        grammar.literal('b'),
        grammar.literal('c'),
    )
    assert grammar.parse("a").slice == slice(0, 1)

    with pytest.raises(P4.MatchError):
        grammar.parse("d")

def test_reference():
    grammar = P4.Grammar()
    grammar.add_literal('dest', 'world')
    grammar.add_sequence('entry', grammar.literal('hello '), grammar.reference('dest'))
    print(grammar.parse("hello world", entry='entry'))
