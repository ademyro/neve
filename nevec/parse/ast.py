from nevec.parse.type import Type, Types
from nevec.lex.tok import Tok, TokType

from enum import auto, Enum
from dataclasses import dataclass

@dataclass
class Ast:
    type: Type
    

class Expr(Ast):
    def infer_type(self) -> Type:
        ...


class Parens(Expr):
    def __init__(self, expr: Expr):
        self.expr = expr
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return self.expr.infer_type()

    def __repr__(self):
        return f"({self.expr})"


class UnOp(Expr):
    class UnOpType(Enum):
        NEG = auto()
        NOT = auto()


    def __init__(self, op: UnOpType, expr: Expr):
        self.op = op
        self.expr = expr
        self.type = self.infer_type()

    def infer_type(self):
        return self.expr.infer_type()

    def __repr__(self):
        op = (
            "-"
            if self.op == UnOp.UnOpType.NEG
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

    def __init__(self, left: Expr, op: BinOpType, right: Expr, tok: Tok):
        self.left = left
        self.op = op
        self.right = right
        self.tok = tok

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


class Int(Expr):
    def __init__(self, value: int):
        self.value = value
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.INT

    def __repr__(self):
        return str(int(self.value))
        

class Float(Expr):
    def __init__(self, value: float):
        self.value = value
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.FLOAT

    def __repr__(self):
        return str(self.value)
        

class Bool(Expr):
    def __init__(self, value: bool):
        self.value = value
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.BOOL

    def __repr__(self):
        return str(self.value).lower()


class Nil(Expr):
    def __init__(self):
        self.type = self.infer_type()

    def infer_type(self) -> Type:
        return Types.NIL

    def __repr__(self):
        return "nil"
