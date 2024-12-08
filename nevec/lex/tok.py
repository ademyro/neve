from dataclasses import dataclass
from enum import Enum, auto
from typing import Optional, Self

@dataclass
class Loc:
    col: int
    line: int
    length: int

    @staticmethod
    def new():
        return Loc(0, 1, 0)

    def copy(self):
        return Loc(self.col, self.line, self.length)
    
    def advance(self):
        self.length += 1

    def newline(self):
        self.col = 0
        self.line += 1

    def sync(self):
        self.col += self.length
        self.length = 0

    def __eq__(self, other: Self):
        return (
            self.col == other.col and
            self.line == other.line and
            self.length == other.length
        )

    def __repr__(self):
        return f"{self.line}:{self.col}"


class TokType(Enum):
    SEMICOL = auto()
    COMMA = auto()
    DOT = auto()
    DOT_DOT = auto()
    
    MINUS = auto()  
    PLUS = auto()
    STAR = auto()
    SLASH = auto()
    
    SHL = auto()
    SHR = auto()
    BIT_AND = auto()
    BIT_XOR = auto()
    BIT_OR = auto()

    NEQ = auto()
    EQ = auto()
    GT = auto()
    GTE = auto()
    LT = auto()
    LTE = auto()

    ASSIGN = auto()

    EXCLAM = auto()
    QUESTION = auto()

    AND = auto()
    CLASS = auto()
    DO = auto()
    ELSE = auto()
    END = auto()
    ENUM = auto()
    FOR = auto()
    FUN = auto()
    IF = auto()
    LET = auto()
    MATCH = auto()
    OR = auto()
    PUTS = auto()
    RETURN = auto()
    UNION = auto()
    VAR = auto()
    WHILE = auto()

    LPAREN = auto()
    RPAREN = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    PIPE = auto()

    ID = auto()
    STR = auto()
    INT = auto() 
    FLOAT = auto()
    INTERPOL = auto()

    FALSE = auto()
    NIL = auto()
    NOT = auto()
    SELF = auto()
    TRUE = auto()
    WITH = auto()

    NEWLINE = auto()
    ERR = auto()
    EOF = auto()

    @staticmethod
    def match(seq: str) -> Optional["TokType"]:
        return TokTypes.TOKS.get(seq) 

    @staticmethod
    def match_keyword(id: str) -> Optional["TokType"]:
        return TokTypes.KEYWORDS.get(id)

class TokTypes:
    KEYWORDS = {
        "and": TokType.AND,
        "bitor": TokType.BIT_OR,
        "class": TokType.CLASS,
        "do": TokType.DO,
        "else": TokType.ELSE,
        "end": TokType.END,
        "enum": TokType.ENUM,
        "false": TokType.FALSE,
        "for": TokType.FOR,
        "fun": TokType.FUN,
        "if": TokType.IF,
        "let": TokType.LET,
        "match": TokType.MATCH,
        "nil": TokType.NIL,
        "not": TokType.NOT,
        "or": TokType.OR,
        "puts": TokType.PUTS,
        "return": TokType.RETURN,
        "self": TokType.SELF,
        "true": TokType.TRUE,
        "union": TokType.UNION,
        "var": TokType.VAR,
        "while": TokType.WHILE,
        "with": TokType.WITH
    }

    TOKS = {
        ";": TokType.SEMICOL,
        ",": TokType.COMMA,
        ".": TokType.DOT,
        "..": TokType.DOT_DOT,
        
        "-": TokType.MINUS,
        "+": TokType.PLUS,
        "*": TokType.STAR,
        "/": TokType.SLASH,
        
        "<<": TokType.SHL,
        ">>": TokType.SHR,
        "&": TokType.BIT_AND,
        "^": TokType.BIT_XOR,

        "!=": TokType.NEQ,
        "=": TokType.ASSIGN,
        "==": TokType.EQ,
        ">": TokType.GT,
        ">=": TokType.GTE,
        "<": TokType.LT,
        "<=": TokType.LTE,

        "!": TokType.EXCLAM,
        "?": TokType.QUESTION,

        "(": TokType.LPAREN,
        ")": TokType.RPAREN,
        "[": TokType.LBRACKET,
        "]": TokType.RBRACKET,
        "|": TokType.PIPE
    }

@dataclass
class Tok:
    type: TokType 
    lexeme: str
    loc: Loc
    value: Optional[any] = None

    @staticmethod
    def eof() -> "Tok":
        return Tok(TokType.EOF, "", Loc.new())
