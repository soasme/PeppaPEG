from _peppapeg import lib, ffi

#: The version of python-peppapeg.
__VERSION__ = '0.1.0'

#: The version of peppapeg clib.
__CLIB_VERSION__ = ffi.string(lib.P4_Version()).decode('utf-8')

### Utils

def _to_string(buffer):
    if buffer == ffi.NULL or buffer.size == 0:
        return ''

    s = ffi.string(buffer.data, buffer.size)
    return s.decode('utf-8', 'ignore')

### Errors

class P4Error(Exception):
    pass

class MatchError(P4Error):
    pass


def raise_error(source=None, err=0, errmsg=""):
    if source:
        err = P4_GetError(source)
        errmsg = P4_GetErrorMessage(source)

    if err == lib.P4_MatchError:
        raise MatchError()

    elif err == lib.P4_MemoryError:
        raise MemoryError()

    else:
        raise P4Error(f'err code={err}')

### Expression

class Expression:
    """Base class for Expression."""

    def __del__(self):
        if self._expr:
            lib.P4_DeleteExpression(self._expr)

class Literal(Expression):
    """Literal Expression."""

    def __init__(self, literal):
        self.literal = literal
        self._expr = lib.P4_CreateLiteral(
            ffi.new('char[]', literal.encode('utf-8')),
            True,
        )

### Token

class Token:

    def __init__(self, slice, expr, children):
        self.slice = slice
        self.expr = expr
        self.children = children

    def __repr__(self):
        return f'Token({self.slice.start}..{self.slice.stop})'

    @classmethod
    def clone(cls, ctoken):
        i = ctoken.slice.i
        j = ctoken.slice.j
        _slice = slice(i, j)

        _expr = ctoken.expr

        _children = []
        tmp = ctoken.head
        while tmp != ffi.NULL:
            _children.append(cls.clone(tmp))
            tmp = tmp.next

        return cls(_slice, _expr, _children)

### Grammar

class Grammar:

    def __init__(self):
        self.names = {}
        self._grammar = lib.P4_CreateGrammar()

    def __del__(self):
        lib.P4_DeleteGrammar(self._grammar)

    def _resolve_rule_id(self, name):
        return self.names[name] + 1

    def _push_name(self, name):
        if name not in self.names:
            id = len(self.names) + 1
            self.names[name] = id
            return id
        raise P4Error()

    def _pop_name(self, name):
        del self.names[name]

    def add_literal(self, name, literal, sensitive=True):
        _id = self._push_name(name)

        err = lib.P4_AddLiteral(
            self._grammar,
            _id,
            literal.encode('utf-8'),
            sensitive
        )

        if err != lib.P4_Ok:
            raise_error(err, "Failed to add literal rule {name}")

    def literal(self, literal, sensitive=True):
        expr = lib.P4_CreateLiteral(_to_string(literal), sensitive)

        if err == lib.NULL:
            raise_error(lib.P4_MemoryError, "Failed to add literal rule {name}")

        return expr

    def parse(self, input, entry=None):
        source = lib.P4_CreateSource(input.encode('utf-8'), 1)

        if source == ffi.NULL:
            raise MemoryError()

        try:

            err = lib.P4_Parse(self._grammar, source)

            if err != lib.P4_Ok:
                raise P4Error()

            ast = lib.P4_GetSourceAst(source)

            return (
                None
                if ast == ffi.NULL
                else Token.clone(ast)
            )

        finally:
            lib.P4_DeleteSource(source)
