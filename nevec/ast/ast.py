from nevec.ast.type import Type, Types
from nevec.lex.tok import Tok, TokType, Loc

from enum import auto, Enum

from typing import Self, Optional

class Ast:
    def __init__(self, type: Type, loc: Loc):
        self.type = type
        self.loc = loc


class Program(Ast):
    def __init__(self, expr: "Expr"):
        self.expr = expr

    def infer_type(self) -> Type:
        return self.expr.infer_type()

    def __repr__(self) -> str:
        return str(self.expr)


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


class Op(Expr):
    ...


class UnOp(Op):
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


class BinOp(Op):
    class Op(Enum):
        PLUS = auto()
        MINUS = auto()  
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

        CONCAT = auto()


    def __init__(
        self, 
        left: Expr, 
        op: Op, 
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
        return BinOp.Op(tok.type.value - TokType.PLUS.value + 1)

    def infer_type(self, base_type: Optional[Type]=None) -> Type:
        if self.left.type != self.right.type:
            return Types.UNKNOWN

        base_type = base_type if base_type else self.left.type 

        return base_type.unless_unknown(self.left.type, self.right.type)

    def __repr__(self):
        return f"{self.left} {self.tok.lexeme} {self.right}"


class Bitwise(BinOp):
    def infer_type(self, base_type=Types.INT) -> Type:
        if (
            self.left.type != Types.INT or
            self.right.type != Types.INT
        ):
            return Types.UNKNOWN

        return super().infer_type(base_type)


class Comparison(BinOp):
    def infer_type(self, base_type=Types.BOOL) -> Type:
        return super().infer_type(base_type)


class Arith(BinOp):
    def infer_type(self, base_type=None):
        _ = base_type

        if self.left.type != self.right.type:
            return Types.UNKNOWN

        if not self.left.type.is_num():
            return Types.UNKNOWN

        return self.left.type.unless_unknown(self.left.type, self.right.type)


class Concat(BinOp):
    def infer_type(self, base_type=None):
        _ = base_type

        if self.left.type != self.right.type:
            return Types.UNKNOWN

        if not self.left.type.is_str():
            return Types.UNKNOWN

        return self.left.type.unless_unknown(self.left.type, self.right.type)


class Show(Expr):
    def __init__(self, expr: Expr, loc: Loc):
        self.expr: Expr = expr
        self.loc: Loc = loc

        self.type: Type = self.infer_type()
    
    def infer_type(self) -> Type:
        return Types.STR

    def __repr__(self) -> str:
        return f"{self.expr}.show"


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
