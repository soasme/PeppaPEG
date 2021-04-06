from _peppapeg import lib, ffi

#: The version of python-peppapeg.
__VERSION__ = '0.1.0'

#: The version of peppapeg clib.
__CLIB_VERSION__ = ffi.string(lib.P4_Version()).decode('utf-8')

## Low Level Interfaces


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

class AdvanceError(P4Error):
    pass

class InternalError(P4Error):
    pass

def raise_error(source=None, err=0, errmsg=""):
    if source:
        err = lib.P4_GetError(source)
        errmsg = to_string(lib.P4_GetErrorMessage(source))

    if err == lib.P4_MatchError:
        raise MatchError(errmsg)

    elif err == lib.P4_MemoryError:
        raise MemoryError(errmsg)

    elif err == lib.P4_NameError:
        raise NameError(errmsg)

    elif err == lib.P4_AdvanceError:
        raise AdvanceError(errmsg)

    elif err == lib.P4_ValueError:
        raise ValueError(errmsg)

    elif err == lib.P4_IndexError:
        raise IndexError(errmsg)

    elif err == lib.P4_KeyError:
        raise KeyError(errmsg)

    elif err == lib.P4_NullError:
        raise TypeError(errmsg)

    elif err == lib.P4_StackError:
        raise RecursionError(errmsg)

    elif err == lib.P4_InternalError:
        raise InternalError(errmsg)

    else:
        raise P4Error(f'err code={err}')


### Token

class Token:

    def __init__(self, slice, rule_id, children):
        self.slice = slice
        self.rule_id = rule_id
        self.children = children

    def __repr__(self):
        return f'Token({self.slice.start}..{self.slice.stop})'

    @classmethod
    def clone(cls, ctoken):
        if ctoken == ffi.NULL:
            return

        i = ctoken.slice.start.pos
        j = ctoken.slice.stop.pos
        _slice = slice(i, j)

        rule_id = ctoken.rule_id

        _children = []
        tmp = ctoken.head
        while tmp != ffi.NULL:
            _children.append(cls.clone(tmp))
            tmp = tmp.next

        return cls(_slice, rule_id , _children)

### Grammar

class Grammar:

    def __init__(self, rules):
        self._grammar = lib.P4_LoadGrammar(rules.encode('utf-8'))

    def __del__(self):
        lib.P4_DeleteGrammar(self._grammar)

    def parse(self, input, entry=""):
        if entry:
            expr = lib.P4_GetGrammarRuleByName(self._grammar, entry.encode('utf-8'))

            if expr == ffi.NULL:
                raise NameError(entry)

            entry_id = entry.id
        else:
            entry_id = 1

        source = lib.P4_CreateSource(input.encode('utf-8'), entry_id)

        if source == ffi.NULL:
            raise MemoryError("unable to create source.")

        try:
            err = lib.P4_Parse(self._grammar, source)

            if err != 0:
                raise_error(err=err)

            return Token.clone(lib.P4_GetSourceAst(source))
        finally:
            lib.P4_DeleteSource(source)
