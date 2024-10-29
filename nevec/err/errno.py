from dataclasses import dataclass
from enum import Enum

from nevec.lex.tok import Tok, Loc

class Errno(Enum):
    CLI = auto()
    SYNTAX = auto()
    UNTERMINATED_STR = auto(),
    INVALID_CHAR = auto(),
    INTEGER_TOO_LARGE = auto(),
    INVALID_EXPR = auto(),
    UNTERMINATED_PARENS = auto()
    UNAPPLICABLE_OP = auto()


class Err:
    def __init__(self, id: Errno, msg: str, loc: Loc):
        self.id: Errno = id
        self.msg: str = msg
        self.loc: Loc = loc


class SyntaxErr(Err):
    def __init__(self, tok: Tok):
        self.id = Errno.SYNTAX
        self.msg = "unexpected character"
        self.loc = tok.loc
        self.tok = tok

