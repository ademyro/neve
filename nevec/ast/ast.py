from nevec.ast.type import Type, Types
from nevec.lex.tok import Tok, TokType, Loc

from enum import auto, Enum
from dataclasses import dataclass

from typing import Self

class Ast:
    def __init__(self, type: Type, loc: Loc):
        self.type = type
        self.loc = loc


class Expr(Ast):
    def __init__(self, type: Type, loc: Loc):
        self.type = type
        self.loc = loc

    def infer_type(self) -> Type:
        return self.type


class Parens(Expr):
    def __init__(self, expr: Expr, loc: Loc):
        self.expr = expr
        self.loc = loc
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return self.expr.infer_type()

    def __repr__(self):
        return f"({self.expr})"


class UnOp(Expr):
    class Op(Enum):
        NEG = auto()
        NOT = auto()


    def __init__(self, op: Op, expr: Expr, loc: Loc):
        self.op = op
        self.expr = expr
        self.loc = loc
        self.type = self.infer_type()

    def infer_type(self):
        return self.expr.infer_type()

    def __repr__(self):
        op = (
            "-"
            if self.op == self.Op.NEG
            else "not "
        )

        return f"{op}{self.expr}"

class BinOp(Expr):
    class BinOpType(Enum):
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

    def __init__(
        self, 
        left: Expr, 
        op: BinOpType, 
        right: Expr, 
        tok: Tok, 
        loc: Loc,
    ):
        self.left = left
        self.op = op
        self.right = right
        self.tok = tok
        self.loc = loc

        self.type = self.infer_type()

    @staticmethod
    def from_tok(tok: Tok):
        return BinOp.BinOpType(tok.type.value - TokType.MINUS.value)

    def infer_type(self) -> Type:
        if (
            self.op == self.BinOpType.PLUS and
            self.left.type == Types.STR and
            self.right.type == Types.STR
        ):
            return Types.STR
        
        if self.op == self.BinOpType.SLASH:
            return Types.FLOAT
        
        if (
            self.op.value >= self.BinOpType.SHL.value and
            self.op.value <= self.BinOpType.BIT_XOR.value
        ):
            return Types.INT
        
        if (
            self.op.value >= self.BinOpType.NEQ.value and
            self.op.value <= self.BinOpType.LTE.value
        ):
            return Types.BOOL

        # otherwise, we're dealing with (-) or (*)
        if self.left.type == self.right.type:
            return self.left.type
        
        if (
            self.left.type == Types.FLOAT or
            self.right.type == Types.FLOAT
        ):
            return Types.FLOAT

        return Types.UNKNOWN

    def __repr__(self):
        return f"{self.left} {self.tok.lexeme} {self.right}"

class Bitwise(BinOp):
    def infer_type(self) -> Type:
        # TODO: replace all these checks with idea implementation checks
        if (
            self.left.type.truly_isnt(Types.INT) or
            self.right.type.truly_isnt(Types.INT)
        ):
            return Types.UNKNOWN 

        if self.left.type != self.right.type:
            return Types.UNKNOWN

        return Types.INT.unless_unknown(self.left.type, self.right.type)


class Comparison(BinOp):
    def infer_type(self) -> Type:
        if self.left.type.truly_isnt(self.right.type):
            return Types.UNKNOWN

        return self.left.type.unless_unknown(self.left.type, self.right.type)


class Term(BinOp):
    def infer_type(self) -> Type:
        if self.left.type.truly_isnt(self.right.type):
            return Types.UNKNOWN

        return self.left.type.unless_unknown(self.left.type, self.right.type)


class Factor(BinOp):
    def infer_type(self) -> Type:
        if self.left.type.truly_isnt(self.right.type):
            return Types.UNKNOWN

        return self.left.type.unless_unknown(self.left.type, self.right.type)


class Int(Expr):
    def __init__(self, value: int, loc: Loc):
        self.value = value
        self.loc = loc

        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.INT

    def __repr__(self):
        return str(int(self.value))
        

class Float(Expr):
    def __init__(self, value: float, loc: Loc):
        self.value = value
        self.loc = loc

        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.FLOAT

    def __repr__(self):
        return str(self.value)
        

class Bool(Expr):
    def __init__(self, value: bool, loc: Loc):
        self.value = value
        self.loc = loc

        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.BOOL

    def __repr__(self):
        return str(self.value).lower()


class Str(Expr):
    def __init__(self, value: str, loc: Loc):
        self.value = value
        self.loc = loc

        self.type = self.infer_type()

    @staticmethod
    def empty():
        return Str("", Loc.new())

    @staticmethod
    def trim_quotes(value: str):
        begin = 1 if value[0] == "\"" else 0
        end = -1 if value[-1] == "\"" else len(value)

        return value[begin:end]
    
    def infer_type(self) -> Type:
        return Types.STR

    def __repr__(self):
        return f"\"{self.value}\""

class Interpol(Expr):
    def __init__(self, left: str, expr: Expr, next: Self | Str, loc: Loc):
        self.left = left
        self.expr = expr
        self.next = next
        self.loc = loc

        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.STR

    def __repr__(self):
        return "".join(
            [
                "\"", 
                self.left, 
                "#{", 
                str(self.expr), 
                "}", 
                Str.trim_quotes(str(self.next)), 
                "\""
            ]
        )

class Nil(Expr):
    def __init__(self, loc: Loc):
        self.loc = loc

        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.NIL

    def __repr__(self):
        return "nil"
